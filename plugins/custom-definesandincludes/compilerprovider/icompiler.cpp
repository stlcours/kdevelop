/*
 * This file is part of KDevelop
 *
 * Copyright 2014 Sergey Kalinichev <kalinichev.so.0@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "icompiler.h"

using namespace KDevelop;

ICompiler::ICompiler(const QString& name, const QString& path, const QString& factoryName, bool editable):
    m_editable(editable),
    m_name(name),
    m_path(path),
    m_factoryName(factoryName)
{}

void ICompiler::setPath(const QString& path)
{
    if (editable()) {
        m_path = path;
    }
}

QString ICompiler::path() const
{
    return m_path;
}

void ICompiler::setName(const QString& name)
{
    if (editable()) {
        m_name = name;
    }
}

QString ICompiler::name() const
{
    return m_name;
}

bool ICompiler::editable() const
{
    return m_editable;
}

QString ICompiler::factoryName() const
{
    return m_factoryName;
}
