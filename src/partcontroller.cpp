#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include <qpopupmenu.h>
#include <qfile.h>
#include <qlayout.h>
#include <qmap.h>
#include <qlabel.h>

#include <kmimetype.h>
#include <kservice.h>
#include <ktrader.h>
#include <kapplication.h>
#include <krun.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kparts/part.h>
#include <kparts/factory.h>
#include <kparts/partmanager.h>
#include <kparts/browserextension.h>
#include <kfiledialog.h>
#include <kmainwindow.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <khtml_part.h>
#include <kpopupmenu.h>
#include <kio/netaccess.h>
#include <kdialogbase.h>
#include <klineedit.h>
#include <kshortcut.h>
#include <kcompletion.h>
#include <kdirwatch.h>

#include "toplevel.h"
#include "api.h"
#include "core.h"
#include "editorproxy.h"
#include "documentationpart.h"
#include "ksavealldialog.h"

#include "kdevproject.h"

#include "partcontroller.h"


PartController *PartController::s_instance = 0;

struct HistoryEntry {
    KURL url;
    QString context;

    HistoryEntry( const KURL& u, const QString& c ): url( u ), context( c ) {}
};

PartController::PartController(QWidget *parent)
  : KDevPartController(parent)
{
  dirWatcher = new KDirWatch( this );

  connect(this, SIGNAL(partRemoved(KParts::Part*)), this, SLOT(updateMenuItems()));
  connect(this, SIGNAL(partAdded(KParts::Part*)), this, SLOT(updateMenuItems()));
  connect(this, SIGNAL(activePartChanged(KParts::Part*)), this, SLOT(slotActivePartChanged(KParts::Part*)));
  connect(dirWatcher, SIGNAL(dirty(const QString&)), this, SLOT(dirty(const QString&)));

  m_history.setAutoDelete( true );
  m_restoring = false;
  setupActions();
}


PartController::~PartController()
{
}


void PartController::createInstance(QWidget *parent)
{
  if (!s_instance)
    s_instance = new PartController(parent);
}


PartController *PartController::getInstance()
{
  return s_instance;
}


void PartController::setupActions()
{
  KActionCollection *ac = TopLevel::getInstance()->main()->actionCollection();

  (void) KStdAction::open(this, SLOT(slotOpenFile()),
    ac, "file_open");

  m_openRecentAction = KStdAction::openRecent( this, SLOT(slotOpenRecent(const KURL&) ),
    ac, "file_open_recent" );
  m_openRecentAction->loadEntries( kapp->config(), "RecentFiles" );

  m_saveAllFilesAction = new KAction(i18n("Save Al&l"), 0,
    this, SLOT(slotSaveAllFiles()),
    ac, "file_save_all");
  m_saveAllFilesAction->setEnabled(false);

  m_revertAllFilesAction = new KAction(i18n("Rever&t All"), 0,
    this, SLOT(slotRevertAllFiles()),
    ac, "file_revert_all");
  m_revertAllFilesAction->setEnabled(false);

  m_closeWindowAction = KStdAction::close(
    this, SLOT(slotCloseWindow()),
    ac, "file_close");
  m_closeWindowAction->setEnabled(false);

  m_closeAllWindowsAction = new KAction(i18n("Close All"), 0,
    this, SLOT(slotCloseAllWindows()),
    ac, "file_close_all");
  m_closeAllWindowsAction->setEnabled(false);

  m_closeOtherWindowsAction = new KAction(i18n("Close All Others"), 0,
    this, SLOT(slotCloseOtherWindows()),
    ac, "file_closeother");
  m_closeOtherWindowsAction->setEnabled(false);

  m_backAction = new KToolBarPopupAction(i18n("Back"), "back", 0,
    this, SLOT(slotBack()),
    ac, "browser_back");
  m_backAction->setEnabled( false );

  m_switchToAction = new KAction(i18n("Switch To..."), KShortcut("CTRL+/"),
    this, SLOT(slotSwitchTo()),
    ac, "file_switchto");

  new KActionSeparator(ac, "dummy_separator");

  connect(m_backAction->popupMenu(), SIGNAL(aboutToShow()),
         this, SLOT(slotBackAboutToShow()));
  connect(m_backAction->popupMenu(), SIGNAL(activated(int)),
         this, SLOT(slotBackPopupActivated(int)));


  m_forwardAction = new KToolBarPopupAction(i18n("Forward"), "forward", 0,
    this, SLOT(slotForward()),
    ac, "browser_forward");
  m_forwardAction->setEnabled( false );

  connect(m_forwardAction->popupMenu(), SIGNAL(aboutToShow()),
         this, SLOT(slotForwardAboutToShow()));
  connect(m_forwardAction->popupMenu(), SIGNAL(activated(int)),
         this, SLOT(slotForwardPopupActivated(int)));
}


