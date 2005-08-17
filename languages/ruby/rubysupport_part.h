/*
 *  Copyright (C) 2003 Marek Janukowicz <marek@janukowicz.net>
 */


#ifndef __KDEVPART_RUBYSUPPORT_H__
#define __KDEVPART_RUBYSUPPORT_H__

#include "kdevlanguagesupport.h"
//Added by qt3to4:
#include <Q3PopupMenu>

class KDialogBase;
class Q3PopupMenu;
class Context;

class RubySupportPart : public KDevLanguageSupport
{
  Q_OBJECT

public:

  RubySupportPart(QObject *parent, const char *name, const QStringList &);
  virtual ~RubySupportPart();

  virtual KDevDesignerIntegration *designer(KInterfaceDesigner::DesignerType type);
  
protected:
    virtual Features features();
    virtual KMimeType::List mimeTypes();

private slots:
    void projectConfigWidget(KDialogBase *dlg);
    void projectOpened();
    void projectClosed();
    void savedFile(const KURL &fileName);
    void addedFilesToProject(const QStringList &fileList);
    void removedFilesFromProject(const QStringList &fileList);
    void slotRun();
    void slotCreateSubclass();
    void contextMenu(Q3PopupMenu *popup, const Context *context);

    void initialParse();

private:
    void startApplication(const QString &program);
    QString interpreter();
    QString mainProgram();
    QString programArgs();
    QString characterCoding();

    void maybeParse(const QString fileName);
    void parse(const QString &fileName);

    QMap<KInterfaceDesigner::DesignerType, KDevDesignerIntegration*> m_designers;
    
    QString m_contextFileName;

};


#endif
