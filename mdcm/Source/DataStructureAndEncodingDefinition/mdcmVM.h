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
#ifndef MDCMVM_H
#define MDCMVM_H

#include "mdcmTypes.h"
#include "mdcmTrace.h"
#include <iostream>

namespace mdcm
{

/**
 * Value Multiplicity
 */

class MDCM_EXPORT VM
{
public:
// clang-format off
  typedef enum
  {
    VM0   = 0,
    VM1   = 1,
    VM2   = 2,
    VM3   = 4,
    VM4   = 8,
    VM5   = 16,
    VM6   = 32,
    VM8   = 64,
    VM9   = 128,
    VM10  = 256,
    VM12  = 512,
    VM16  = 1024,
    VM18  = 2048,
    VM24  = 4096,
    VM28  = 8192,
    VM32  = 16384,
    VM35  = 32768,
    VM99  = 65536,
    VM256 = 131072,
    VM1_2 = VM1 | VM2,
    VM1_3 = VM1 | VM2 | VM3,
    VM1_4 = VM1 | VM2 | VM3 | VM4,
    VM1_5 = VM1 | VM2 | VM3 | VM4 | VM5,
    VM1_8 = VM1 | VM2 | VM3 | VM4 | VM5 | VM6 | VM8,
    // The following need some work
    VM1_32 = VM1 | VM2 | VM3 | VM4 | VM5 | VM6 | VM8 | VM9 | VM16 | VM24 | VM32,
    VM1_99 = VM1 | VM2 | VM3 | VM4 | VM5 | VM6 | VM8 | VM9 | VM16 | VM24 | VM32 | VM99,
    VM1_n  = VM1 | VM2 | VM3 | VM4 | VM5 | VM6 | VM8 | VM9 | VM16 | VM24 | VM32 | VM99 | VM256,
    VM2_2n =       VM2       | VM4       | VM6 | VM8       | VM16 | VM24 | VM32        | VM256,
    VM2_n  =       VM2 | VM3 | VM4 | VM5 | VM6 | VM8 | VM9 | VM16 | VM24 | VM32 | VM99 | VM256,
    VM2_4  =       VM2 | VM3 | VM4,
    VM3_4  =             VM3 | VM4,
    VM3_3n =             VM3 |             VM6       | VM9        | VM24        | VM99 | VM256,
    VM3_n  =             VM3 | VM4 | VM5 | VM6 | VM8 | VM9 | VM16 | VM24 | VM32 | VM99 | VM256,
    VM4_4n =                   VM4                         | VM16 | VM24 | VM32        | VM256,
    VM6_6n =                               VM6             | VM12 | VM18 | VM24               ,
    VM6_n  =                               VM6 | VM8 | VM9 | VM16 | VM24 | VM32 | VM99 | VM256,
    VM7_7n,
    VM30_30n,
    VM47_47n,
    VM_END = VM1_n + 1
  } VMType;
// clang-format on

  static const char * GetVMString(VMType);
  static VMType
  GetVMType(const char *);
  // Check if vm1 is valid compare to vm2, i.e vm1 is element of vm2
  // vm1 is typically deduce from counting in a ValueField
  static bool
  IsValid(int vm1, VMType vm2);
  // WARNING: Implementation deficiency
  // The Compatible function is poorly implemented, the reference vm should be coming from
  // the dictionary, while the passed in value is the value guess from the file.
  bool
  Compatible(VM const &) const;
  static VMType
  GetVMTypeFromLength(size_t length, unsigned int size);
  static size_t
  GetNumberOfElementsFromArray(const char * array, size_t length);
  VM(VMType type = VM0)
    : VMField(type)
  {}
  operator VMType() const { return VMField; }
  unsigned int
  GetLength() const;
  friend std::ostream &
  operator<<(std::ostream &, const VM &);

protected:
  static unsigned int
  GetIndex(VMType vm);

private:
  VMType VMField;
};

inline std::ostream &
operator<<(std::ostream & _os, const VM & _val)
{
  assert(VM::GetVMString(_val));
  _os << VM::GetVMString(_val);
  return _os;
}

// clang-format off
template <int T>
struct VMToLength;
#define TYPETOLENGTH(type, length) \
  template <>                      \
  struct VMToLength<VM::type>      \
  {                                \
    enum                           \
    {                              \
      Length = length              \
    };                             \
  }

TYPETOLENGTH(VM1,     1);
TYPETOLENGTH(VM2,     2);
TYPETOLENGTH(VM3,     3);
TYPETOLENGTH(VM4,     4);
TYPETOLENGTH(VM5,     5);
TYPETOLENGTH(VM6,     6);
TYPETOLENGTH(VM8,     8);
TYPETOLENGTH(VM9,     9);
TYPETOLENGTH(VM10,   10);
TYPETOLENGTH(VM12,   12);
TYPETOLENGTH(VM16,   16);
TYPETOLENGTH(VM18,   18);
TYPETOLENGTH(VM24,   24);
TYPETOLENGTH(VM28,   28);
TYPETOLENGTH(VM32,   32);
TYPETOLENGTH(VM35,   35);
TYPETOLENGTH(VM99,   99);
TYPETOLENGTH(VM256, 256);

// clang-format on

} // end namespace mdcm

#endif // MDCMVM_H