void PartController::setEncoding(const QString &encoding)
{
  m_presetEncoding = encoding;
}

KParts::Part* PartController::findOpenDocument(const KURL& url)
{
  KURL partURL = API::getInstance()->project() ? findURLInProject(url) : url;

  partURL.cleanPath();

  return partForURL(partURL);
}

KURL PartController::findURLInProject(const KURL& url)
{
  QStringList fileList = API::getInstance()->project()->allFiles();

  bool filenameOnly = (url.url().find('/') == -1);
  QString filename = filenameOnly ? "/" : "";
  filename += url.url();

  for (QStringList::Iterator it = fileList.begin(); it != fileList.end(); ++it) {
    if ((*it).endsWith(filename)) {
      // Match! The first one is as good as any one, I guess...
      return API::getInstance()->project()->projectDirectory() + "/" + *it;
    }
  }

  return url;
}

void PartController::editDocument(const KURL &inputUrl, int lineNum, int col)
{
  kdDebug(9000) << k_funcinfo << inputUrl.prettyURL() << " linenum " << lineNum << endl;

  KURL url = inputUrl;
  bool localUrl = url.url().startsWith("file:/");

  // Make sure the URL exists
  // KDE 3.0 compatibility hack: use KIO::NetAccess for everything >= KDE 3.1
  if (!url.isValid() || (localUrl ? !QFile(url.path()).exists() : !KIO::NetAccess::exists(url))) {
    // Try to find this file in the current project's list instead
    KDevProject* project = API::getInstance()->project();

    if (project) {
      url = findURLInProject(url);

      localUrl = url.url().startsWith("file:/");
      if (!url.isValid() || (localUrl ? !QFile(url.path()).exists() : !KIO::NetAccess::exists(url))) {
        // See if this url is relative to the current project's directory
        url = project->projectDirectory() + "/" + url.url();
      }
    }

    localUrl = url.url().startsWith("file:/");
    if (!url.isValid() || (localUrl ? !QFile(url.path()).exists() : !KIO::NetAccess::exists(url))) {
      // Here perhaps we should prompt the user to find the file?
      return;
    }
  }

  // We now have a url that exists ;)

  url.cleanPath(true);
  if (url.isLocalFile())
  {
    QString path = url.path();
    path = QDir(path).canonicalPath();
    if ( !path.isEmpty() )
      url.setPath(path);
  }

  KParts::Part *existingPart = partForURL(url);
  if (existingPart)
  {
    activatePart(existingPart);
    EditorProxy::getInstance()->setLineNumber(existingPart, lineNum, col);
    return;
  }

  QString preferred, className;

  QString mimeType, encoding;
  if (m_presetEncoding.isNull())
    mimeType = KMimeType::findByURL(url)->name();
  else {
    mimeType = "text/plain";
    encoding = m_presetEncoding;
    m_presetEncoding = QString::null;
  }

  kdDebug(9000) << "mimeType = " << mimeType << endl;

  if (mimeType.startsWith("text/")
      || mimeType.startsWith("application/x-")
      || mimeType == "image/x-xpm")
  {
      mimeType = "text/plain";
      kapp->config()->setGroup("Editor");
      preferred = kapp->config()->readPathEntry("EmbeddedKTextEditor");
  } else if( mimeType.startsWith("inode/") ){
      return;
  }

  KParts::Factory *factory = 0;

  // load the appropriate part factory like chosen in the editor-chooser part
  // (Note: KTextEditor/Document is the editor in MDI mode. KTextEditor/Editor is the editor in SDI mode.
  //        But KTextEditor/Editor doesn't work for the Kate part on KDE-3.0.x, so better use KTextEditor/Document anyway.)
  QString services[] = {"KTextEditor/Document", "KParts/ReadWritePart", "KParts/ReadOnlyPart"};
  QString classnames[] = {"KTextEditor::Document", "KParts::ReadWritePart", "KParts::ReadOnlyPart"};
  for (uint i=0; i<3; ++i)
  {
    factory = findPartFactory(mimeType, services[i], preferred);
    if (factory)
    {
      className = classnames[i];
      break;
    }
  }

  kdDebug(9000) << "factory = " << factory << endl;

  if (factory)
  {
      // Currently, only a single view per document is supported.
      // So fall back (downgrade) from MDI-mode editor to SDI-mode editor
      // (Note: This always works since KTextEditor::Document inherits KTextEditor::Editor)

      if (className == "KTextEditor::Document")
	  className = "KTextEditor::Editor";

      // create the object of the desired class
      KParts::ReadOnlyPart *part = static_cast<KParts::ReadOnlyPart*>(factory->createPart(TopLevel::getInstance()->main(), 0, 0, 0, className));
      KParts::BrowserExtension *extension = KParts::BrowserExtension::childObject(part);
      kdDebug(9000) << "Encoding: " << encoding << ", extension: " << extension << endl;
      if (extension && !encoding.isNull())
      {
	  KParts::URLArgs args;
	  args.serviceType = mimeType + ";" + encoding;
	  extension->setURLArgs(args);
      }
      part->openURL(url);

      bool isTextEditor = className == "KTextEditor::Editor";
      integratePart(part, url, isTextEditor );

      if( isTextEditor )
	  EditorProxy::getInstance()->setLineNumber(part, lineNum, col);
  }
  else
      KRun::runURL(url, mimeType);
}


