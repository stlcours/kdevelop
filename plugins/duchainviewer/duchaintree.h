/*
 * KDevelop DUChain viewer
 *
 * Copyright (c) 2007 Hamish Rodda <rodda@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DUCHAINTREE_H
#define DUCHAINTREE_H

#include <QtGui/QTreeView>

#include "duchainbase.h"
#include "duchainobserver.h"

class DUChainViewPart;

namespace KDevelop {
 class TopDUContext;
 class IDocument;
}

class DUChainTree : public QTreeView
{
  Q_OBJECT

public:
  DUChainTree(QWidget* parent, DUChainViewPart* part);
  virtual ~DUChainTree();

  class DUChainModel* model();

private Q_SLOTS:
  void contextActivated(const QModelIndex& index);

private:
  DUChainViewPart* m_part;
};

#endif

// kate: space-indent on; indent-width 2; tab-width: 4; replace-tabs on; auto-insert-doxygen on
