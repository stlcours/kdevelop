/*  This file is part of KDevelop
    Copyright 2012 Miha Čančula <miha@noughmad.eu>

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

#include "templateclassassistant.h"

#include "templateselectionpage.h"
#include "templateoptionspage.h"
#include "classmemberspage.h"
#include "classidentifierpage.h"
#include "overridespage.h"
#include "licensepage.h"
#include "outputpage.h"
#include "testcasespage.h"
#include "defaultcreateclasshelper.h"

#include <language/codegen/templateclassgenerator.h>
#include <language/codegen/sourcefiletemplate.h>
#include <language/codegen/documentchangeset.h>
#include <language/codegen/templaterenderer.h>
#include <language/interfaces/icreateclasshelper.h>
#include <language/interfaces/ilanguagesupport.h>

#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/ilanguage.h>
#include <interfaces/idocumentcontroller.h>

#include <KLocalizedString>
#include <KConfig>

#define REMOVE_PAGE(name)       \
if (d->name##Page)              \
{                               \
    removePage(d->name##Page);  \
    d->name##Page = 0;          \
    d->name##PageWidget = 0;    \
}

#define ZERO_PAGE(name)         \
d->name##Page = 0;              \
d->name##PageWidget = 0;

using namespace KDevelop;

class KDevelop::TemplateClassAssistantPrivate
{
public:
    TemplateClassAssistantPrivate(const KUrl& baseUrl);
    ~TemplateClassAssistantPrivate();

    KPageWidgetItem* templateSelectionPage;
    KPageWidgetItem* classIdentifierPage;
    KPageWidgetItem* overridesPage;
    KPageWidgetItem* membersPage;
    KPageWidgetItem* testCasesPage;
    KPageWidgetItem* licensePage;
    KPageWidgetItem* templateOptionsPage;
    KPageWidgetItem* outputPage;
    KPageWidgetItem* dummyPage;

    TemplateSelectionPage* templateSelectionPageWidget;
    ClassIdentifierPage* classIdentifierPageWidget;
    OverridesPage* overridesPageWidget;
    ClassMembersPage* membersPageWidget;
    TestCasesPage* testCasesPageWidget;
    LicensePage* licensePageWidget;
    TemplateOptionsPage* templateOptionsPageWidget;
    OutputPage* outputPageWidget;

    KUrl baseUrl;
    SourceFileTemplate fileTemplate;
    ICreateClassHelper* helper;
    TemplateClassGenerator* generator;
    TemplateRenderer* renderer;

    QString type;
    QVariantHash templateOptions;
};

TemplateClassAssistantPrivate::TemplateClassAssistantPrivate(const KUrl& baseUrl)
: baseUrl(baseUrl)
, helper(0)
, generator(0)
, renderer(0)
{
}

TemplateClassAssistantPrivate::~TemplateClassAssistantPrivate()
{
    delete helper;
    if (generator)
    {
        delete generator;
    }
    else
    {
        delete renderer;
    }
}

TemplateClassAssistant::TemplateClassAssistant(QWidget* parent, const KUrl& baseUrl)
: KAssistantDialog(parent)
, d(new TemplateClassAssistantPrivate(baseUrl))
{
    ZERO_PAGE(templateSelection)
    ZERO_PAGE(templateOptions)
    ZERO_PAGE(members)
    ZERO_PAGE(classIdentifier)
    ZERO_PAGE(overrides)
    ZERO_PAGE(license)
    ZERO_PAGE(output)
    ZERO_PAGE(testCases)

    setup();
}

TemplateClassAssistant::~TemplateClassAssistant()
{
    delete d;
}

void TemplateClassAssistant::setup()
{
    if (d->baseUrl.isValid())
    {
        setWindowTitle(i18n("Create Files from Template in <filename>%1</filename>", d->baseUrl.prettyUrl()));
    }
    else
    {
        setWindowTitle(i18n("Create Files from Template"));
    }

    d->templateSelectionPageWidget = new TemplateSelectionPage(this);
    connect(this, SIGNAL(accepted()), d->templateSelectionPageWidget, SLOT(saveConfig()));
    d->templateSelectionPage = addPage(d->templateSelectionPageWidget, i18n("Language and Template"));
    d->templateSelectionPage->setIcon(KIcon("project-development-new-template"));

    d->dummyPage = addPage(new QWidget(this), QLatin1String("Dummy Page"));
    showButton(KDialog::Help, false);
}

void TemplateClassAssistant::templateChosen(const QString& templateDescription)
{
    d->fileTemplate.setTemplateDescription(templateDescription);
    d->type = d->fileTemplate.type();

    d->generator = 0;

    if (!d->fileTemplate.isValid())
    {
        return;
    }

    kDebug() << "Selected template" << templateDescription << "of type" << d->type;
    removePage(d->dummyPage);

    if (d->baseUrl.isValid())
    {
        setWindowTitle(i18n("Create Files from Template <filename>%1</filename> in <filename>%2</filename>",
                            d->fileTemplate.name(),
                            d->baseUrl.prettyUrl()));
    }
    else
    {
        setWindowTitle(i18n("Create Files from Template <filename>%1</filename>", d->fileTemplate.name()));
    }

    if (d->type == "Class")
    {
        d->classIdentifierPageWidget = new ClassIdentifierPage(this);
        d->classIdentifierPage = addPage(d->classIdentifierPageWidget, i18n("Class Basics"));
        d->classIdentifierPage->setIcon(KIcon("classnew"));
        connect(d->classIdentifierPageWidget, SIGNAL(isValid(bool)), SLOT(setCurrentPageValid(bool)));
        setValid(d->classIdentifierPage, false);

        d->overridesPageWidget = new OverridesPage(this);
        d->overridesPage = addPage(d->overridesPageWidget, i18n("Override Methods"));
        d->overridesPage->setIcon(KIcon("code-class"));
        setValid(d->overridesPage, true);

        d->membersPageWidget = new ClassMembersPage(this);
        d->membersPage = addPage(d->membersPageWidget, i18n("Class Members"));
        d->membersPage->setIcon(KIcon("field"));
        setValid(d->membersPage, true);

        d->helper = 0;
        QString languageName = d->fileTemplate.languageName();
        ILanguage* language = ICore::self()->languageController()->language(languageName);
        if (language && language->languageSupport())
        {
            d->helper = language->languageSupport()->createClassHelper();
        }

        if (!d->helper)
        {
            kDebug() << "No class creation helper for language" << languageName;
            d->helper = new DefaultCreateClassHelper;
        }

        d->generator = d->helper->createGenerator(d->baseUrl);
        Q_ASSERT(d->generator);
        d->generator->setTemplateDescription(d->fileTemplate);
        d->renderer = d->generator->renderer();
    }
    else
    {
        if (d->type == "Test")
        {
            d->testCasesPageWidget = new TestCasesPage(this);
            d->testCasesPage = addPage(d->testCasesPageWidget, i18n("Test Cases"));
            connect(d->testCasesPageWidget, SIGNAL(isValid(bool)), SLOT(setCurrentPageValid(bool)));
            setValid(d->testCasesPage, false);
        }

        d->renderer = new TemplateRenderer;
        d->renderer->setEmptyLinesPolicy(TemplateRenderer::TrimEmptyLines);
        d->renderer->addArchive(d->fileTemplate.directory());
    }

    d->licensePageWidget = new LicensePage(this);
    d->licensePage = addPage(d->licensePageWidget, i18n("License"));
    d->licensePage->setIcon(KIcon("text-x-copying"));
    setValid(d->licensePage, true);

    d->outputPageWidget = new OutputPage(this);
    d->outputPageWidget->prepareForm(d->fileTemplate);
    d->outputPage = addPage(d->outputPageWidget, i18n("Output"));
    d->outputPage->setIcon(KIcon("document-save"));
    connect(d->outputPageWidget, SIGNAL(isValid(bool)), SLOT(setCurrentPageValid(bool)));
    setValid(d->outputPage, false);

    if (d->fileTemplate.hasCustomOptions())
    {
        kDebug() << "Class generator has custom options";
        d->templateOptionsPageWidget = new TemplateOptionsPage(this);
        d->templateOptionsPage = insertPage(d->outputPage, d->templateOptionsPageWidget,
                                            i18n("Template Options"));
    }

    setCurrentPage(d->templateSelectionPage);
}

void TemplateClassAssistant::next()
{
    kDebug() << currentPage()->name() << currentPage()->header();
    if (currentPage() == d->templateSelectionPage)
    {
        // We have chosen the template
        // Depending on the template's language, we can now create a helper
        QString description = d->templateSelectionPageWidget->selectedTemplate();
        templateChosen(description);
        if (!d->fileTemplate.isValid())
        {
            return;
        }
    }
    else if (currentPage() == d->classIdentifierPage)
    {
        d->generator->setIdentifier(d->classIdentifierPageWidget->identifier());
        foreach (const QString& base, d->classIdentifierPageWidget->inheritanceList())
        {
            d->generator->addBaseClass(base);
        }
    }
    else if (currentPage() == d->overridesPage)
    {
        ClassDescription desc = d->generator->description();
        foreach (const DeclarationPointer& declaration, d->overridesPageWidget->selectedOverrides())
        {
            desc.methods << FunctionDescription(declaration);
        }
        d->generator->setDescription(desc);
    }
    else if (currentPage() == d->membersPage)
    {
        ClassDescription desc = d->generator->description();
        desc.members << d->membersPageWidget->members();
        d->generator->setDescription(desc);
    }
    else if (currentPage() == d->licensePage)
    {
        if (d->generator)
        {
            d->generator->setLicense(d->licensePageWidget->license());
        }
        else
        {
            d->renderer->addVariable("license", d->licensePageWidget->license());
        }
    }
    else if (d->templateOptionsPage && (currentPage() == d->templateOptionsPage))
    {
        if (d->generator)
        {
            d->generator->addVariables(d->templateOptionsPageWidget->templateOptions());
        }
        else
        {
            d->renderer->addVariables(d->templateOptionsPageWidget->templateOptions());
        }
    }
    else if (currentPage() == d->testCasesPage)
    {
        d->renderer->addVariable("name", d->testCasesPageWidget->name());
        d->renderer->addVariable("testCases", d->testCasesPageWidget->testCases());
    }

    KAssistantDialog::next();

    if (currentPage() == d->classIdentifierPage)
    {
        d->classIdentifierPageWidget->setInheritanceList(d->fileTemplate.defaultBaseClasses());
    }
    else if (currentPage() == d->membersPage)
    {
        d->membersPage->widget()->setProperty("members", QVariant::fromValue(d->generator->description().members));
    }
    else if (currentPage() == d->overridesPage)
    {
        d->overridesPageWidget->clear();
        d->overridesPageWidget->addCustomDeclarations(i18n("Default"),
            d->helper->defaultMethods(d->generator->name()));
        d->overridesPageWidget->addBaseClasses(d->generator->directBaseClasses(),
                                               d->generator->allBaseClasses());
    }
    else if (d->templateOptionsPage && (currentPage() == d->templateOptionsPage))
    {
        d->templateOptionsPageWidget->load(d->fileTemplate, d->renderer);
    }
    else if (currentPage() == d->outputPage)
    {
        d->outputPageWidget->loadFileTemplate(d->fileTemplate, d->baseUrl, d->renderer);
    }
}

void TemplateClassAssistant::back()
{
    KAssistantDialog::back();
    if (currentPage() == d->templateSelectionPage)
    {
        REMOVE_PAGE(classIdentifier)
        REMOVE_PAGE(overrides)
        REMOVE_PAGE(members)
        REMOVE_PAGE(testCases)
        REMOVE_PAGE(output)
        REMOVE_PAGE(templateOptions)
        REMOVE_PAGE(license)

        delete d->helper;
        d->helper = 0;

        if (d->generator)
        {
            delete d->generator;
        }
        else
        {
            delete d->renderer;
        }
        d->generator = 0;
        d->renderer = 0;

        if (d->baseUrl.isValid())
        {
            setWindowTitle(i18n("Create Files from Template in <filename>%1</filename>", d->baseUrl.prettyUrl()));
        }
        else
        {
            setWindowTitle(i18n("Create Files from Template"));
        }
        d->dummyPage = addPage(new QWidget(this), QLatin1String("Dummy Page"));
    }
}

void TemplateClassAssistant::accept()
{
    // next() is not called for the last page (when the user clicks Finish), so we have to set output locations here
    QHash<QString, KUrl> fileUrls = d->outputPageWidget->fileUrls();
    QHash<QString, SimpleCursor> filePositions = d->outputPageWidget->filePositions();

    DocumentChangeSet changes;
    if (d->generator)
    {
        QHash<QString, KUrl>::const_iterator it = fileUrls.constBegin();
        for (; it != fileUrls.constEnd(); ++it)
        {
            d->generator->setFileUrl(it.key(), it.value());
            d->generator->setFilePosition(it.key(), filePositions.value(it.key()));
        }

        d->generator->addVariables(d->templateOptions);
        changes = d->generator->generate();
    }
    else
    {
        changes = d->renderer->renderFileTemplate(d->fileTemplate, d->baseUrl, fileUrls);
    }
    changes.applyAllChanges();

    foreach (const KUrl& url, fileUrls)
    {
        ICore::self()->documentController()->openDocument(url);
    }

    KAssistantDialog::accept();
}

void TemplateClassAssistant::setCurrentPageValid(bool valid)
{
    setValid(currentPage(), valid);
}

KUrl TemplateClassAssistant::baseUrl() const
{
    return d->baseUrl;
}