void PartController::showDocument(const KURL &url, const QString &context)
{
  QString fixedPath = DocumentationPart::resolveEnvVarsInURL(url.url()); // possibly could env vars
  KURL docUrl(fixedPath);
  kdDebug(9000) << "SHOW: " << docUrl.url() << " context=" << context << endl;

  if ( docUrl.isLocalFile() && KMimeType::findByURL(docUrl)->name() != "text/html" ) {
    // a link in a html-file pointed to a local text file - display
    // it in the editor instead of a html-view to avoid uglyness
    editDocument( docUrl );
    return;
  }

  DocumentationPart *part = 0;

  if (!context.isEmpty())
    part = findDocPart(context);

  if (!part)
  {
    part = new DocumentationPart;
    part->setContext(context);
    integratePart(part,docUrl);
  }
  else
    activatePart(part);

  if( !m_restoring )
     addHistoryEntry( new HistoryEntry(docUrl, context) );

  bool bSuccess = part->openURL(docUrl);
  if (!bSuccess) {
    // part->showError(...);
  }
}


DocumentationPart *PartController::findDocPart(const QString &context)
{
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
  {
    DocumentationPart *part = dynamic_cast<DocumentationPart*>(it.current());
    if (part && (part->context() == context))
      return part;
  }

  return 0;
}


KParts::Factory *PartController::findPartFactory(const QString &mimeType, const QString &partType, const QString &preferredName)
{
  KTrader::OfferList offers = KTrader::self()->query(mimeType, QString("'%1' in ServiceTypes").arg(partType));

  if (offers.count() > 0)
  {
    KService::Ptr ptr = 0;
    // if there is a preferred plugin we'll take it
    if ( !preferredName.isEmpty() ) {
      KTrader::OfferList::Iterator it;
      for (it = offers.begin(); it != offers.end(); ++it) {
        if ((*it)->name() == preferredName) {
          ptr = (*it);
        }
      }
    }
    // else we just take the first in the list
    if ( !ptr ) {
      ptr = offers.first();
    }
    return static_cast<KParts::Factory*>(KLibLoader::self()->factory(QFile::encodeName(ptr->library())));
  }

  return 0;
}


