/* This file is part of KDevelop
    Copyright (C) 2006 Hamish Rodda <rodda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QtTest/QtTest>
#include <QFlags>

#include "duchain.h"
#include "topducontext.h"
#include "declarationbuilder.h"
#include "usebuilder.h"
#include "declaration.h"
#include "kdevdocumentrange.h"
#include "cppeditorintegrator.h"
#include "typerepository.h"
#include "dumptypes.h"

#include "parser.h"
#include "control.h"
#include "dumpchain.h"
#include "tokens.h"
#include "parsesession.h"

#include "rpp/preprocessor.h"

using namespace KTextEditor;

namespace QTest {
  template<>
  char* toString(const Cursor& cursor)
  {
    QByteArray ba = "Cursor(";
    ba += QByteArray::number(cursor.line()) + ", " + QByteArray::number(cursor.column());
    ba += ')';
    return qstrdup(ba.data());
  }
  template<>
  char* toString(const QualifiedIdentifier& id)
  {
    QByteArray arr = id.toString().toLatin1();
    return qstrdup(arr.data());
  }
  template<>
  char* toString(const Declaration& def)
  {
    QString s = QString("Declaration %1 (%2): %3").arg(def.identifier().toString()).arg(def.qualifiedIdentifier().toString()).arg(reinterpret_cast<long>(&def));
    return qstrdup(s.toLatin1().constData());
  }
  template<>
  char* toString(const KSharedPtr<AbstractType>& type)
  {
    QString s = QString("Type: %1").arg(type ? type->toString() : QString("<null>"));
    return qstrdup(s.toLatin1().constData());
  }
}

class TestDUChain : public QObject
{
  Q_OBJECT

  // Parser
  Control control;
  DumpChain dumper;

  // Declaration - use chain
  Declaration* definition1;
  Declaration* definition2;
  Declaration* definition3;
  Declaration* definition4;
  Declaration* noDef;
  KUrl file1, file2;
  TopDUContext* topContext;

public:
  TestDUChain()
  {
  }

private slots:

  void initTestCase()
  {
    definition1 = new Declaration(new KDevDocumentRange(file1, Range(4,4,4,16)), Declaration::LocalScope);
    definition1->setIdentifier(Identifier("lazy"));

    definition2 = new Declaration(new KDevDocumentRange(file1, Range(5,4,5,16)), Declaration::ClassScope);
    definition2->setIdentifier(Identifier("m_errorCode"));

    definition3 = new Declaration(new KDevDocumentRange(file1, Range(6,4,6,16)), Declaration::GlobalScope);
    definition3->setIdentifier(Identifier("lazy"));

    definition4 = new Declaration(new KDevDocumentRange(file1, Range(7,4,7,16)), Declaration::ClassScope);
    definition4->setIdentifier(Identifier("m_errorCode2"));

    noDef = 0;

    file1 = "file:///opt/kde4/src/kdevelop/languages/cpp/parser/duchain.cpp";
    file2 = "file:///opt/kde4/src/kdevelop/languages/cpp/parser/dubuilder.cpp";

    topContext = new TopDUContext(new KDevDocumentRange(file1, Range(0,0,25,0)));
    DUChain::self()->addDocumentChain(file1, topContext);
  }

  void cleanupTestCase()
  {
    /*delete type1;
    delete type2;
    delete type3;*/

    delete definition1;
    delete definition2;
    delete definition3;
    delete definition4;

    delete topContext;
  }

  void testIdentifiers()
  {
    QualifiedIdentifier at("A::t");
    QualifiedIdentifier at2;
    at2.push(Identifier("A"));
    at2.push(Identifier("t"));
    QCOMPARE(at, at2);
    QVERIFY(at.isQualified());

    QualifiedIdentifier global("::test::foo()");
    QVERIFY(global.explicitlyGlobal());
    QVERIFY(global.isQualified());

    QualifiedIdentifier unqualified("unqualified");
    QVERIFY(!unqualified.isQualified());
  }

  void testContextRelationships()
  {
    QCOMPARE(DUChain::self()->chainForDocument(file1), topContext);

    DUContext* firstChild = new DUContext(new KDevDocumentRange(file1, Range(4,4, 10,3)));
    topContext->addChildContext(firstChild);

    QCOMPARE(firstChild->parentContext(), topContext);
    QCOMPARE(firstChild->childContexts().count(), 0);
    QCOMPARE(topContext->childContexts().count(), 1);
    QCOMPARE(topContext->childContexts().last(), firstChild);

    DUContext* secondChild = new DUContext(new KDevDocumentRange(file1, Range(14,4, 19,3)));
    topContext->addChildContext(secondChild);

    QCOMPARE(topContext->childContexts().count(), 2);
    QCOMPARE(topContext->childContexts()[1], secondChild);

    DUContext* thirdChild = new DUContext(new KDevDocumentRange(file1, Range(10,4, 14,3)));
    topContext->addChildContext(thirdChild);

    QCOMPARE(topContext->childContexts().count(), 3);
    QCOMPARE(topContext->childContexts()[1], thirdChild);

    delete topContext->takeChildContext(firstChild);
    QCOMPARE(topContext->childContexts().count(), 2);
    QCOMPARE(topContext->childContexts(), QList<DUContext*>() << thirdChild << secondChild);

    topContext->deleteChildContextsRecursively();
    QVERIFY(topContext->childContexts().isEmpty());
  }

  void testLocalDeclarations()
  {
    QCOMPARE(definition1->identifier(), Identifier("lazy"));
    QCOMPARE(definition1->scope(), Declaration::LocalScope);
    QCOMPARE(topContext->localDeclarations().count(), 0);

    topContext->addDeclaration(definition1);
    QCOMPARE(topContext->localDeclarations().count(), 1);
    QCOMPARE(topContext->localDeclarations()[0], definition1);

    topContext->addDeclaration(definition2);
    QCOMPARE(topContext->localDeclarations().count(), 2);
    QCOMPARE(topContext->localDeclarations()[1], definition2);

    //kDebug() << k_funcinfo << "Warning expected here (bug if not present)." << endl;
    //topContext->addDeclaration(definition2);
    //QCOMPARE(topContext->localDeclarations().count(), 2);

    topContext->clearLocalDeclarations();
    QVERIFY(topContext->localDeclarations().isEmpty());
  }

  void testDeclarationChain()
  {
    topContext->addDeclaration(definition1);
    topContext->addDeclaration(definition2);

    DUContext* context1 = new DUContext(new KDevDocumentRange(file1, Range(4,4, 14,3)));
    topContext->addChildContext(context1);
    Cursor insideContext1(5,9);

    QCOMPARE(topContext->findContext(insideContext1), context1);
    QCOMPARE(*topContext->findDeclaration(definition1->identifier(), insideContext1), *definition1);
    QCOMPARE(*context1->findDeclaration(definition1->identifier(), insideContext1), *definition1);

    context1->addDeclaration(definition3);
    QCOMPARE(*topContext->findDeclaration(definition1->identifier(), insideContext1), *definition1);
    QCOMPARE(*context1->findDeclaration(definition1->identifier(), insideContext1), *definition1);
    QCOMPARE(*context1->findDeclaration(definition1->identifier(), definition3->textRange().start()), *definition3);

    context1->takeDeclaration(definition3);
    QCOMPARE(*topContext->findDeclaration(definition1->identifier(), insideContext1), *definition1);
    QCOMPARE(*context1->findDeclaration(definition1->identifier(), insideContext1), *definition1);

    DUContext* subContext1 = new DUContext(new KDevDocumentRange(file1, Range(5,4, 7,3)));
    topContext->addChildContext(subContext1);

    DUContext* subContext2 = new DUContext(new KDevDocumentRange(file1, Range(9,4, 12,3)));
    topContext->addChildContext(subContext2);

    subContext1->addDeclaration(definition3);

    subContext2->addDeclaration(definition4);

    subContext1->takeDeclaration(definition3);
    subContext2->takeDeclaration(definition4);
    topContext->takeDeclaration(definition1);
    topContext->takeDeclaration(definition2);

    topContext->deleteChildContextsRecursively();
  }

  void testDeclareInt()
  {
    QByteArray method("int i;");

    DUContext* top = parse(method, DumpNone);

    QVERIFY(!top->parentContext());
    QCOMPARE(top->childContexts().count(), 0);
    QCOMPARE(top->localDeclarations().count(), 1);
    QVERIFY(top->localScopeIdentifier().isEmpty());

    Declaration* def = top->localDeclarations().first();
    QCOMPARE(def->identifier(), Identifier("i"));
    QCOMPARE(top->findDeclaration(def->identifier()), def);

    release(top);
  }

  void testIntegralTypes()
  {
    QByteArray method("const unsigned int i, k; volatile long double j; int* l; double * const * m; const int& n = l;");

    DUContext* top = parse(method, DumpNone);

    QVERIFY(!top->parentContext());
    QCOMPARE(top->childContexts().count(), 0);
    QCOMPARE(top->localDeclarations().count(), 6);
    QVERIFY(top->localScopeIdentifier().isEmpty());

    Declaration* defI = top->localDeclarations().first();
    QCOMPARE(defI->identifier(), Identifier("i"));
    QCOMPARE(top->findDeclaration(defI->identifier()), defI);
    QVERIFY(defI->type<CppIntegralType>());
    QCOMPARE(defI->type<CppIntegralType>()->integralType(), CppIntegralType::TypeInt);
    QCOMPARE(defI->type<CppIntegralType>()->typeModifiers(), CppIntegralType::ModifierUnsigned);
    QVERIFY(defI->type<CppIntegralType>()->isConstant());
    QVERIFY(!defI->type<CppIntegralType>()->isVolatile());

    Declaration* defK = top->localDeclarations()[1];
    QCOMPARE(defK->identifier(), Identifier("k"));
    QCOMPARE(defK->type<CppIntegralType>(), defI->type<CppIntegralType>());

    Declaration* defJ = top->localDeclarations()[2];
    QCOMPARE(defJ->identifier(), Identifier("j"));
    QCOMPARE(top->findDeclaration(defJ->identifier()), defJ);
    QVERIFY(defJ->type<CppIntegralType>());
    QCOMPARE(defJ->type<CppIntegralType>()->integralType(), CppIntegralType::TypeDouble);
    QCOMPARE(defJ->type<CppIntegralType>()->typeModifiers(), CppIntegralType::ModifierLong);
    QVERIFY(!defJ->type<CppIntegralType>()->isConstant());
    QVERIFY(defJ->type<CppIntegralType>()->isVolatile());

    Declaration* defL = top->localDeclarations()[3];
    QCOMPARE(defL->identifier(), Identifier("l"));
    QVERIFY(defL->type<CppPointerType>());
    QCOMPARE(CppIntegralType::Ptr::dynamicCast(defL->type<CppPointerType>()->baseType()), TypeRepository::self()->integral(CppIntegralType::TypeInt));
    QVERIFY(!defL->type<CppPointerType>()->isConstant());
    QVERIFY(!defL->type<CppPointerType>()->isVolatile());

    Declaration* defM = top->localDeclarations()[4];
    QCOMPARE(defM->identifier(), Identifier("m"));
    CppPointerType::Ptr firstpointer = defM->type<CppPointerType>();
    QVERIFY(firstpointer);
    QVERIFY(!firstpointer->isConstant());
    QVERIFY(!firstpointer->isVolatile());
    CppPointerType::Ptr secondpointer = CppPointerType::Ptr::dynamicCast(firstpointer->baseType());
    QVERIFY(secondpointer);
    QVERIFY(secondpointer->isConstant());
    QVERIFY(!secondpointer->isVolatile());
    CppIntegralType::Ptr base = CppIntegralType::Ptr::dynamicCast(secondpointer->baseType());
    QVERIFY(base);
    QCOMPARE(base->integralType(), CppIntegralType::TypeDouble);
    QVERIFY(!base->isConstant());
    QVERIFY(!base->isVolatile());

    Declaration* defN = top->localDeclarations()[5];
    QCOMPARE(defN->identifier(), Identifier("n"));
    QVERIFY(defN->type<CppReferenceType>());
    base = CppIntegralType::Ptr::dynamicCast(defN->type<CppReferenceType>()->baseType());
    QVERIFY(base);
    QCOMPARE(base->integralType(), CppIntegralType::TypeInt);
    QVERIFY(base->isConstant());
    QVERIFY(!base->isVolatile());

    release(top);
  }

  void testArrayType()
  {
    QByteArray method("int i[3];");

    DUContext* top = parse(method, DumpNone);

    QVERIFY(!top->parentContext());
    QCOMPARE(top->childContexts().count(), 0);
    QCOMPARE(top->localDeclarations().count(), 1);
    QVERIFY(top->localScopeIdentifier().isEmpty());

    Declaration* defI = top->localDeclarations().first();
    QCOMPARE(defI->identifier(), Identifier("i"));
    QCOMPARE(top->findDeclaration(defI->identifier()), defI);

    ArrayType::Ptr array = defI->type<ArrayType>();
    QVERIFY(array);
    CppIntegralType::Ptr element = CppIntegralType::Ptr::dynamicCast(array->elementType());
    QVERIFY(element);
    QCOMPARE(element->integralType(), CppIntegralType::TypeInt);
    QCOMPARE(array->dimension(), 3);

    release(top);
  }

  void testDeclareFor()
  {
    //                 0         1         2         3         4         5
    //                 012345678901234567890123456789012345678901234567890123456789
    QByteArray method("int main() { for (int i = 0; i < 10; i++) { if (i == 4) return; } }");

    DUContext* top = parse(method, DumpNone);

    QVERIFY(!top->parentContext());
    QCOMPARE(top->childContexts().count(), 2);
    QCOMPARE(top->localDeclarations().count(), 1);
    QVERIFY(top->localScopeIdentifier().isEmpty());

    Declaration* defMain = top->localDeclarations().first();
    QCOMPARE(defMain->identifier(), Identifier("main"));
    QCOMPARE(top->findDeclaration(defMain->identifier()), defMain);
    QVERIFY(defMain->type<CppFunctionType>());
    QCOMPARE(CppIntegralType::Ptr::dynamicCast(defMain->type<CppFunctionType>()->returnType()), TypeRepository::self()->integral(CppIntegralType::TypeInt));
    QCOMPARE(defMain->type<CppFunctionType>()->arguments().count(), 0);
    QVERIFY(!defMain->type<CppFunctionType>()->isConstant());
    QVERIFY(!defMain->type<CppFunctionType>()->isVolatile());

    QCOMPARE(top->findDeclaration(Identifier("i")), noDef);

    DUContext* main = top->childContexts()[1];
    QVERIFY(main->parentContext());
    QCOMPARE(main->importedParentContexts().count(), 1);
    QCOMPARE(main->childContexts().count(), 2);
    QCOMPARE(main->localDeclarations().count(), 0);
    QVERIFY(main->localScopeIdentifier().isEmpty());

    QCOMPARE(main->findDeclaration(Identifier("i")), noDef);

    DUContext* forCtx = main->childContexts()[1];
    QVERIFY(forCtx->parentContext());
    QCOMPARE(forCtx->importedParentContexts().count(), 1);
    QCOMPARE(forCtx->childContexts().count(), 2);
    QCOMPARE(forCtx->localDeclarations().count(), 0);
    QVERIFY(forCtx->localScopeIdentifier().isEmpty());

    DUContext* forParamCtx = forCtx->importedParentContexts().first();
    QVERIFY(forParamCtx->parentContext());
    QCOMPARE(forParamCtx->importedParentContexts().count(), 0);
    QCOMPARE(forParamCtx->childContexts().count(), 0);
    QCOMPARE(forParamCtx->localDeclarations().count(), 1);
    QVERIFY(forParamCtx->localScopeIdentifier().isEmpty());

    Declaration* defI = forParamCtx->localDeclarations().first();
    QCOMPARE(defI->identifier(), Identifier("i"));
    QCOMPARE(defI->uses().count(), 3);

    QCOMPARE(forCtx->findDeclaration(defI->identifier()), defI);

    DUContext* ifCtx = forCtx->childContexts()[1];
    QVERIFY(ifCtx->parentContext());
    QCOMPARE(ifCtx->importedParentContexts().count(), 1);
    QCOMPARE(ifCtx->childContexts().count(), 0);
    QCOMPARE(ifCtx->localDeclarations().count(), 0);
    QVERIFY(ifCtx->localScopeIdentifier().isEmpty());

    QCOMPARE(ifCtx->findDeclaration(defI->identifier()), defI);

    release(top);
  }

  void testDeclareStruct()
  {
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("struct A { int i; A(int b, int c) : i(c) { } virtual void test(int j) = 0; };");

    DUContext* top = parse(method, DumpNone);

    QVERIFY(!top->parentContext());
    QCOMPARE(top->childContexts().count(), 1);
    QCOMPARE(top->localDeclarations().count(), 1);
    QVERIFY(top->localScopeIdentifier().isEmpty());

    Declaration* defStructA = top->localDeclarations().first();
    QCOMPARE(defStructA->identifier(), Identifier("A"));
    QCOMPARE(defStructA->uses().count(), 0);
    QVERIFY(defStructA->type<CppClassType>());
    QCOMPARE(defStructA->type<CppClassType>()->elements().count(), 3);
    QCOMPARE(defStructA->type<CppClassType>()->elements().first(), AbstractType::Ptr::staticCast(TypeRepository::self()->integral(CppIntegralType::TypeInt)));
    QVERIFY(CppFunctionType::Ptr::dynamicCast(defStructA->type<CppClassType>()->elements()[1]));
    QCOMPARE(defStructA->type<CppClassType>()->classType(), CppClassType::Struct);

    DUContext* structA = top->childContexts().first();
    QVERIFY(structA->parentContext());
    QCOMPARE(structA->importedParentContexts().count(), 0);
    QCOMPARE(structA->childContexts().count(), 3);
    QCOMPARE(structA->localDeclarations().count(), 3);
    QCOMPARE(structA->localScopeIdentifier(), QualifiedIdentifier("A"));

    Declaration* defI = structA->localDeclarations().first();
    QCOMPARE(defI->identifier(), Identifier("i"));
    QCOMPARE(defI->uses().count(), 1);

    QCOMPARE(structA->findDeclaration(Identifier("i")), defI);
    QCOMPARE(structA->findDeclaration(Identifier("b")), noDef);
    QCOMPARE(structA->findDeclaration(Identifier("c")), noDef);

    DUContext* ctorImplCtx = structA->childContexts()[1];
    QVERIFY(ctorImplCtx->parentContext());
    QCOMPARE(ctorImplCtx->importedParentContexts().count(), 1);
    QCOMPARE(ctorImplCtx->childContexts().count(), 1);
    QCOMPARE(ctorImplCtx->localDeclarations().count(), 0);
    QVERIFY(ctorImplCtx->localScopeIdentifier().isEmpty());

    DUContext* ctorCtx = ctorImplCtx->importedParentContexts().first();
    QVERIFY(ctorCtx->parentContext());
    QCOMPARE(ctorCtx->childContexts().count(), 0);
    QCOMPARE(ctorCtx->localDeclarations().count(), 2);
    QVERIFY(ctorCtx->localScopeIdentifier().isEmpty());

    Declaration* defB = ctorCtx->localDeclarations().first();
    QCOMPARE(defB->identifier(), Identifier("b"));
    QCOMPARE(defB->uses().count(), 0);

    Declaration* defC = ctorCtx->localDeclarations()[1];
    QCOMPARE(defC->identifier(), Identifier("c"));
    QCOMPARE(defC->uses().count(), 1);

    QCOMPARE(ctorCtx->findDeclaration(Identifier("i")), defI);
    QCOMPARE(ctorCtx->findDeclaration(Identifier("b")), defB);
    QCOMPARE(ctorCtx->findDeclaration(Identifier("c")), defC);

    DUContext* testCtx = structA->childContexts().last();
    QCOMPARE(testCtx->childContexts().count(), 0);
    QCOMPARE(testCtx->localDeclarations().count(), 1);
    QVERIFY(testCtx->localScopeIdentifier().isEmpty());

    Declaration* defJ = testCtx->localDeclarations().first();
    QCOMPARE(defJ->identifier(), Identifier("j"));
    QCOMPARE(defJ->uses().count(), 0);

    /*DUContext* insideCtorCtx = ctorCtx->childContexts().first();
    QCOMPARE(insideCtorCtx->childContexts().count(), 0);
    QCOMPARE(insideCtorCtx->localDeclarations().count(), 0);
    QVERIFY(insideCtorCtx->localScopeIdentifier().isEmpty());*/

    release(top);
  }

  void testDeclareNamespace()
  {
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("namespace foo { int bar; } int bar; int test() { return foo::bar; }");

    DUContext* top = parse(method, DumpNone);

    QVERIFY(!top->parentContext());
    QCOMPARE(top->childContexts().count(), 3);
    QCOMPARE(top->localDeclarations().count(), 2);
    QVERIFY(top->localScopeIdentifier().isEmpty());
    QCOMPARE(top->findDeclaration(Identifier("foo")), noDef);

    QCOMPARE(top->usingNamespaces().count(), 0);

    DUContext* fooCtx = top->childContexts().first();
    QCOMPARE(fooCtx->childContexts().count(), 0);
    QCOMPARE(fooCtx->localDeclarations().count(), 1);
    QCOMPARE(fooCtx->localScopeIdentifier(), QualifiedIdentifier("foo"));
    QCOMPARE(fooCtx->scopeIdentifier(), QualifiedIdentifier("foo"));

    DUContext* testCtx = top->childContexts()[2];
    QCOMPARE(testCtx->childContexts().count(), 0);
    QCOMPARE(testCtx->localDeclarations().count(), 0);
    QCOMPARE(testCtx->localScopeIdentifier(), QualifiedIdentifier());
    QCOMPARE(testCtx->scopeIdentifier(), QualifiedIdentifier());

    Declaration* bar2 = top->localDeclarations().first();
    QCOMPARE(bar2->identifier(), Identifier("bar"));
    QCOMPARE(bar2->qualifiedIdentifier(), QualifiedIdentifier("bar"));
    QCOMPARE(bar2->uses().count(), 0);

    Declaration* bar = fooCtx->localDeclarations().first();
    QCOMPARE(bar->identifier(), Identifier("bar"));
    QCOMPARE(bar->qualifiedIdentifier(), QualifiedIdentifier("foo::bar"));
    QCOMPARE(testCtx->findDeclaration(QualifiedIdentifier("foo::bar")), bar);
    QCOMPARE(bar->uses().count(), 1);
    QCOMPARE(top->findDeclaration(bar->identifier()), bar2);
    QCOMPARE(top->findDeclaration(bar->qualifiedIdentifier()), bar);

    QCOMPARE(top->findDeclaration(QualifiedIdentifier("bar")), bar2);
    QCOMPARE(top->findDeclaration(QualifiedIdentifier("::bar")), bar2);
    QCOMPARE(top->findDeclaration(QualifiedIdentifier("foo::bar")), bar);
    QCOMPARE(top->findDeclaration(QualifiedIdentifier("::foo::bar")), bar);

    release(top);
  }

  void testDeclareUsingNamespace()
  {
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("namespace foo { int bar; } using namespace foo; int test() { return bar; }");

    DUContext* top = parse(method, DumpNone);

    QVERIFY(!top->parentContext());
    QCOMPARE(top->childContexts().count(), 3);
    QCOMPARE(top->localDeclarations().count(), 1);
    QVERIFY(top->localScopeIdentifier().isEmpty());
    QCOMPARE(top->findDeclaration(Identifier("foo")), noDef);

    QCOMPARE(top->usingNamespaces().count(), 1);
    QCOMPARE(top->usingNamespaces().first()->nsIdentifier, QualifiedIdentifier("foo"));
    QCOMPARE(top->usingNamespaces().first()->textCursor(), Cursor(0, 47));

    DUContext* fooCtx = top->childContexts().first();
    QCOMPARE(fooCtx->childContexts().count(), 0);
    QCOMPARE(fooCtx->localDeclarations().count(), 1);
    QCOMPARE(fooCtx->localScopeIdentifier(), QualifiedIdentifier("foo"));
    QCOMPARE(fooCtx->scopeIdentifier(), QualifiedIdentifier("foo"));

    Declaration* bar = fooCtx->localDeclarations().first();
    QCOMPARE(bar->identifier(), Identifier("bar"));
    QCOMPARE(bar->qualifiedIdentifier(), QualifiedIdentifier("foo::bar"));
    QCOMPARE(bar->uses().count(), 1);
    QCOMPARE(top->findDeclaration(bar->identifier(), top->textRange().start()), noDef);
    QCOMPARE(top->findDeclaration(bar->identifier()), bar);
    QCOMPARE(top->findDeclaration(bar->qualifiedIdentifier()), bar);

    QCOMPARE(top->findDeclaration(QualifiedIdentifier("bar")), bar);
    QCOMPARE(top->findDeclaration(QualifiedIdentifier("::bar")), noDef);
    QCOMPARE(top->findDeclaration(QualifiedIdentifier("foo::bar")), bar);
    QCOMPARE(top->findDeclaration(QualifiedIdentifier("::foo::bar")), bar);

    DUContext* testCtx = top->childContexts()[2];
    QVERIFY(testCtx->parentContext());
    QCOMPARE(testCtx->importedParentContexts().count(), 1);
    QCOMPARE(testCtx->childContexts().count(), 0);
    QCOMPARE(testCtx->localDeclarations().count(), 0);
    QCOMPARE(testCtx->localScopeIdentifier(), QualifiedIdentifier());
    QCOMPARE(testCtx->scopeIdentifier(), QualifiedIdentifier());

    release(top);
  }

  void testFileParse()
  {
    //QSKIP("Unwanted", SkipSingle);

    QFile file("/opt/kde4/src/kdevelop/languages/cpp/duchain/tests/files/membervariable.cpp");
    //QFile file("/opt/kde4/src/kdevelop/languages/csharp/parser/csharp_parser.cpp");
    //QFile file("/opt/kde4/src/kdelibs/kate/part/katecompletionmodel.h");
    QVERIFY( file.open( QIODevice::ReadOnly ) );

    QByteArray fileData = file.readAll();
    file.close();
    QString contents = QString::fromUtf8( fileData.constData() );
    Preprocessor preprocessor;
    QString ppd = preprocessor.processString( contents );
    QByteArray preprocessed = ppd.toUtf8();

    DUContext* top = parse(preprocessed);//, DumpNone);

    release(top);
  }

