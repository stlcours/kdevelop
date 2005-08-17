/***************************************************************************
*   Copyright (C) 2001 by Daniel Engelschalt                              *
*   daniel.engelschalt@gmx.net                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef _CCCONFIGWIDGET_H_
#define _CCCONFIGWIDGET_H_

#include "ccconfigwidgetbase.h"
#include <qmap.h>

class CppSupportPart;
class Catalog;
class Q3CheckListItem;

class CCConfigWidget : public CCConfigWidgetBase
{
	Q_OBJECT

public:
	CCConfigWidget( CppSupportPart* part, QWidget* parent = 0, const char* name = 0 );
	virtual ~CCConfigWidget( );

public slots:
	void accept( );
	void slotNewPCS();

protected slots:
	void slotGetterSetterValuesChanged();
	virtual void slotRemovePCS();

private slots:
	void catalogRegistered( Catalog* c );
	void catalogUnregistered( Catalog* c );

private:
	void initGeneralTab();
	void saveFileTemplatesTab();

	void initCodeCompletionTab();
	void saveCodeCompletionTab();

	void initGetterSetterTab();
	void saveGetterSetterTab();

private:
	CppSupportPart* m_pPart;
	QMap<Q3CheckListItem*, Catalog*> m_catalogs;
};

#endif 
// kate: indent-mode csands; tab-width 4;