void PartController::integratePart(KParts::Part *part, const KURL &url, bool isTextEditor )
{
  if (!part->widget()) {
    /// @todo error handling
      kdDebug(9000) << "no widget for this part!!" << endl;
      return; // to avoid later crash
  }

  TopLevel::getInstance()->embedPartView(part->widget(), url.filename(), url.url());

  addPart(part);

  if( isTextEditor ){
      EditorProxy::getInstance()->installPopup(part, contextPopupMenu());

#if KDE_VERSION < 310
      // HACK: this is a workaround. The kate-part does not emit "completed" when
      // it save a file yet.
      connect(part, SIGNAL(fileNameChanged()), this, SLOT(slotUploadFinished()));
#endif
  }

  // tell the parts we loaded a document
  KParts::ReadOnlyPart *ro_part = dynamic_cast<KParts::ReadOnlyPart*>(part);
  if (ro_part && ro_part->url().isLocalFile()) {
//    kdDebug(9000) << "KDirWatch: adding " << url.path() << endl;
    dirWatcher->addFile( url.path() );
    accessTimeMap[ ro_part ] = dirWatcher->ctime( url.path() );
    emit loadedFile(ro_part->url().path());
  }

  // let's get notified when a document has been changed
  connect(part, SIGNAL(completed()), this, SLOT(slotUploadFinished()));
  connect(part, SIGNAL(fileNameChanged(const KURL &)), this, SLOT(slotFileNameChanged()));
}


void PartController::slotUploadFinished()
{
  const KParts::ReadOnlyPart *ro_part = dynamic_cast<const KParts::ReadOnlyPart*>(sender());

  if (!ro_part || !ro_part->url().isLocalFile())
    return;

  QString path = ro_part->url().path();
  // can't use KDirWatch's ctime here since it might not be updated yet
  QFileInfo fi( path );
//      kdDebug(9000) << "*** uploadFinished() " << fi.lastModified().toString( "mm:ss:zzz" ) << endl;
  accessTimeMap[ ro_part ] = fi.lastModified();
  emit savedFile( path );
}

void PartController::slotFileNameChanged()
{
  const KParts::ReadOnlyPart *ro_part = dynamic_cast<const KParts::ReadOnlyPart*>(sender());

  if ( !ro_part || !ro_part->url().isLocalFile() )
      return;

  QString path = ro_part->url().path();
  accessTimeMap[ ro_part ] = dirWatcher->ctime( path );
  emit fileDirty( path );
}

QPopupMenu *PartController::contextPopupMenu()
{
  static QPopupMenu *popup = 0;

  if (!popup)
    popup = (QPopupMenu*)(TopLevel::getInstance()->main())->factory()->container("rb_popup", TopLevel::getInstance()->main());

  return popup;
}


/*static bool urlIsEqual(const KURL &a, const KURL &b)
{
  if (a.isLocalFile() && b.isLocalFile())
  {
    struct stat aStat, bStat;

    if ((::stat(QFile::encodeName(a.fileName()), &aStat) == 0)
        && (::stat(QFile::encodeName(b.fileName()), &bStat) == 0))
    {
      return (aStat.st_dev == bStat.st_dev) && (aStat.st_ino == bStat.st_ino);
    }
  }

  return a == b;
}*/

KParts::Part *PartController::partForURL(const KURL &url)
{
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
  {
    KParts::ReadOnlyPart *ro_part = dynamic_cast<KParts::ReadOnlyPart*>(it.current());
    if (ro_part && url == ro_part->url())
      return ro_part;
  }

  return 0;
}


void PartController::activatePart(KParts::Part *part)
{
  setActivePart(part);

  if (part->widget())
  {
    TopLevel::getInstance()->raiseView(part->widget());
    part->widget()->setFocus();
  }
}


void PartController::closeActivePart()
{
  if (!activePart())
    return;

  closePart(activePart());
}


bool PartController::closePart(KParts::Part *part)
{
  if (part->inherits("KParts::ReadOnlyPart"))
  {
    KParts::ReadOnlyPart *ro_part = static_cast<KParts::ReadOnlyPart*>(part);

    if (!ro_part->closeURL())
    {
      return false;
    }
  }

  // If we didn't call removePart(), KParts::PartManager::slotObjectDestroyed would
  // get called from the destroyed signal of the part being deleted below.
  // The call chain from that looks like this:
  // QObject::destroyed()
  //   KParts::PartManager::slotWidgetDestroyed() -> setActivePart() -> activePartChanged()
  //     TopLevelXXX::createGUI()
  //       KXMLGUIFactory::removeClient()
  // But then KXMLGUIFactory tries to remove the already-deleted part.
  // Normally this would work, because the factory uses a QGuardedPtr to the part.
  // But the QGuardedPtr is connected to the _same_ destroyed() slot that got us to
  // that point (slots are called in an undefined order)!

  // The following line can be removed with kdelibs HEAD! (2002-05-26)
  removePart( part );

  if (part->widget())
    TopLevel::getInstance()->removeView(part->widget());

  delete part;

  return true;
}


