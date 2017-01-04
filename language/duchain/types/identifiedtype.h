/* This file is part of KDevelop
    Copyright 2002-2005 Roberto Raggi <roberto@kdevelop.org>
    Copyright 2006 Adam Treat <treat@kde.org>
    Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
    Copyright 2007-2008 David Nolden <david.nolden.kdevelop@art-master.de>

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

#ifndef KDEVPLATFORM_IDENTIFIEDTYPE_H
#define KDEVPLATFORM_IDENTIFIEDTYPE_H

#include "../identifier.h"
#include "../declarationid.h"

namespace KDevelop
{
class DUContext;
class Declaration;
class DeclarationId;
class TopDUContext;
class AbstractType;

/**
 * Data structure for identified types.
 */
class IdentifiedTypeData
{
public:
  DeclarationId m_id;
};

/**
 * \short An IdentifiedType is a type that has a declaration.
 *
 * Example of an identified type:
 * - A class type
 *
 * Example of types which are not identified:
 * - Pointer types (they can point to identified types, but by themselves have no declaration)
 * - Reference types (the same)
 * */
class KDEVPLATFORMLANGUAGE_EXPORT IdentifiedType
{
public:
  /// Destructor
  virtual ~IdentifiedType();

  /**
   * Test for equivalence with another type.
   *
   * \param rhs other type to check for equivalence
   * \returns true if equal, otherwise false.
   */
  bool equals(const IdentifiedType* rhs) const;

  /// Clear the identifier
  void clear();

  /**
   * Determine the qualified identifier for this identified type.
   *
   * \note This is relatively expensive. Use declarationId() instead when possible!
   * \returns the type's qualified identifier
   */
  QualifiedIdentifier qualifiedIdentifier() const;

  /// Determine this identified type's hash value. \returns the hash value
  uint hash() const;

  /**
   * Access the identified type's declaration id, which is a unique global identifier for the
   * type even when the same identifier represents a different type in a different context
   * or source file.
   *
   * \returns the declaration identifier.
   * \sa DeclarationId
   */
  DeclarationId declarationId() const;

  /**
   * Set the globally unique declaration \a id.
   * \param id new identifier
   */
  void setDeclarationId(const DeclarationId& id);

  /**
   * Look up the declaration of this type in the given \a top context.
   *
   * \param top Top context to search for the declaration within
   * \returns the declaration corresponding to this identified type
   */
  Declaration* declaration(const TopDUContext* top) const;

  /**
   * \param top Top context to search for the declaration within
   * Convenience function that returns the internal context of the attached declaration,
   * or zero if no declaration is found, or if it does not have an internal context.
   */
  DUContext* internalContext(const TopDUContext* top) const;
  
  /**
   * Set the declaration which created this type.
   *
   * \note You should be careful when setting this, because it also changes the meaning of the declaration.
   *
   * The logic is:
   * If a declaration has a set abstractType(), and that abstractType() has set the same declaration as declaration(),
   * then the declaration declares the type(thus it is a type-declaration, see Declaration::kind())
   *
   * \param declaration Declaration which created the type
   * */
  void setDeclaration(Declaration* declaration);

  /// Allow IdentifiedType to access its data.
  virtual IdentifiedTypeData* idData() = 0;
  /// Allow IdentifiedType to access its data.
  virtual const IdentifiedTypeData* idData() const = 0;
};

///Implements everything necessary to merge the given Parent class with IdentifiedType
///Your used Data class must be based on the Data member class
template<class Parent>
class MergeIdentifiedType : public Parent, public IdentifiedType {
  public:

    class Data : public Parent::Data, public IdentifiedTypeData {
    };

    MergeIdentifiedType() {
    }


    explicit MergeIdentifiedType(Data& data) : Parent(data) {
    }

    virtual IdentifiedTypeData* idData() {
      return static_cast<Data*>(this->d_func_dynamic());
    }

    virtual const IdentifiedTypeData* idData() const {
      return static_cast<const Data*>(this->d_func());
    }

    virtual bool equals(const KDevelop::AbstractType* rhs) const
    {
      if (!Parent::equals(rhs))
        return false;

      const IdentifiedType* rhsId = dynamic_cast<const IdentifiedType*>(rhs);
      Q_ASSERT(rhsId);

      return IdentifiedType::equals(static_cast<const IdentifiedType*>(rhsId));
    }

  private:
    MergeIdentifiedType(const MergeIdentifiedType& rhs);
};

}

#endif
