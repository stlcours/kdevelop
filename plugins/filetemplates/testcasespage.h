/*
 * This file is part of KDevelop
 * Copyright 2012 Miha Čančula <miha@noughmad.eu>
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

#ifndef KDEVPLATFORM_PLUGIN_TESTCASESPAGE_H
#define KDEVPLATFORM_PLUGIN_TESTCASESPAGE_H

#include <QWidget>

#include "ipagefocus.h"

namespace KDevelop
{

/**
 * Assistant page for specifying the list of test cases
 *
 */
class TestCasesPage : public QWidget, public IPageFocus
{
    Q_OBJECT
    Q_PROPERTY(QStringList testCases READ testCases WRITE setTestCases)

public:
    explicit TestCasesPage(QWidget* parent = nullptr);
    ~TestCasesPage() override;

    /**
     * The name of the new test, as set by the user
     */
    QString name() const;

    /**
     * Returns the list of test case names
     */
    QStringList testCases() const;
    /**
     * Sets the current list of test case names to @p testCases
     */
    void setTestCases(const QStringList& testCases);

    void setFocusToFirstEditWidget() override;

Q_SIGNALS:
    void isValid(bool valid);

private:
    class TestCasesPagePrivate* const d;

private Q_SLOTS:
    void identifierChanged(const QString& identifier);
};

}

#endif // KDEVPLATFORM_PLUGIN_TESTCASESPAGE_H
