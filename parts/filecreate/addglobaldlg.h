/****************************************************************************
** Form interface generated from reading ui file './addglobaldlg.ui'
**
** Created: Сбт Лют 8 20:48:22 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ADDGLOBALDLG_H
#define ADDGLOBALDLG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class Q3ListView;
class Q3ListViewItem;
class QPushButton;

class AddGlobalDlg : public QDialog
{
    Q_OBJECT

public:
    AddGlobalDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~AddGlobalDlg();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    Q3ListView* fcglobal_view;

protected:
    QGridLayout* AddGlobalDlgLayout;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();
};

#endif // ADDGLOBALDLG_H
