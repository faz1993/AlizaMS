/*********************************************************
 *
 * MDCM
 *
 * Modifications github.com/issakomi
 *
 *********************************************************/

/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library

  Copyright (c) 2006-2011 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef MDCMSMARTPOINTER_H
#define MDCMSMARTPOINTER_H

#include "mdcmObject.h"

namespace mdcm
{

/**
 * Class for Smart Pointer
 *
 * Class partly based on post by Bill Hubauer
 * http://groups.google.com/group/comp.lang.c++/msg/173ddc38a827a930
 * http://www.davethehat.com/articles/smartp.htm
 * and itk::SmartPointer
 */

template <class ObjectType>
class SmartPointer
{
public:
  SmartPointer()
    : Pointer(NULL)
  {}

  SmartPointer(const SmartPointer<ObjectType> & p)
  {
    Pointer = p.Pointer;
    Register();
  }

#if 0
  SmartPointer(SmartPointer<ObjectType> && p)
  {
    Pointer = p.Pointer;
    p.Pointer = NULL;
  }
#endif

  SmartPointer(ObjectType * p)
  {
    Pointer = p;
    Register();
  }

  SmartPointer(const ObjectType & p)
  {
    Pointer = const_cast<ObjectType *>(&p);
    Register();
  }

  ~SmartPointer()
  {
    UnRegister();
    Pointer = NULL;
  }

  // Overload operator ->
  ObjectType * operator->() const { return Pointer; }

  ObjectType & operator*() const
  {
    assert(Pointer);
    return *Pointer;
  }

  // Return pointer to object.
  operator ObjectType *() const { return Pointer; }

  // Overload operator assignment.
  SmartPointer &
  operator=(const SmartPointer & r)
  {
    return operator=(r.Pointer);
  }

  // Overload operator assignment.
  SmartPointer &
  operator=(ObjectType * r)
  {
    // http://www.parashift.com/c++-faq-lite/freestore-mgmt.html#faq-16.22
    // DO NOT CHANGE THE ORDER OF THESE STATEMENTS!
    // This order properly handles self-assignment.
    // This order also properly handles recursion,
    // e.g. if a ObjectType contains SmartPointer<ObjectType>s)
    if (Pointer != r)
    {
      ObjectType * old = Pointer;
      Pointer = r;
      Register();
      if (old)
      {
        old->UnRegister();
      }
    }
    return *this;
  }

  SmartPointer &
  operator=(const ObjectType & r)
  {
    ObjectType * tmp = const_cast<ObjectType *>(&r);
    return       operator=(tmp);
  }

  // Explicit function to retrieve the pointer
  ObjectType *
  GetPointer() const
  {
    return Pointer;
  }

private:
  void
  Register()
  {
    if (Pointer)
      Pointer->Register();
  }

  void
  UnRegister()
  {
    if (Pointer)
      Pointer->UnRegister();
  }

  ObjectType * Pointer;
};

} // end namespace mdcm

#endif // MDCMSMARTPOINTER_H
