/* This file is part of KDevelop
Copyright 2007-2008 Hamish Rodda <rodda@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#ifndef KDEVPLATFORM_RUNCONTROLLER_H
#define KDEVPLATFORM_RUNCONTROLLER_H

#include <QItemDelegate>

#include <KColorScheme>

#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iruncontroller.h>

#include "shellexport.h"

class QStyleOptionViewItem;
class QPainter;
class QModelIndex;
class KStatefulBrush;

namespace KDevelop
{
class Context;
class ContextMenuExtension;
class IPlugin;
class IProject;
class LaunchConfiguration;
class LaunchConfigurationType;

class KDEVPLATFORMSHELL_EXPORT RunController : public IRunController
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kdevelop.RunController")

public:
    explicit RunController(QObject *parent);
    ~RunController() override;

    void registerJob(KJob *job) override;
    void unregisterJob(KJob *job) override;
    QList<KJob*> currentJobs() const override;

    KJob* execute(const QString& launchMode, ILaunchConfiguration* launch) override;
    QList<ILaunchMode*> launchModes() const override;

    /**
     * @copydoc IRunController::addLaunchMode
     */
    void addLaunchMode( ILaunchMode* mode ) override;

    /**
     * @copydoc IRunController::removeLaunchMode
     */
    void removeLaunchMode( ILaunchMode* mode ) override;

    /**
     * @copydoc IRunController::launchModeForId()
     */
    KDevelop::ILaunchMode* launchModeForId(const QString& id) const override;

    void initialize();
    void cleanup();

    QItemDelegate* delegate() const;

    void addLaunchConfiguration( LaunchConfiguration* l );
    void removeLaunchConfiguration( LaunchConfiguration* l );

    QList<LaunchConfiguration*> launchConfigurationsInternal() const;
    QList<ILaunchConfiguration*> launchConfigurations() const override;
    /**
     * @copydoc IRunController::launchConfigurationTypes()
     */
    QList<LaunchConfigurationType*> launchConfigurationTypes() const override;

    /**
     * @copydoc IRunController::addConfigurationType()
     */
    void addConfigurationType( LaunchConfigurationType* type ) override;

    /**
     * @copydoc IRunController::removeConfigurationType()
     */
    void removeConfigurationType( LaunchConfigurationType* type ) override;

    /**
     * Find the launch configuration type for the given @p id.
     * @returns the launch configuration type having the id, or 0 if no such type is known
     */
    LaunchConfigurationType* launchConfigurationTypeForId( const QString& ) override;

    ILaunchConfiguration* createLaunchConfiguration ( LaunchConfigurationType* type,
                                                              const QPair<QString,QString>& launcher,
                                                              IProject* project = 0,
                                                              const QString& name = QString() ) override;


    void setDefaultLaunch(ILaunchConfiguration* l);
    LaunchConfiguration* defaultLaunch() const;

    ContextMenuExtension contextMenuExtension( KDevelop::Context* ctx );

public Q_SLOTS:
    Q_SCRIPTABLE void executeDefaultLaunch(const QString& runMode) override;

    Q_SCRIPTABLE void stopAllProcesses() override;

protected Q_SLOTS:
    void finished(KJob *job) override;
    void suspended(KJob *job) override;
    void resumed(KJob *job) override;

private Q_SLOTS:
    void slotRefreshProject(KDevelop::IProject* project);
    void slotExecute();
    void slotDebug();
    void slotProfile();
    void slotProjectOpened(KDevelop::IProject* project);
    void slotProjectClosing(KDevelop::IProject* project);
    void slotKillJob();
    void launchChanged(LaunchConfiguration*);
    void jobDestroyed(QObject* job);

private:
    void setupActions();
    void checkState();

    Q_PRIVATE_SLOT(d, void configureLaunches())
    Q_PRIVATE_SLOT(d, void launchAs(int))

    class RunControllerPrivate;
    RunControllerPrivate* const d;
};

class RunDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit RunDelegate( QObject* = 0 );
    void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const override;
private:
    KStatefulBrush runProviderBrush;
    KStatefulBrush errorBrush;
};

}

#endif
