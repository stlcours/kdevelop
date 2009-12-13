//This is file has been generated by xmltokross, you should not edit this file but the files used to generate it.

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <kross/core/manager.h>
#include <kross/core/wrapperinterface.h>
#include <project/projectmodel.h>

class KrossKDevelopProjectVisitor : public QObject, public Kross::WrapperInterface
{
	Q_OBJECT
	public:
		KrossKDevelopProjectVisitor(KDevelop::ProjectVisitor* obj, QObject* parent=0) : QObject(parent), wrapped(obj)		{ setObjectName("KDevelop::ProjectVisitor"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE void visit(KDevelop::IProject* x0) { wrapped->visit(x0); }
		Q_SCRIPTABLE void visit(KDevelop::ProjectBuildFolderItem* x0) { wrapped->visit(x0); }
		Q_SCRIPTABLE void visit(KDevelop::ProjectExecutableTargetItem* x0) { wrapped->visit(x0); }
		Q_SCRIPTABLE void visit(KDevelop::ProjectFolderItem* x0) { wrapped->visit(x0); }
		Q_SCRIPTABLE void visit(KDevelop::ProjectFileItem* x0) { wrapped->visit(x0); }
		Q_SCRIPTABLE void visit(KDevelop::ProjectLibraryTargetItem* x0) { wrapped->visit(x0); }
	private:
		KDevelop::ProjectVisitor* wrapped;
};

class KrossKDevelopProjectBaseItem : public QObject, public Kross::WrapperInterface
{
	Q_OBJECT
	Q_ENUMS(ProjectItemType)
	Q_FLAGS(ProjectItemType BuildFolder Folder ExecutableTarget LibraryTarget Target File)

	public:
		enum KrossProjectItemType { BuildFolder=KDevelop::ProjectBaseItem::BuildFolder, Folder=KDevelop::ProjectBaseItem::Folder, ExecutableTarget=KDevelop::ProjectBaseItem::ExecutableTarget, LibraryTarget=KDevelop::ProjectBaseItem::LibraryTarget, Target=KDevelop::ProjectBaseItem::Target, File=KDevelop::ProjectBaseItem::File };
		KrossKDevelopProjectBaseItem(KDevelop::ProjectBaseItem* obj, QObject* parent=0) : QObject(parent), wrapped(obj)		{ setObjectName("KDevelop::ProjectBaseItem"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE void add(KDevelop::ProjectBaseItem* x0) { wrapped->add(x0); }
		Q_SCRIPTABLE KDevelop::IProject* project() const { return wrapped->project(); }
		Q_SCRIPTABLE KDevelop::ProjectFolderItem* folder() const { return wrapped->folder(); }
		Q_SCRIPTABLE KDevelop::ProjectTargetItem* target() const { return wrapped->target(); }
		Q_SCRIPTABLE KDevelop::ProjectFileItem* file() const { return wrapped->file(); }
		Q_SCRIPTABLE KDevelop::ProjectExecutableTargetItem* executable() const { return wrapped->executable(); }
		Q_SCRIPTABLE void setParent(QStandardItem* x0) { wrapped->setParent(x0); }
		Q_SCRIPTABLE void setIcon() { wrapped->setIcon(); }
		Q_SCRIPTABLE QList< KDevelop::ProjectFolderItem* > folderList() const { return wrapped->folderList(); }
		Q_SCRIPTABLE QList< KDevelop::ProjectTargetItem* > targetList() const { return wrapped->targetList(); }
		Q_SCRIPTABLE QList< KDevelop::ProjectFileItem* > fileList() const { return wrapped->fileList(); }
	private:
		KDevelop::ProjectBaseItem* wrapped;
};

class KrossKDevelopProjectFolderItem : public KrossKDevelopProjectBaseItem
{
	Q_OBJECT
	public:
		KrossKDevelopProjectFolderItem(KDevelop::ProjectFolderItem* obj, QObject* parent=0) : KrossKDevelopProjectBaseItem(obj, parent), wrapped(obj)
	{ setObjectName("KDevelop::ProjectFolderItem"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE KDevelop::ProjectFolderItem* folder() const { return wrapped->folder(); }
		Q_SCRIPTABLE int type() const { return wrapped->type(); }
		Q_SCRIPTABLE KUrl url() const { return wrapped->url(); }
		Q_SCRIPTABLE QString folderName() const { return wrapped->folderName(); }
		Q_SCRIPTABLE void setUrl(const KUrl& x0) { wrapped->setUrl(x0); }
		Q_SCRIPTABLE void setIcon() { wrapped->setIcon(); }
		Q_SCRIPTABLE bool isProjectRoot() const { return wrapped->isProjectRoot(); }
		Q_SCRIPTABLE void setProjectRoot(bool x0) { wrapped->setProjectRoot(x0); }
		Q_SCRIPTABLE bool hasFileOrFolder(const QString& x0) const { return wrapped->hasFileOrFolder(x0); }
	private:
		KDevelop::ProjectFolderItem* wrapped;
};

class KrossKDevelopProjectBuildFolderItem : public KrossKDevelopProjectFolderItem
{
	Q_OBJECT
	public:
		KrossKDevelopProjectBuildFolderItem(KDevelop::ProjectBuildFolderItem* obj, QObject* parent=0) : KrossKDevelopProjectFolderItem(obj, parent), wrapped(obj)
	{ setObjectName("KDevelop::ProjectBuildFolderItem"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE int type() const { return wrapped->type(); }
		Q_SCRIPTABLE void setIcon() { wrapped->setIcon(); }
	private:
		KDevelop::ProjectBuildFolderItem* wrapped;
};

class KrossKDevelopProjectTargetItem : public KrossKDevelopProjectBaseItem
{
	Q_OBJECT
	public:
		KrossKDevelopProjectTargetItem(KDevelop::ProjectTargetItem* obj, QObject* parent=0) : KrossKDevelopProjectBaseItem(obj, parent), wrapped(obj)
	{ setObjectName("KDevelop::ProjectTargetItem"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE int type() const { return wrapped->type(); }
		Q_SCRIPTABLE KDevelop::ProjectTargetItem* target() const { return wrapped->target(); }
		Q_SCRIPTABLE void setIcon() { wrapped->setIcon(); }
	private:
		KDevelop::ProjectTargetItem* wrapped;
};

class KrossKDevelopProjectExecutableTargetItem : public KrossKDevelopProjectTargetItem
{
	Q_OBJECT
	public:
		KrossKDevelopProjectExecutableTargetItem(KDevelop::ProjectExecutableTargetItem* obj, QObject* parent=0) : KrossKDevelopProjectTargetItem(obj, parent), wrapped(obj)
	{ setObjectName("KDevelop::ProjectExecutableTargetItem"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE KDevelop::ProjectExecutableTargetItem* executable() const { return wrapped->executable(); }
		Q_SCRIPTABLE int type() const { return wrapped->type(); }
		Q_SCRIPTABLE KUrl builtUrl() const { return wrapped->builtUrl(); }
		Q_SCRIPTABLE KUrl installedUrl() const { return wrapped->installedUrl(); }
	private:
		KDevelop::ProjectExecutableTargetItem* wrapped;
};

class KrossKDevelopProjectLibraryTargetItem : public KrossKDevelopProjectTargetItem
{
	Q_OBJECT
	public:
		KrossKDevelopProjectLibraryTargetItem(KDevelop::ProjectLibraryTargetItem* obj, QObject* parent=0) : KrossKDevelopProjectTargetItem(obj, parent), wrapped(obj)
	{ setObjectName("KDevelop::ProjectLibraryTargetItem"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE int type() const { return wrapped->type(); }
	private:
		KDevelop::ProjectLibraryTargetItem* wrapped;
};

class KrossKDevelopProjectFileItem : public KrossKDevelopProjectBaseItem
{
	Q_OBJECT
	public:
		KrossKDevelopProjectFileItem(KDevelop::ProjectFileItem* obj, QObject* parent=0) : KrossKDevelopProjectBaseItem(obj, parent), wrapped(obj)
	{ setObjectName("KDevelop::ProjectFileItem"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE int type() const { return wrapped->type(); }
		Q_SCRIPTABLE KDevelop::ProjectFileItem* file() const { return wrapped->file(); }
		Q_SCRIPTABLE KUrl url() const { return wrapped->url(); }
		Q_SCRIPTABLE QString fileName() const { return wrapped->fileName(); }
		Q_SCRIPTABLE void setUrl(const KUrl& x0) { wrapped->setUrl(x0); }
		Q_SCRIPTABLE void setIcon() { wrapped->setIcon(); }
	private:
		KDevelop::ProjectFileItem* wrapped;
};

class KrossKDevelopProjectModel : public QObject, public Kross::WrapperInterface
{
	Q_OBJECT
	public:
		KrossKDevelopProjectModel(KDevelop::ProjectModel* obj, QObject* parent=0) : QObject(parent), wrapped(obj)		{ setObjectName("KDevelop::ProjectModel"); }
		void* wrappedObject() const { return wrapped; }

		Q_PROPERTY(const QMetaObject  staticMetaObject READ getstaticMetaObject SCRIPTABLE true)
		Q_SCRIPTABLE const QMetaObject  getstaticMetaObject() const { return wrapped->staticMetaObject; }
		Q_SCRIPTABLE KDevelop::ProjectBaseItem* item(const QModelIndex& x0) const { return wrapped->item(x0); }
		Q_SCRIPTABLE Qt::ItemFlags flags(const QModelIndex& x0) const { return wrapped->flags(x0); }
		Q_SCRIPTABLE void resetModel() { wrapped->resetModel(); }
		Q_SCRIPTABLE void fetchMore(const QModelIndex& x0) { wrapped->fetchMore(x0); }
		Q_SCRIPTABLE bool canFetchMore(const QModelIndex& x0) const { return wrapped->canFetchMore(x0); }
		Q_SCRIPTABLE QModelIndex pathToIndex(const QStringList& x0) const { return wrapped->pathToIndex(x0); }
		Q_SCRIPTABLE QStringList pathFromIndex(const QModelIndex& x0) const { return wrapped->pathFromIndex(x0); }
	private:
		KDevelop::ProjectModel* wrapped;
};

bool krossprojectmodel_registerHandler(const QByteArray& name, Kross::MetaTypeHandler::FunctionPtr* handler)
{ Kross::Manager::self().registerMetaTypeHandler(name, handler); return false; }

namespace Handlers
{
QVariant _kDevelopProjectModelHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectModel* t=static_cast<KDevelop::ProjectModel*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectModel*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopProjectModel(t, 0));
}
bool b_KDevelopProjectModel1=krossprojectmodel_registerHandler("ProjectModel*", _kDevelopProjectModelHandler);
bool b_KDevelopProjectModel=krossprojectmodel_registerHandler("KDevelop::ProjectModel*", _kDevelopProjectModelHandler);
QVariant kDevelopProjectModelHandler(KDevelop::ProjectModel* type){ return _kDevelopProjectModelHandler(type); }
QVariant kDevelopProjectModelHandler(const KDevelop::ProjectModel* type) { return _kDevelopProjectModelHandler((void*) type); }

QVariant _kDevelopProjectFileItemHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectFileItem* t=static_cast<KDevelop::ProjectFileItem*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectFileItem*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopProjectFileItem(t, 0));
}
bool b_KDevelopProjectFileItem1=krossprojectmodel_registerHandler("ProjectFileItem*", _kDevelopProjectFileItemHandler);
bool b_KDevelopProjectFileItem=krossprojectmodel_registerHandler("KDevelop::ProjectFileItem*", _kDevelopProjectFileItemHandler);
QVariant kDevelopProjectFileItemHandler(KDevelop::ProjectFileItem* type){ return _kDevelopProjectFileItemHandler(type); }
QVariant kDevelopProjectFileItemHandler(const KDevelop::ProjectFileItem* type) { return _kDevelopProjectFileItemHandler((void*) type); }

QVariant _kDevelopProjectLibraryTargetItemHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectLibraryTargetItem* t=static_cast<KDevelop::ProjectLibraryTargetItem*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectLibraryTargetItem*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopProjectLibraryTargetItem(t, 0));
}
bool b_KDevelopProjectLibraryTargetItem1=krossprojectmodel_registerHandler("ProjectLibraryTargetItem*", _kDevelopProjectLibraryTargetItemHandler);
bool b_KDevelopProjectLibraryTargetItem=krossprojectmodel_registerHandler("KDevelop::ProjectLibraryTargetItem*", _kDevelopProjectLibraryTargetItemHandler);
QVariant kDevelopProjectLibraryTargetItemHandler(KDevelop::ProjectLibraryTargetItem* type){ return _kDevelopProjectLibraryTargetItemHandler(type); }
QVariant kDevelopProjectLibraryTargetItemHandler(const KDevelop::ProjectLibraryTargetItem* type) { return _kDevelopProjectLibraryTargetItemHandler((void*) type); }

QVariant _kDevelopProjectExecutableTargetItemHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectExecutableTargetItem* t=static_cast<KDevelop::ProjectExecutableTargetItem*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectExecutableTargetItem*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopProjectExecutableTargetItem(t, 0));
}
bool b_KDevelopProjectExecutableTargetItem1=krossprojectmodel_registerHandler("ProjectExecutableTargetItem*", _kDevelopProjectExecutableTargetItemHandler);
bool b_KDevelopProjectExecutableTargetItem=krossprojectmodel_registerHandler("KDevelop::ProjectExecutableTargetItem*", _kDevelopProjectExecutableTargetItemHandler);
QVariant kDevelopProjectExecutableTargetItemHandler(KDevelop::ProjectExecutableTargetItem* type){ return _kDevelopProjectExecutableTargetItemHandler(type); }
QVariant kDevelopProjectExecutableTargetItemHandler(const KDevelop::ProjectExecutableTargetItem* type) { return _kDevelopProjectExecutableTargetItemHandler((void*) type); }

QVariant _kDevelopProjectTargetItemHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectTargetItem* t=static_cast<KDevelop::ProjectTargetItem*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectTargetItem*>(t));
	if(dynamic_cast<KDevelop::ProjectExecutableTargetItem*>(t)) return _kDevelopProjectExecutableTargetItemHandler(type);
	else if(dynamic_cast<KDevelop::ProjectLibraryTargetItem*>(t)) return _kDevelopProjectLibraryTargetItemHandler(type);
	else return qVariantFromValue((QObject*) new KrossKDevelopProjectTargetItem(t, 0));
}
bool b_KDevelopProjectTargetItem1=krossprojectmodel_registerHandler("ProjectTargetItem*", _kDevelopProjectTargetItemHandler);
bool b_KDevelopProjectTargetItem=krossprojectmodel_registerHandler("KDevelop::ProjectTargetItem*", _kDevelopProjectTargetItemHandler);
QVariant kDevelopProjectTargetItemHandler(KDevelop::ProjectTargetItem* type){ return _kDevelopProjectTargetItemHandler(type); }
QVariant kDevelopProjectTargetItemHandler(const KDevelop::ProjectTargetItem* type) { return _kDevelopProjectTargetItemHandler((void*) type); }

QVariant _kDevelopProjectBuildFolderItemHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectBuildFolderItem* t=static_cast<KDevelop::ProjectBuildFolderItem*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectBuildFolderItem*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopProjectBuildFolderItem(t, 0));
}
bool b_KDevelopProjectBuildFolderItem1=krossprojectmodel_registerHandler("ProjectBuildFolderItem*", _kDevelopProjectBuildFolderItemHandler);
bool b_KDevelopProjectBuildFolderItem=krossprojectmodel_registerHandler("KDevelop::ProjectBuildFolderItem*", _kDevelopProjectBuildFolderItemHandler);
QVariant kDevelopProjectBuildFolderItemHandler(KDevelop::ProjectBuildFolderItem* type){ return _kDevelopProjectBuildFolderItemHandler(type); }
QVariant kDevelopProjectBuildFolderItemHandler(const KDevelop::ProjectBuildFolderItem* type) { return _kDevelopProjectBuildFolderItemHandler((void*) type); }

QVariant _kDevelopProjectFolderItemHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectFolderItem* t=static_cast<KDevelop::ProjectFolderItem*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectFolderItem*>(t));
	if(dynamic_cast<KDevelop::ProjectBuildFolderItem*>(t)) return _kDevelopProjectBuildFolderItemHandler(type);
	else return qVariantFromValue((QObject*) new KrossKDevelopProjectFolderItem(t, 0));
}
bool b_KDevelopProjectFolderItem1=krossprojectmodel_registerHandler("ProjectFolderItem*", _kDevelopProjectFolderItemHandler);
bool b_KDevelopProjectFolderItem=krossprojectmodel_registerHandler("KDevelop::ProjectFolderItem*", _kDevelopProjectFolderItemHandler);
QVariant kDevelopProjectFolderItemHandler(KDevelop::ProjectFolderItem* type){ return _kDevelopProjectFolderItemHandler(type); }
QVariant kDevelopProjectFolderItemHandler(const KDevelop::ProjectFolderItem* type) { return _kDevelopProjectFolderItemHandler((void*) type); }

QVariant _kDevelopProjectBaseItemHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectBaseItem* t=static_cast<KDevelop::ProjectBaseItem*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectBaseItem*>(t));
	if(dynamic_cast<KDevelop::ProjectFolderItem*>(t)) return _kDevelopProjectFolderItemHandler(type);
	else if(dynamic_cast<KDevelop::ProjectTargetItem*>(t)) return _kDevelopProjectTargetItemHandler(type);
	else if(dynamic_cast<KDevelop::ProjectFileItem*>(t)) return _kDevelopProjectFileItemHandler(type);
	else return qVariantFromValue((QObject*) new KrossKDevelopProjectBaseItem(t, 0));
}
bool b_KDevelopProjectBaseItem1=krossprojectmodel_registerHandler("ProjectBaseItem*", _kDevelopProjectBaseItemHandler);
bool b_KDevelopProjectBaseItem=krossprojectmodel_registerHandler("KDevelop::ProjectBaseItem*", _kDevelopProjectBaseItemHandler);
QVariant kDevelopProjectBaseItemHandler(KDevelop::ProjectBaseItem* type){ return _kDevelopProjectBaseItemHandler(type); }
QVariant kDevelopProjectBaseItemHandler(const KDevelop::ProjectBaseItem* type) { return _kDevelopProjectBaseItemHandler((void*) type); }

QVariant _kDevelopProjectVisitorHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ProjectVisitor* t=static_cast<KDevelop::ProjectVisitor*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ProjectVisitor*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopProjectVisitor(t, 0));
}
bool b_KDevelopProjectVisitor1=krossprojectmodel_registerHandler("ProjectVisitor*", _kDevelopProjectVisitorHandler);
bool b_KDevelopProjectVisitor=krossprojectmodel_registerHandler("KDevelop::ProjectVisitor*", _kDevelopProjectVisitorHandler);
QVariant kDevelopProjectVisitorHandler(KDevelop::ProjectVisitor* type){ return _kDevelopProjectVisitorHandler(type); }
QVariant kDevelopProjectVisitorHandler(const KDevelop::ProjectVisitor* type) { return _kDevelopProjectVisitorHandler((void*) type); }

}
#include "krossprojectmodel.moc"
