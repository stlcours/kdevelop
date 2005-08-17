/***************************************************************************
 *   Copyright (C) 2004 by Jens Dagerbo                                    *
 *   jens.dagerbo@swipnet.se                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __KDEVPART_CTAGS2_H__
#define __KDEVPART_CTAGS2_H__

#include <qpointer.h>
#include <qstring.h>
//Added by qt3to4:
#include <QTextStream>
#include <Q3PopupMenu>

#include <kdevplugin.h>
#include "tags.h"

class Q3PopupMenu;
class Context;
class KURL;
class QTextStream;
class CTags2Widget;
class ConfigWidgetProxy;
class KDialogBase;

class CTags2Part : public KDevPlugin
{
  Q_OBJECT

public:
   
	CTags2Part(QObject *parent, const char *name, const QStringList &);
	~CTags2Part();

	int getFileLineFromPattern( KURL const & url, QString const & pattern);
	bool createTagsFile();

private slots:
	void slotLookupDeclaration();
	void slotLookupDefinition();
	void slotLookup();
	void slotOpenLookup();
	void slotGoToNext();
	void slotGotoDefinition();
	void slotGotoDeclaration();
	void slotGotoTag();
	void contextMenu(Q3PopupMenu *, const Context *);
	void insertConfigWidget( const KDialogBase * dlg, QWidget * page, unsigned int );
	void updateTagsfileName( const QString & name );
	
private:
	int getFileLineFromStream( QTextStream & stream, QString const & pattern);
	void gotoTagForTypes( QStringList const & types );
	void showHits( Tags::TagList const & tags );
	QString currentWord();
    
	QPointer<CTags2Widget> m_widget;
	QString m_contextString ;
	
	ConfigWidgetProxy * _configProxy;
};


#endif

// kate: space-indent off; indent-width 4; tab-width 4; show-tabs off;