void PartController::updateMenuItems()
{
  bool hasWriteParts = false;
  bool hasReadOnlyParts = false;

  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
  {
    if (it.current()->inherits("KParts::ReadWritePart"))
      hasWriteParts = true;
    if (it.current()->inherits("KParts::ReadOnlyPart"))
      hasReadOnlyParts = true;
  }

  m_saveAllFilesAction->setEnabled(hasWriteParts);
  m_revertAllFilesAction->setEnabled(hasWriteParts);
  m_closeWindowAction->setEnabled(hasReadOnlyParts);
  m_closeAllWindowsAction->setEnabled(hasReadOnlyParts);
  m_closeOtherWindowsAction->setEnabled(hasReadOnlyParts);

  m_backAction->setEnabled(m_history.current() != m_history.getFirst());
  m_forwardAction->setEnabled(m_history.current() != m_history.getLast());
}


void PartController::slotSaveAllFiles()
{
  saveAllFiles();
}


void PartController::saveFile(KParts::Part *part)
{
  KParts::ReadWritePart *rw_part = dynamic_cast<KParts::ReadWritePart*>(part);
  if ( !rw_part )
    return;
  if ( isDirty( rw_part ) ) {
    kdDebug(9000) << "DIRTY SAVE" << endl;
  }

  if( rw_part->isModified() ) {
    rw_part->save();
    TopLevel::getInstance()->statusBar()->message(i18n("Saved %1").arg(rw_part->url().prettyURL()), 2000);
  }
}

void PartController::saveAllFiles()
{
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
    saveFile( it.current() );
}


void PartController::slotRevertAllFiles()
{
  revertAllFiles();
}

void PartController::revertFile(KParts::Part *part)
{
  if ( !part )
    return;

  if (part->inherits("KParts::ReadWritePart")) {
      KParts::ReadWritePart *rw_part = static_cast<KParts::ReadWritePart*>(part);
      if ( rw_part->url().isLocalFile() )
          accessTimeMap[ static_cast<KParts::ReadOnlyPart*>(part) ] = dirWatcher->ctime( rw_part->url().path() );
      rw_part->openURL(rw_part->url());
    }
}

void PartController::revertAllFiles()
{
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
    revertFile( it.current() );
}


void PartController::slotCloseWindow()
{
  closeActivePart();
}


QStringList PartController::getModifiedDocuments( KParts::Part* excludeMe )
{
  QStringList modFiles;
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it) {
    if (it.current()->inherits("KParts::ReadWritePart")) {
      KParts::ReadWritePart *rw_part = static_cast<KParts::ReadWritePart*>(it.current());
      if ( rw_part->isModified() && rw_part != excludeMe )
        modFiles << rw_part->url().url();
    }
  }
  return modFiles;
}

void PartController::slotCloseAllWindows()
{
  QStringList modFiles = getModifiedDocuments();
  if ( modFiles.count() > 1 ) {
    KSaveAllDialog* sad = new KSaveAllDialog( modFiles, TopLevel::getInstance()->main() );
    sad->exec();
    if ( sad->result() == KSaveAllDialog::Cancel )
      return;
    else if ( sad->result() == KSaveAllDialog::Revert )
      revertAllFiles();
    else if ( sad->result() == KSaveAllDialog::SaveAll )
      saveAllFiles();
  }

  while (parts()->count() > 0)
    closePart(parts()->getFirst());
}


void PartController::slotCloseOtherWindows()
{
  if (!activePart())
    return;

  QStringList modFiles = getModifiedDocuments( activePart() );
  if ( modFiles.count() > 1 ) {
    KSaveAllDialog* sad = new KSaveAllDialog( modFiles, TopLevel::getInstance()->main() );
    sad->exec();
    if ( sad->result() == KSaveAllDialog::Cancel )
      return;
    else if ( sad->result() == KSaveAllDialog::Revert )
      revertAllFiles();
    else if ( sad->result() == KSaveAllDialog::SaveAll )
      saveAllFiles();
  }

  while (parts()->count() > 1)
  {
    QPtrListIterator<KParts::Part> it(*parts());
    for ( ; it.current(); ++it)
      if (it.current() != activePart())
        closePart(it.current());
  }
}


