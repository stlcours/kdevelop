/* This file is part of KDevelop
    Copyright (C) 2003 Roberto Raggi <roberto@kdevelop.org>

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "codemodel.h"
#include <kdebug.h>

CodeModel::CodeModel()
{
    wipeout();
}

CodeModel::~ CodeModel( )
{
}

void CodeModel::wipeout()
{
    m_files.clear();
    NamespaceDom ns = create<NamespaceModel>();
    ns->setName( "::" );

    m_globalNamespace = ns;
}

FileList CodeModel::fileList( )
{
    return m_files.values();
}

const FileList CodeModel::fileList( ) const
{
    return m_files.values();
}

bool CodeModel::hasFile( const QString & name ) const
{
    return m_files.contains( name );
}

FileDom CodeModel::fileByName( const QString & name )
{
    return m_files.contains(name) ? m_files[ name ] : FileDom();
}

const FileDom CodeModel::fileByName( const QString & name ) const
{
    return m_files.contains(name) ? m_files[ name ] : FileDom();
}

bool CodeModel::addFile( FileDom file )
{
    if( file->name().isEmpty() )
	return false;

    // update global namespace
    NamespaceList namespaceList = file->namespaceList();
    ClassList classList = file->classList();
    FunctionList functionList = file->functionList();
    VariableList variableList = file->variableList();

    for( NamespaceList::Iterator it=namespaceList.begin(); it!=namespaceList.end(); ++it )
	m_globalNamespace->addNamespace( *it );
    for( ClassList::Iterator it=classList.begin(); it!=classList.end(); ++it )
	m_globalNamespace->addClass( *it );
    for( FunctionList::Iterator it=functionList.begin(); it!=functionList.end(); ++it )
	m_globalNamespace->addFunction( *it );
    for( VariableList::Iterator it=variableList.begin(); it!=variableList.end(); ++it )
	m_globalNamespace->addVariable( *it );

    m_files.insert( file->name(), file );
    return true;
}

void CodeModel::removeFile( FileDom file )
{
    // update global namespace
    NamespaceList namespaceList = file->namespaceList();
    ClassList classList = file->classList();
    FunctionList functionList = file->functionList();
    VariableList variableList = file->variableList();

    for( NamespaceList::Iterator it=namespaceList.begin(); it!=namespaceList.end(); ++it )
	m_globalNamespace->removeNamespace( *it );
    for( ClassList::Iterator it=classList.begin(); it!=classList.end(); ++it )
	m_globalNamespace->removeClass( *it );
    for( FunctionList::Iterator it=functionList.begin(); it!=functionList.end(); ++it )
	m_globalNamespace->removeFunction( *it );
    for( VariableList::Iterator it=variableList.begin(); it!=variableList.end(); ++it )
	m_globalNamespace->removeVariable( *it );

    m_files.remove( file->name() );
}

// ------------------------------------------------------------------------
CodeModelItem::CodeModelItem( int kind, CodeModel* model )
    : m_kind( kind ), m_model( model )
{
    //kdDebug() << "CodeModelItem::CodeModelItem()" << endl;
    m_startLine = 0;
    m_startColumn = 0;
    m_endLine = 0;
    m_endColumn = 0;
}

CodeModelItem::~ CodeModelItem( )
{
}

QString CodeModelItem::name( ) const
{
    return m_name;
}

void CodeModelItem::setName( const QString & name )
{
    m_name = name;
}

const FileDom CodeModelItem::file( ) const
{
    return m_model->fileByName( m_fileName );
}

FileDom CodeModelItem::file( )
{
    return m_model->fileByName( m_fileName );
}

QString CodeModelItem::fileName() const
{
    return m_fileName;
}

void CodeModelItem::setFileName( const QString& fileName )
{
    m_fileName = fileName;
}

void CodeModelItem::getStartPosition( int * line, int * column ) const
{
    if( line ) *line = m_startLine;
    if( column ) *column = m_startColumn;
}

void CodeModelItem::setStartPosition( int line, int column )
{
    m_startLine = line;
    m_startColumn = column;
}

void CodeModelItem::getEndPosition( int * line, int * column ) const
{
    if( line ) *line = m_endLine;
    if( column ) *column = m_endColumn;
}

void CodeModelItem::setEndPosition( int line, int column )
{
    m_endLine = line;
    m_endColumn = column;
}

// ------------------------------------------------------------------------
NamespaceModel::NamespaceModel( CodeModel* model )
    : ClassModel( model )
{
    setKind( Namespace );
}

NamespaceList NamespaceModel::namespaceList( )
{
    return m_namespaces.values();
}

const NamespaceList NamespaceModel::namespaceList( ) const
{
    return m_namespaces.values();
}

NamespaceDom NamespaceModel::namespaceByName( const QString & name )
{
    return m_namespaces.contains( name ) ? m_namespaces[ name ] : NamespaceDom();
}

const NamespaceDom NamespaceModel::namespaceByName( const QString & name ) const
{
    return m_namespaces.contains( name ) ? m_namespaces[ name ] : NamespaceDom();
}

bool NamespaceModel::hasNamespace( const QString & name ) const
{
    return m_namespaces.contains( name );
}

bool NamespaceModel::addNamespace( NamespaceDom ns )
{
    if( ns->name().isEmpty() || m_namespaces.contains(ns->name()) )
	return false;

    m_namespaces.insert( ns->name(), ns );
    return true;
}

void NamespaceModel::removeNamespace( NamespaceDom ns )
{
    m_namespaces.remove( ns->name() );
}

// ------------------------------------------------------------------------
FileModel::FileModel( CodeModel* model )
    : NamespaceModel( model)
{
}

// ------------------------------------------------------------------------
ClassModel::ClassModel( CodeModel* model )
    : CodeModelItem( Class, model)
{
}

QStringList ClassModel::baseClassList( ) const
{
    return m_baseClassList;
}

bool ClassModel::addBaseClass( const QString & baseClass )
{
    m_baseClassList.push_back( baseClass );
    return true;
}

void ClassModel::removeBaseClass( const QString & baseClass )
{
    m_baseClassList.remove( baseClass );
}

ClassList ClassModel::classList( )
{
    ClassList l;
    QMap<QString, ClassList>::Iterator it = m_classes.begin();
    while( it != m_classes.end() ){
	l += *it;
	++it;
    }

    return l;
}

const ClassList ClassModel::classList( ) const
{
    ClassList l;
    QMap<QString, ClassList>::ConstIterator it = m_classes.begin();
    while( it != m_classes.end() ){
	l += *it;
	++it;
    }

    return l;
}

bool ClassModel::hasClass( const QString & name ) const
{
    return m_classes.contains( name );
}

ClassList ClassModel::classByName( const QString & name )
{
    return m_classes.contains( name ) ? m_classes[ name ] : ClassList();
}

const ClassList ClassModel::classByName( const QString & name ) const
{
    return m_classes.contains( name ) ? m_classes[ name ] : ClassList();
}

bool ClassModel::addClass( ClassDom klass )
{
    if( klass->name().isEmpty() )
	return false;

    m_classes[ klass->name() ].push_back( klass );
    return true;
}

void ClassModel::removeClass( ClassDom klass )
{
    m_classes[ klass->name() ].remove( klass );

    if( m_classes[klass->name()].isEmpty() )
	m_classes.remove( klass->name() );
}

FunctionList ClassModel::functionList( )
{
    FunctionList l;
    QMap<QString, FunctionList>::Iterator it = m_functions.begin();
    while( it != m_functions.end() ){
	l += *it;
	++it;
    }

    return l;
}

const FunctionList ClassModel::functionList( ) const
{
    FunctionList l;
    QMap<QString, FunctionList>::ConstIterator it = m_functions.begin();
    while( it != m_functions.end() ){
	l += *it;
	++it;
    }

    return l;
}

bool ClassModel::hasFunction( const QString & name ) const
{
    return m_functions.contains( name );
}

FunctionList ClassModel::functionByName( const QString & name )
{
    return m_functions.contains( name ) ? m_functions[ name ] : FunctionList();
}

const FunctionList ClassModel::functionByName( const QString & name ) const
{
    return m_functions.contains( name ) ? m_functions[ name ] : FunctionList();
}

bool ClassModel::addFunction( FunctionDom fun )
{
    if( fun->name().isEmpty() )
	return false;

    m_functions[ fun->name() ].push_back( fun );
    return true;
}

void ClassModel::removeFunction( FunctionDom fun )
{
    m_functions[ fun->name() ].remove( fun );

    if( m_functions[fun->name()].isEmpty() )
	m_functions.remove( fun->name() );
}

VariableList ClassModel::variableList( )
{
    return m_variables.values();
}

const VariableList ClassModel::variableList( ) const
{
    return m_variables.values();
}

VariableDom ClassModel::variableByName( const QString & name )
{
    return m_variables.contains( name ) ? m_variables[ name ] : VariableDom();
}

const VariableDom ClassModel::variableByName( const QString & name ) const
{
    return m_variables.contains( name ) ? m_variables[ name ] : VariableDom();
}

bool ClassModel::hasVariable( const QString & name ) const
{
    return m_variables.contains( name );
}

bool ClassModel::addVariable( VariableDom var )
{
    if( var->name().isEmpty() )
	return false;

    m_variables.insert( var->name(), var );
    return true;
}

void ClassModel::removeVariable( VariableDom var )
{
    m_variables.remove( var->name() );
}

// ------------------------------------------------------------------------
ArgumentModel::ArgumentModel( CodeModel* model )
    : CodeModelItem( Argument, model)
{
}

QString ArgumentModel::type( ) const
{
    return m_type;
}

void ArgumentModel::setType( const QString& type )
{
    m_type = type;
}

QString ArgumentModel::defaultValue( ) const
{
    return m_defaultValue;
}

void ArgumentModel::setDefaultValue( const QString & defaultValue )
{
    m_defaultValue = defaultValue;
}

// ------------------------------------------------------------------------
FunctionModel::FunctionModel( CodeModel* model )
    : CodeModelItem( Function, model)
{
    m_access = Public;
    d.v.m_signal = false;
    d.v.m_slot = false;
    d.v.m_virtual = false;
    d.v.m_static = false;
    d.v.m_inline = false;
    d.v.m_constant = false;
    d.v.m_abstract = false;
    m_implStartLine = m_implEndLine = 0;
    m_implStartColumn = m_implEndColumn = 0;
}

bool FunctionModel::isVirtual( ) const
{
    return d.v.m_virtual;
}

void FunctionModel::setVirtual( bool isVirtual )
{
    d.v.m_virtual = isVirtual;
}

bool FunctionModel::isStatic( ) const
{
    return d.v.m_static;
}

void FunctionModel::setStatic( bool isStatic )
{
    d.v.m_static = isStatic;
}

bool FunctionModel::isInline( ) const
{
    return d.v.m_inline;
}

void FunctionModel::setInline( bool isInline )
{
    d.v.m_inline = isInline;
}

bool FunctionModel::isConstant( ) const
{
    return d.v.m_constant;
}

void FunctionModel::setConstant( bool isConstant )
{
    d.v.m_constant = isConstant;
}

bool FunctionModel::isAbstract( ) const
{
    return d.v.m_abstract;
}

void FunctionModel::setAbstract( bool isAbstract )
{
    d.v.m_abstract = isAbstract;
}

QString FunctionModel::resultType( ) const
{
    return m_resultType;
}

void FunctionModel::setResultType( const QString& type )
{
    m_resultType = type;
}

ArgumentList FunctionModel::argumentList( )
{
    return m_arguments;
}

const ArgumentList FunctionModel::argumentList( ) const
{
    return m_arguments;
}

bool FunctionModel::addArgument( ArgumentDom arg )
{
    m_arguments.push_back( arg );
    return true;
}

void FunctionModel::removeArgument( ArgumentDom arg )
{
    m_arguments.remove( arg );
}

bool FunctionModel::hasImplementation( ) const
{
    return !m_implementedInFile.isEmpty();
}

QString FunctionModel::implementedInFile( ) const
{
    return m_implementedInFile;
}

void FunctionModel::setImplementedInFile( const QString & fileName )
{
    m_implementedInFile = fileName;
}

void FunctionModel::getImplementationStartPosition( int * line, int * col ) const
{
    if( line ) *line = m_implStartLine;
    if( col ) *col = m_implStartColumn;
}

void FunctionModel::setImplementationStartPosition( int line, int col )
{
    m_implStartLine = line;
    m_implStartColumn = col;
}

void FunctionModel::getImplementationEndPosition( int * line, int * col ) const
{
    if( line ) *line = m_implEndLine;
    if( col ) *col = m_implEndColumn;
}

void FunctionModel::setImplementationEndPosition( int line, int col )
{
    m_implEndLine = line;
    m_implEndColumn = col;
}

// ------------------------------------------------------------------------
VariableModel::VariableModel( CodeModel* model )
    : CodeModelItem( Variable, model)
{
    m_access = Public;
    m_static = false;
}

bool VariableModel::isStatic( ) const
{
    return m_static;
}

void VariableModel::setStatic( bool isStatic )
{
    m_static = isStatic;
}

QString VariableModel::type( ) const
{
    return m_type;
}

void VariableModel::setType( const QString& type )
{
    m_type = type;
}

int FunctionModel::access( ) const
{
    return m_access;
}

void FunctionModel::setAccess( int access )
{
    m_access = access;
}

bool FunctionModel::isSignal( ) const
{
    return d.v.m_signal;
}

void FunctionModel::setSignal( bool isSignal )
{
    d.v.m_signal = isSignal;
}

bool FunctionModel::isSlot( ) const
{
    return d.v.m_slot;
}

void FunctionModel::setSlot( bool isSlot )
{
    d.v.m_slot = isSlot;
}

int VariableModel::access( ) const
{
    return m_access;
}

void VariableModel::setAccess( int access )
{
    m_access = access;
}

NamespaceDom CodeModel::globalNamespace( )
{
    return m_globalNamespace;
}

const NamespaceDom CodeModel::globalNamespace( ) const
{
    return m_globalNamespace;
}

void CodeModelItem::read( QDataStream & stream )
{
    stream
	>> m_kind
	>> m_name
	>> m_fileName
	>> m_startLine
	>> m_startColumn
	>> m_endLine
	>> m_endColumn;
}

void CodeModelItem::write( QDataStream & stream ) const
{
    stream
	<< m_kind
	<< m_name
	<< m_fileName
	<< m_startLine
	<< m_startColumn
	<< m_endLine
	<< m_endColumn;
}

void ClassModel::read( QDataStream & stream )
{
    CodeModelItem::read( stream );

    stream >> m_baseClassList;

    int n;

    m_classes.clear();
    stream >> n;
    for( int i=0; i<n; ++i ){
	ClassDom klass = codeModel()->create<ClassModel>();
	klass->read( stream );
	addClass( klass );
    }

    m_functions.clear();
    stream >> n;
    for( int i=0; i<n; ++i ){
	FunctionDom fun = codeModel()->create<FunctionModel>();
	fun->read( stream );
	addFunction( fun );
    }

    m_variables.clear();
    stream >> n;
    for( int i=0; i<n; ++i ){
	VariableDom var = codeModel()->create<VariableModel>();
	var->read( stream );
	addVariable( var );
    }
}

void ClassModel::write( QDataStream & stream ) const
{
    CodeModelItem::write( stream );

    stream << m_baseClassList;

    const ClassList class_list = classList();
    stream << int( class_list.size() );
    for( ClassList::ConstIterator it = class_list.begin(); it!=class_list.end(); ++it )
	(*it)->write( stream );

    const FunctionList function_list = functionList();
    stream << int( function_list.size() );
    for( FunctionList::ConstIterator it = function_list.begin(); it!=function_list.end(); ++it )
	(*it)->write( stream );

    const VariableList variable_list = variableList();
    stream << int( variable_list.size() );
    for( VariableList::ConstIterator it = variable_list.begin(); it!=variable_list.end(); ++it )
	(*it)->write( stream );
}

void NamespaceModel::read( QDataStream & stream )
{
    ClassModel::read( stream );

    int n;

    m_namespaces.clear();
    stream >> n;
    for( int i=0; i<n; ++i ){
	NamespaceDom ns = codeModel()->create<NamespaceModel>();
	ns->read( stream );
	addNamespace( ns );
    }
}

void NamespaceModel::write( QDataStream & stream ) const
{
    ClassModel::write( stream );

    const NamespaceList namespace_list = namespaceList();
    stream << int( namespace_list.size() );
    for( NamespaceList::ConstIterator it = namespace_list.begin(); it!=namespace_list.end(); ++it )
	(*it)->write( stream );
}

void FileModel::read( QDataStream & stream )
{
    NamespaceModel::read( stream );
}

void FileModel::write( QDataStream & stream ) const
{
    NamespaceModel::write( stream );
}

void ArgumentModel::read( QDataStream & stream )
{
    CodeModelItem::read( stream );

    stream >> m_type >> m_defaultValue;
}

void ArgumentModel::write( QDataStream & stream ) const
{
    CodeModelItem::write( stream );

    stream << m_type << m_defaultValue;
}

void FunctionModel::read( QDataStream & stream )
{
    CodeModelItem::read( stream );

    stream >> d.flags;

    int n;

    m_arguments.clear();
    stream >> n;
    for( int i=0; i<n; ++i ){
	ArgumentDom arg = codeModel()->create<ArgumentModel>();
	arg->read( stream );
	addArgument( arg );
    }

    stream
	>> m_implementedInFile
	>> m_implStartLine >> m_implEndLine
	>> m_implStartColumn >> m_implEndColumn
	>> m_resultType;
}

void FunctionModel::write( QDataStream & stream ) const
{
    CodeModelItem::write( stream );

    stream << d.flags;

    const ArgumentList argument_list = argumentList();
    stream << int( argument_list.size() );
    for( ArgumentList::ConstIterator it = argument_list.begin(); it!=argument_list.end(); ++it )
	(*it)->write( stream );

    stream
	<< m_implementedInFile
	<< m_implStartLine << m_implEndLine
	<< m_implStartColumn << m_implEndColumn
	<< m_resultType;
}

void CodeModel::read( QDataStream & stream )
{
    int n;

    m_files.clear();

    stream >> n;
    for( int i=0; i<n; ++i ){
	FileDom file = this->create<FileModel>();
	file->read( stream );
	addFile( file );
    }
}

void CodeModel::write( QDataStream & stream ) const
{
    const FileList file_list = fileList();
    stream << int( file_list.size() );
    for( FileList::ConstIterator it = file_list.begin(); it!=file_list.end(); ++it )
	(*it)->write( stream );
}

void VariableModel::read( QDataStream & stream )
{
    CodeModelItem::read( stream );
    stream >> m_access >> m_static >> m_type;
}

void VariableModel::write( QDataStream & stream ) const
{
    CodeModelItem::write( stream );
    stream << m_access << m_static << m_type;
}