public:
  enum DumpArea {
    DumpNone = 0,
    DumpAST = 1,
    DumpDUChain = 2,
    DumpType = 4
  };
  Q_DECLARE_FLAGS(DumpAreas, DumpArea)

private:
  DUContext* parse(const QByteArray& unit, DumpAreas dump = static_cast<DumpAreas>(DumpAST | DumpDUChain | DumpType));

  void release(DUContext* top)
  {
    delete top;
  }
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TestDUChain::DumpAreas)

DUContext* TestDUChain::parse(const QByteArray& unit, DumpAreas dump)
{
  if (dump)
    kDebug() << "==== Beginning new test case...:" << endl << unit << endl << endl;

  ParseSession* session = new ParseSession();
  session->setContents(unit);

  Parser parser(&control);
  TranslationUnitAST* ast = parser.parse(session);
  ast->session = session;

  if (dump & DumpAST) {
    kDebug() << "===== AST:" << endl;
    dumper.dump(ast, session);
  }

  static int testNumber = 0;
  KUrl url(QString("file:///internal/%1").arg(testNumber++));

  DeclarationBuilder definitionBuilder(session);
  TopDUContext* top = definitionBuilder.buildDeclarations(url, ast);

  UseBuilder useBuilder(session);
  useBuilder.buildUses(ast);

  if (dump & DumpDUChain) {
    kDebug() << "===== DUChain:" << endl;
    dumper.dump(top);
  }

  if (dump & DumpType) {
    kDebug() << "===== Types:" << endl;
    DumpTypes dt;
    foreach (const AbstractType::Ptr& type, definitionBuilder.topTypes())
      dt.dump(type.data());
  }

  if (dump)
    kDebug() << "===== Finished test case." << endl;

  delete session;

  return top;
}

#include "test_duchain.moc"

QTEST_MAIN(TestDUChain)