void PartController::slotCurrentChanged(QWidget *w)
{
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
    if (it.current()->widget() == w)
    {
      setActivePart(it.current(), w);
      break;
    }
}


void PartController::slotOpenFile()
{
  QStringList fileNames = KFileDialog::getOpenFileNames(QString::null, QString::null, TopLevel::getInstance()->main(), QString::null);

  for ( QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it ) {
    editDocument(KURL(*it));
    m_openRecentAction->addURL( KURL(*it) );
  }
  m_openRecentAction->saveEntries( kapp->config(), "RecentFiles" );
}

void PartController::slotOpenRecent( const KURL& url )
{
  editDocument( url );
  // stupid bugfix - don't allow an active item in the list
  m_openRecentAction->setCurrentItem( -1 );
}

bool PartController::closeDocuments(const QStringList &documents)
{
  QStringList::ConstIterator it;
  KSaveAllDialog::SaveAllResult result = KSaveAllDialog::Cancel;

  QStringList modFiles;
  for (it=documents.begin(); it != documents.end(); ++it) {
    KParts::Part *part = partForURL(KURL(*it));
    if ( part && part->inherits("KParts::ReadWritePart") && ((KParts::ReadWritePart*)part)->isModified() )
      modFiles << (*it);
  }

  if ( modFiles.count() > 1 ) {
    KSaveAllDialog* sad = new KSaveAllDialog( modFiles, TopLevel::getInstance()->main() );
    sad->exec();
    if ( sad->result() == KSaveAllDialog::Cancel )
      return false;
    else if ( sad->result() == KSaveAllDialog::Revert )
      result = KSaveAllDialog::Revert;
    else if ( sad->result() == KSaveAllDialog::SaveAll )
      result = KSaveAllDialog::SaveAll;
  }

  for (it=documents.begin(); it != documents.end(); ++it)
  {
    KParts::Part *part = partForURL(KURL(*it));

    if (!part)
      continue;

    if ( result == KSaveAllDialog::SaveAll ) {
      saveFile( part );
    } else if ( result == KSaveAllDialog::Revert ) {
      revertFile( part );
    }

    if(!closePart(part))
      return false;
  }

  return true;
}

void PartController::slotClosePartForWidget( const QWidget* w)
{
  closePartForWidget(w);
}

void PartController::slotCloseAllButPartForWidget(QWidget* w)
{
  slotCurrentChanged(w);
  slotCloseOtherWindows();
}

bool PartController::closePartForWidget( const QWidget* w )
{
  qDebug("in close part for widget");
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
    if (it.current()->widget() == w)
    {
      return closePart( *it );
    }
  return true;
}

bool PartController::readyToClose()
{
  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
  {
    if(!closePart(*it))
      return false;
  }

  return true;
}

void PartController::slotActivePartChanged( KParts::Part* part )
{
    updateMenuItems();
    if( !part || QString(part->name()) != "DocumentationPart" ){
        m_backAction->setEnabled( false );
	m_forwardAction->setEnabled( false );
    }
}

void PartController::slotSwitchTo()
{
    QMap<QString,KParts::ReadOnlyPart*> parts_map;
    QStringList part_list;
    QPtrList<KParts::Part> pl = *parts();
    KParts::Part *part;
    for(part=pl.first();part;part=pl.next()) {
        kdDebug(9000) << "Part..." << endl;
	if (part->inherits("KParts::ReadOnlyPart")) {
	    KParts::ReadOnlyPart *ro_part = static_cast<KParts::ReadOnlyPart*>(part);
            QString name = ro_part->url().fileName();
	    part_list.append(name);
            parts_map[name] = ro_part;
            kdDebug(9000) << "Found part for URL " << ro_part->url().prettyURL() << endl;
	}
    }

    KDialogBase dialog(KDialogBase::Plain, i18n("Switch To..."), KDialogBase::Ok|KDialogBase::Cancel,
		       KDialogBase::Ok, 0, "Switch to", true);
    QGridLayout *grid = new QGridLayout( dialog.plainPage(), 2, 1, 10, 10);
    KLineEdit *editbox = new KLineEdit(dialog.plainPage());
    grid->addWidget(new QLabel( i18n("Switch to buffer:"), dialog.plainPage() ), 0, 0);
    grid->addWidget(editbox, 1, 0);
    editbox->completionObject()->setItems( part_list );
    editbox->setFocus();
    int result = dialog.exec();
    if (result==KDialogBase::KDialogBase::Accepted) {
        if (parts_map.contains(editbox->text())) {
            activatePart(parts_map[editbox->text()]);
        }
    }
}

