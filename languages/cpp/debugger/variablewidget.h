/***************************************************************************
    begin                : Sun Aug 8 1999
    copyright            : (C) 1999 by John Birch
    email                : jbb@kdevelop.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _VARIABLEWIDGET_H_
#define _VARIABLEWIDGET_H_

#include "gdbcontroller.h"

#include <klistview.h>
#include <kcombobox.h>
#include <qwidget.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QFocusEvent>
#include <Q3CString>

class KLineEdit;

namespace GDBDebugger
{

class TrimmableItem;
class VarFrameRoot;
class WatchRoot;
class VarItem;
class VariableTree;
class DbgController;

enum { VarNameCol = 0, ValueCol = 1, VarTypeCol = 2};
enum DataType { typeUnknown, typeValue, typePointer, typeReference,
                typeStruct, typeArray, typeQString, typeWhitespace,
                typeName };

class VariableWidget : public QWidget
{
    Q_OBJECT

public:
    VariableWidget( QWidget *parent=0, const char *name=0 );
    void clear();

    VariableTree *varTree() const
    { return varTree_; }

    virtual void setEnabled(bool b);

protected:
    virtual void focusInEvent(QFocusEvent *e);

public slots:
    void slotAddWatchVariable();
    void slotAddWatchVariable(const QString &ident);
    void slotEvaluateExpression();
    void slotEvaluateExpression(const QString &ident);

private:
    VariableTree *varTree_;
//    KLineEdit *watchVarEntry_;
    friend class VariableTree;

    KHistoryCombo *watchVarEditor_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class VariableTree : public KListView, public QToolTip
{
    Q_OBJECT
//rgruber: we need this to be able to emit expandItem() from within TrimmableItem
friend class TrimmableItem;

public:
    VariableTree( VariableWidget *parent, const char *name=0 );
    virtual ~VariableTree();

    Q3ListViewItem *lastChild() const;

    int activeFlag() const                { return activeFlag_; }
    void setActiveFlag()                  { activeFlag_++; }
    void setRadix(int r)                  { iOutRadix=r; }

    Q3ListViewItem *findRoot(Q3ListViewItem *item) const;
    VarFrameRoot *findFrame(int frameNo, int threadNo) const;
    WatchRoot *findWatch();

    // Remove items that are not active
    void trim();
    void trimExcessFrames();

	// (from QToolTip) Display a tooltip when the cursor is over an item
	virtual void maybeTip(const QPoint &);

signals:
    void toggleWatchpoint(const QString &varName);
    void selectFrame(int frameNo, int threadNo);
    void expandItem(TrimmableItem *item);
    void expandUserItem(VarItem *item, const Q3CString &request);
    void setLocalViewState(bool localsOn);
    // Emitted when *this is interested in args and locals for the
    // current frame.
    void produceVariablesInfo();

    // jw
    void varItemConstructed(VarItem *item);

    //rgr
    void toggleRadix(Q3ListViewItem *item);
public slots:
    void slotAddWatchVariable(const QString& watchVar);
    void slotEvaluateExpression(const QString& expression);

    //rgr
    void slotToggleRadix(Q3ListViewItem *item);

    void slotDbgStatus(const QString &status, int statusFlag);
    void slotParametersReady(const char* data);
    void slotLocalsReady(const char* data);
    void slotCurrentFrame(int frameNo, int threadNo);

private slots:
    void slotContextMenu(KListView *, Q3ListViewItem *item);

    // jw
    void slotDoubleClicked(Q3ListViewItem *item, const QPoint &pos, int c);

private: // helper functions
    /** Get (if exists) and create (otherwise) frame root for
        the specified frameNo/threadNo combination.
    */    
    VarFrameRoot* demand_frame_root(int frameNo, int threadNo);

private:
    int activeFlag_;
    int currentThread_;
    int currentFrame_;
    int iOutRadix;
    bool justPaused_;
    //DbgController *controller;

    // Root of all recently printed expressions.
    TrimmableItem* recentExpressions_;

    friend class VarFrameRoot;
    friend class VarItem;
    friend class WatchRoot;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/** List view item that can 'trim' outdated children.

    The instances of this class hold a number of children corresponding
    to variables. When program state changes, such as after a step in source,
    some variable values can change, and some variables can go out of scope.
    We need
    - highlight modified variables
    - remove gone variables

    We could just remove all children and repopulate the list from
    the data from debugger, but then we'd loose information about previous
    variable values.

    So, we first update the values, highlighting the modified variables, and
    keeping track which variables were recieved from gdb. After that, the
    'trim' method is called, removing all variables which were not recieved
    from gdbr.    
 */
class TrimmableItem : public KListViewItem
{
public:
    TrimmableItem(VariableTree *parent);
    TrimmableItem(TrimmableItem *parent);

    virtual ~TrimmableItem();

    virtual void trim();
    virtual QString getName() const         { return text(VarNameCol); }
    virtual TrimmableItem *findMatch(const QString& match, DataType type) const;
    Q3ListViewItem *lastChild() const;
    int  rootActiveFlag() const;
    void setActive()                        { activeFlag_ = rootActiveFlag(); }
    bool isActive() const                   { return activeFlag_ == rootActiveFlag(); }
    QString getValue() const                { return text(ValueCol); }
    bool isTrimmable() const;
    void waitingForData ()                  { waitingForData_ = true; }

    virtual void updateValue(char */* buf */);
    virtual DataType getDataType() const;

    virtual void setCache(const Q3CString& value);
    virtual Q3CString getCache();
    virtual QString key( int column, bool ascending ) const;

    // jw
    virtual void handleDoubleClicked(const QPoint &, int ) {}

protected:

    void paintCell( QPainter *p, const QColorGroup &cg,
                    int column, int width, int align );

private:
    int activeFlag_;
    bool waitingForData_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class VarItem : public TrimmableItem
{
public:
    VarItem( TrimmableItem *parent, const QString &varName, DataType dataType );

    virtual ~VarItem();

    QString varPath() const;
    QString fullName() const;
    DataType getDataType() const;

    void updateValue(char *data);

    // jw
    void updateType(char *data);

    void setCache(const Q3CString& value);
    Q3CString getCache();

    void setOpen(bool open);
    void setText (int column, const QString& text);

    // jw - overriden from TrimmableItem to handle renaming
    void handleDoubleClicked(const QPoint &pos, int c);

    // Returns the text to be displayed as tooltip (the value)
    QString tipText() const;

private:
    void checkForRequests();
    void paintCell( QPainter *p, const QColorGroup &cg,
                    int column, int width, int align );

private:
    Q3CString  cache_;
    DataType  dataType_;
    bool      highlight_;

    // the non-cast type of the variable
    Q3CString originalValueType_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class VarFrameRoot : public TrimmableItem
{
public:
    VarFrameRoot(VariableTree *parent, int frameNo, int threadNo);
    virtual ~VarFrameRoot();

    // Sets parameter information as passed from gdb.
    void setParams(const char *params);
    void setLocals(const char *locals);

    void setOpen(bool open);

    void setFrameName(const QString &frameName)
                { setText(VarNameCol, frameName); setText(ValueCol, ""); }

    bool needLocals() const                     { return needLocals_; }
    bool matchDetails(int frameNo, int threadNo);

private:
    bool    needLocals_;
    int     frameNo_;
    int     threadNo_;
    Q3CString params_;
    Q3CString locals_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class WatchRoot : public TrimmableItem
{
public:
    WatchRoot(VariableTree *parent);
    virtual ~WatchRoot();

    void requestWatchVars();
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

}

#endif
