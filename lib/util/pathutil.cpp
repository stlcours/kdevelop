/***************************************************************************
 *   Copyright (C) 2001-2002 by Jakob Simon-Gaarde                         *
 *   jakob@simon-gaarde                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include <qstringlist.h>
#include <qfile.h>
#include <qdir.h>
#include "pathutil.h"
//#include <iostream>

QString getRelativePath(const QString& basepath, const QString& destpath)
{
  QString relpath = ".";
  if (!QFile::exists(basepath) ||
      !QFile::exists(destpath))
    return "";
  QStringList basedirs = QStringList::split('/',basepath);
  QStringList destdirs = QStringList::split('/',destpath);
  
  int maxCompare=0;
  if (basedirs.count()>=destdirs.count())
    maxCompare=destdirs.count();
  else
    maxCompare=basedirs.count();
  int lastCommonDir=-1;
  for (int i=0; i<maxCompare; i++)
  {
    if (basedirs[i] != destdirs[i])
      break;      
    lastCommonDir=i;
  }  
  for (uint i=0;i<basedirs.count()-(lastCommonDir+1); i++)
    relpath += "/..";
  for (int i=0; i<lastCommonDir+1; i++)
    destdirs.pop_front();
  if (destdirs.count())
    relpath += "/"+destdirs.join("/");
  return QDir::cleanDirPath(relpath);
}

/*
int main(int argc, char** argv)
{
  QString destpath,basepath;
  if (argc<2)
    return 0;
  if (argc==2)
  {
    char cwd[1026];
    destpath = argv[1];
    getcwd(cwd,1024);
    basepath = cwd;
  }
  else
  {
    destpath = argv[2];
    basepath = argv[1];
  }
  std::cout << getRelativePath(basepath,destpath).ascii() << "\0";
  return 1;
}
*/