void PartController::slotBack()
{
  saveState(activePart());

  if(m_history.prev()==0L) m_history.first();

  restoreState();
}

void PartController::slotForward()
{
  saveState(activePart());

  if(m_history.next()==0L)  m_history.last();

  restoreState();
}

void PartController::slotBackAboutToShow()
{
  KPopupMenu *popup = m_backAction->popupMenu();
  popup->clear();

  int savePos = m_history.at();
  for (int i=0; i<10 && m_history.prev(); ++i)
    popup->insertItem( m_history.current()->url.url() );

  m_history.at(savePos);
}

void PartController::slotBackPopupActivated( int id )
{
  int by = m_backAction->popupMenu()->indexOf(id)+1;

  saveState(activePart());
  for (int i=0; i < by; ++i)
    m_history.prev();
  if(m_history.prev()==0L) m_history.first();

  restoreState();

  updateMenuItems();
}

void PartController::slotForwardAboutToShow()
{
  KPopupMenu *popup = m_forwardAction->popupMenu();
  popup->clear();

  int savePos = m_history.at();
  for (int i=0; i<10 && m_history.next(); ++i)
    popup->insertItem(m_history.current()->url.url());

  m_history.at(savePos);
}

void PartController::slotForwardPopupActivated( int id )
{
  int by = m_forwardAction->popupMenu()->indexOf(id)+1;

  saveState(activePart());
  for (int i=0; i < by; ++i)
    m_history.next();
  if(m_history.current()==0L) m_history.last();

  restoreState();

  updateMenuItems();
}

void PartController::addHistoryEntry( HistoryEntry* entry )
{
  HistoryEntry *current = m_history.current();
  while (m_history.getLast() != current)
    m_history.removeLast();
  m_history.append( entry );
  m_history.last();

  updateMenuItems();
}

void PartController::saveState( KParts::Part* part )
{
  DocumentationPart* d = dynamic_cast<DocumentationPart*>( part );
  if( !d )
     return;

  HistoryEntry *entry = m_history.current();
  if (!entry)
    return;

  entry->url = d->url();
}


void PartController::restoreState()
{
  HistoryEntry *entry = m_history.current();
  if (!entry)
    return;

  m_restoring = true;
  showDocument( entry->url, entry->context );
  m_restoring = false;
  updateMenuItems();
}

void PartController::showPart( KParts::Part* part, const QString& name, const QString& shortDescription )
{
  if (!part->widget()) {
    /// @todo error handling
    return; // to avoid later crash
  }

  QPtrListIterator<KParts::Part> it(*parts());
  for ( ; it.current(); ++it)
  {
    if( it.current() == part ){
        // part already embedded
	activatePart( it.current() );
	return;
    }
  }

  // embed the part
  TopLevel::getInstance()->embedPartView( part->widget(), name, shortDescription );
  addPart( part );
}

void PartController::dirty( const QString& fileName )
{
//  kdDebug(9000) << "DIRRRRRTY: " << fileName << " " << dirWatcher->ctime( fileName ).toString( "mm:ss:zzz" ) << endl;
  emit fileDirty( fileName );
}

bool PartController::isDirty( KParts::ReadOnlyPart* part )
{
  if ( !part || !part->url().isLocalFile() )
    return false;

//  kdDebug( 9000 ) << "isDirty?" << accessTimeMap[ part ].toString( "mm:zzz" ) << " : " << dirWatcher->ctime( part->url().path() ).toString( "mm:zzz" ) << endl;

  if ( accessTimeMap.contains( part ) )
    return ( accessTimeMap[ part ] < dirWatcher->ctime( part->url().path() ) );

  accessTimeMap[ part ] = dirWatcher->ctime( part->url().path() );
  return false;
}

#include "partcontroller.moc"
