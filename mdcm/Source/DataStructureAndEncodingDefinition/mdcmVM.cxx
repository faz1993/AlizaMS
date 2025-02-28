/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library

  Copyright (c) 2006-2011 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mdcmVM.h"
#include <cstdlib>
#include <cstring>

namespace mdcm
{

static const char * VMStrings[] = { "INVALID", "1",    "2",   "3",    "4",    "5",    "6",      "8",      "9",   "10",
                                    "12",      "16",   "18",  "24",   "28",   "32",   "35",     "99",     "256", "1-2",
                                    "1-3",     "1-4",  "1-5", "1-8",  "1-32", "1-99", "1-n",    "2-2n",   "2-n", "2-4",
                                    "3-4",     "3-3n", "3-n", "4-4n", "6-6n", "7-7n", "30-30n", "47-47n", 0 };

unsigned int
VM::GetLength() const
{
  unsigned int len = 0;
  switch (VMField)
  {
    case VM::VM1:
      len = VMToLength<VM::VM1>::Length;
      break;
    case VM::VM2:
      len = VMToLength<VM::VM2>::Length;
      break;
    case VM::VM3:
      len = VMToLength<VM::VM3>::Length;
      break;
    case VM::VM4:
      len = VMToLength<VM::VM4>::Length;
      break;
    case VM::VM5:
      len = VMToLength<VM::VM5>::Length;
      break;
    case VM::VM6:
      len = VMToLength<VM::VM6>::Length;
      break;
    case VM::VM8:
      len = VMToLength<VM::VM8>::Length;
      break;
    case VM::VM9:
      len = VMToLength<VM::VM9>::Length;
      break;
    case VM::VM10:
      len = VMToLength<VM::VM10>::Length;
      break;
    case VM::VM12:
      len = VMToLength<VM::VM12>::Length;
      break;
    case VM::VM16:
      len = VMToLength<VM::VM16>::Length;
      break;
    case VM::VM18:
      len = VMToLength<VM::VM18>::Length;
      break;
    case VM::VM24:
      len = VMToLength<VM::VM24>::Length;
      break;
    case VM::VM28:
      len = VMToLength<VM::VM28>::Length;
      break;
    case VM::VM32:
      len = VMToLength<VM::VM32>::Length;
      break;
    case VM::VM35:
      len = VMToLength<VM::VM35>::Length;
      break;
    case VM::VM99:
      len = VMToLength<VM::VM99>::Length;
      break;
    case VM::VM256:
      len = VMToLength<VM::VM256>::Length;
      break;
    case VM::VM0:
    case VM::VM1_2:
    case VM::VM1_3:
    case VM::VM1_4:
    case VM::VM1_5:
    case VM::VM1_8:
    case VM::VM1_32:
    case VM::VM1_99:
    case VM::VM1_n:
    case VM::VM2_2n:
    case VM::VM2_n:
    case VM::VM2_4:
    case VM::VM3_4:
    case VM::VM3_3n:
    case VM::VM3_n:
    case VM::VM4_4n:
    case VM::VM6_6n:
    case VM::VM7_7n:
    default:
      break;
  }
  assert(len);
  return len;
}

unsigned int
VM::GetIndex(VMType vm)
{
  assert(vm <= VM::VM_END);
  unsigned int l = 0;
  switch (vm)
  {
    case VM::VM0:
      break;
    case VM::VM1_2:
      l = 19;
      break;
    case VM::VM1_3:
      l = 20;
      break;
    case VM::VM1_4:
      l = 21;
      break;
    case VM::VM1_5:
      l = 22;
      break;
    case VM::VM1_8:
      l = 23;
      break;
    case VM::VM1_32:
      l = 24;
      break;
    case VM::VM1_99:
      l = 25;
      break;
    case VM::VM1_n:
      l = 26;
      break;
    case VM::VM2_2n:
      l = 27;
      break;
    case VM::VM2_n:
      l = 28;
      break;
    case VM::VM2_4:
      l = 29;
      break;
    case VM::VM3_4:
      l = 30;
      break;
    case VM::VM3_3n:
      l = 31;
      break;
    case VM::VM3_n:
      l = 32;
      break;
    case VM::VM4_4n:
      l = 33;
      break;
    case VM::VM6_6n:
      l = 34;
      break;
    case VM::VM7_7n:
      l = 35;
      break;
    case VM::VM30_30n:
      l = 36;
      break;
    case VM::VM47_47n:
      l = 37;
      break;
    case VM::VM_END:
      l = 38;
      break;
    default:
    {
      unsigned int a = (unsigned int)vm;
      for (; a > 1; ++l)
        a >>= 1;
      l++;
    }
    break;
  }
  return l;
}

const char *
VM::GetVMString(VMType vm)
{
  unsigned int idx = GetIndex(vm);
  assert(idx < sizeof(VMStrings) / sizeof(VMStrings[0]));
  return VMStrings[idx];
}

VM::VMType
VM::GetVMType(const char * vm)
{
  if (!vm)
    return VM::VM_END;
  if (!*vm)
    return VM::VM0; // FIXME
  for (int i = 0; VMStrings[i] != NULL; ++i)
  {
    if (strcmp(VMStrings[i], vm) == 0)
    {
      return (VM::VMType)(i);
    }
  }
  return VM::VM_END;
}

// FIXME IsValid will only work for VM defined in public dict
bool
VM::IsValid(int vm1, VMType vm2)
{
  bool r = false;
  assert(vm1 >= 0);
  switch (vm2)
  {
    case VM::VM1:
      r = (vm1 == 1);
      break;
    case VM::VM2:
      r = (vm1 == 2);
      break;
    case VM::VM3:
      r = (vm1 == 3);
      break;
    case VM::VM4:
      r = (vm1 == 4);
      break;
    case VM::VM5:
      r = (vm1 == 5);
      break;
    case VM::VM6:
      r = (vm1 == 6);
      break;
    case VM::VM8:
      r = (vm1 == 8);
      break;
    case VM::VM16:
      r = (vm1 == 16);
      break;
    case VM::VM24:
      r = (vm1 == 24);
      break;
    case VM::VM1_2:
      r = (vm1 == 1 || vm1 == 2);
      break;
    case VM::VM1_3:
      r = (vm1 >= 1 && vm1 <= 3);
      break;
    case VM::VM2_4:
      r = (vm1 >= 2 && vm1 <= 4);
      break;
    case VM::VM1_8:
      r = (vm1 >= 1 && vm1 <= 8);
      break;
    case VM::VM1_32:
      r = (vm1 >= 1 && vm1 <= 32);
      break;
    case VM::VM1_99:
      r = (vm1 >= 1 && vm1 <= 99);
      break;
    case VM::VM1_n:
      r = (vm1 >= 1);
      break;
    case VM::VM2_2n:
      r = (vm1 >= 2 && !(vm1 % 2));
      break;
    case VM::VM2_n:
      r = (vm1 >= 2);
      break;
    case VM::VM3_3n:
      r = (vm1 >= 3 && !(vm1 % 3));
      break;
    case VM::VM3_n:
      r = (vm1 >= 3);
      break;
    default:
      assert(0);
      break;
  }
  return r;
}

// This function should not be used in production code.
// Indeed this only return a 'guess' at the VM (ie. a lower bound)
VM::VMType
VM::GetVMTypeFromLength(size_t length, unsigned int size)
{
  if (!length || length % size)
    return VM::VM0;
  const unsigned int ratio = (unsigned int)(length / size);
  switch (ratio)
  {
    case 1:
      return VM::VM1;
    case 2:
      return VM::VM2;
    case 3:
      return VM::VM3;
    case 4:
      return VM::VM4;
    case 5:
      return VM::VM5;
    case 6:
      return VM::VM6;
    case 8:
      return VM::VM8;
    case 9:
      return VM::VM9;
    case 16:
      return VM::VM16;
    case 24:
      return VM::VM24;
    case 32:
      return VM::VM32;
    default:
      return VM::VM1_n;
  }
}

size_t
VM::GetNumberOfElementsFromArray(const char * array, size_t length)
{
  size_t c = 0;
  if (!length || !array)
    return 0;
  const char * parray = array;
  const char * end = array + length;
  bool         valuefound = false;
  for (; parray != end; ++parray)
  {
    if (*parray == ' ')
    {
      ;
      ;
    }
    else if (*parray == '\\')
    {
      if (valuefound)
      {
        ++c;
        valuefound = false;
      }
    }
    else
    {
      valuefound = true;
    }
  }
  if (valuefound)
    ++c;
  return c;
}

bool
VM::Compatible(VM const & vm) const
{
  if (VMField == VM::VM0)
    return false;
  if (vm == VM::VM0)
    return true;
  if (VMField == vm.VMField)
    return true;
  bool r = false;
  switch (VMField)
  {
    case VM::VM1_2:
      r = (vm.VMField >= VM::VM1 && vm.VMField <= VM::VM2);
      break;
    case VM::VM1_3:
      r = (vm.VMField >= VM::VM1 && vm.VMField <= VM::VM3);
      break;
    case VM::VM1_8:
      r = (vm.VMField >= VM::VM1 && vm.VMField <= VM::VM8);
      break;
    case VM::VM1_32:
      r = (vm.VMField >= VM::VM1 && vm.VMField <= VM::VM32);
      break;
    case VM::VM1_99:
      r = (vm.VMField >= VM::VM1 && vm.VMField <= VM::VM99);
      break;
    case VM::VM1_n:
      r = (vm.VMField >= VM::VM1);
      break;
    case VM::VM2_2n:
    {
      if (vm == VM1_n)
        r = true; // FIXME
      else
        r = vm.VMField >= VM::VM2 && !(vm.GetLength() % 2);
    }
    break;
    case VM::VM2_n:
      r = (vm.VMField >= VM::VM2);
      break;
    case VM::VM2_4:
      r = (vm.VMField >= VM::VM2 && vm.VMField <= VM::VM4);
      break;
    case VM::VM3_4:
      r = (vm.VMField == VM::VM3 || vm.VMField == VM::VM4);
      break;
    case VM::VM3_3n:
      r = (vm.VMField >= VM::VM3 && !(vm.GetLength() % 3));
      break;
    case VM::VM3_n:
      r = (vm.VMField >= VM::VM3);
      break;
    default:
      r = (VMField == vm.VMField);
  }
  if (r)
  {
    assert(VMField & vm.VMField);
  }
  return r;
}

} // end namespace mdcm
