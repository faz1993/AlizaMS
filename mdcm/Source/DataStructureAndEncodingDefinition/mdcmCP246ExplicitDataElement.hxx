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

#ifndef MDCMCP246EXPLICITDATAELEMENT_TXX
#define MDCMCP246EXPLICITDATAELEMENT_TXX

#include "mdcmSequenceOfItems.h"
#include "mdcmSequenceOfFragments.h"
#include "mdcmVL.h"
#include "mdcmParseException.h"
#include "mdcmImplicitDataElement.h"
#include "mdcmValueIO.h"
#include "mdcmSwapper.h"

namespace mdcm
{

template <typename TSwap>
std::istream &
CP246ExplicitDataElement::Read(std::istream & is)
{
  ReadPreValue<TSwap>(is);
  return ReadValue<TSwap>(is);
}

template <typename TSwap>
std::istream &
CP246ExplicitDataElement::ReadPreValue(std::istream & is)
{
  TagField.Read<TSwap>(is);
  // See PS 3.5, Data Element Structure With Explicit VR
  // Read Tag
  if (!is)
  {
    if (!is.eof())
    {
      assert(0 && "Should not happen");
    }
    return is;
  }
  assert(TagField != Tag(0xfffe, 0xe0dd));
  const Tag itemDelItem(0xfffe, 0xe00d);
  if (TagField == itemDelItem)
  {
    if (!ValueLengthField.Read<TSwap>(is))
    {
      assert(0 && "Should not happen");
      return is;
    }
    if (ValueLengthField != 0)
    {
      mdcmDebugMacro("Item Delimitation Item has a length different from 0");
    }
    // Set pointer to NULL to avoid user error
    ValueField = 0;
    return is;
  }
  // Read VR
  try
  {
    if (!VRField.Read(is))
    {
      assert(0 && "Should not happen");
      return is;
    }
  }
  catch (std::exception &)
  {
    // mdcm-MR-PHILIPS-16-Multi-Seq.dcm
    // assert(TagField == Tag(0xfffe, 0xe000));
    // -> For some reason VR is written as {44,0} well I guess this is a VR...
    // Technically there is a second bug, dcmtk assume other things when reading this tag,
    // so I need to change this tag too, if I ever want dcmtk to read this file. oh well
    // 0019004_Baseline_IMG1.dcm
    // -> VR is garbage also...
    // assert(TagField == Tag(8348,0339) || TagField == Tag(b5e8,0338))
    mdcmWarningMacro("Assuming 16 bits VR for Tag=" << TagField << " in order to read a buggy DICOM file.");
    VRField = VR::INVALID;
  }
  // Read Value Length
  if (VR::GetLength(VRField) == 4)
  {
    if (!ValueLengthField.Read<TSwap>(is))
    {
      assert(0 && "Should not happen");
      return is;
    }
  }
  else
  {
    // 16bits only
    if (!ValueLengthField.template Read16<TSwap>(is))
    {
      // The following is occurs with mdcm 2.0.17 when two
      // seq del item marker are found
      // See UnexpectedSequenceDelimiterInFixedLengthSequence.dcm
      throw std::logic_error("Should not happen, CP246");
    }
  }
  return is;
}

template <typename TSwap>
std::istream &
CP246ExplicitDataElement::ReadValue(std::istream & is, bool readvalues)
{
  if (is.eof())
    return is;
  if (ValueLengthField == 0)
  {
    // Simple fast path
    ValueField = 0;
    return is;
  }
  // Read the Value
  if (VRField == VR::SQ)
  {
    // Check whether or not this is an undefined length sequence
    assert(TagField != Tag(0x7fe0, 0x0010));
    ValueField = new SequenceOfItems;
  }
  else if (ValueLengthField.IsUndefined())
  {
    if (VRField == VR::UN)
    {
      // Support cp246 conforming file:
      // Enhanced_MR_Image_Storage_PixelSpacingNotIn_0028_0030.dcm (illegal)
      // vs
      // undefined_length_un_vr.dcm
      assert(TagField != Tag(0x7fe0, 0x0010));
      ValueField = new SequenceOfItems;
      ValueField->SetLength(ValueLengthField); // perform realloc
      try
      {
        if (!ValueIO<CP246ExplicitDataElement, TSwap>::Read(is, *ValueField, readvalues)) // non cp246
        {
          assert(0);
        }
      }
      catch (std::exception &)
      {
        // Must be one of those non-cp246 file,
        // but for some reason seekg back to previous offset + Read
        // as CP246Explicit does not work.
        ParseException pe;
        pe.SetLastElement(*this);
        throw pe;
      }
      return is;
    }
    else
    {
      // Ok this is Pixel Data fragmented...
      assert(TagField == Tag(0x7fe0, 0x0010));
      assert(VRField & VR::OB_OW);
      ValueField = new SequenceOfFragments;
    }
  }
  else
  {
    ValueField = new ByteValue;
  }
  // We have the length, we should be able to read the value
  ValueField->SetLength(ValueLengthField); // perform realloc
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
  if (TagField == Tag(0x2001, 0xe05f) || TagField == Tag(0x2001, 0xe100) || TagField == Tag(0x2005, 0xe080) ||
      TagField == Tag(0x2005, 0xe083) || TagField == Tag(0x2005, 0xe084))
  // TagField.IsPrivate() && VRField == VR::SQ
  //-> Does not work for 0029
  // we really need to read item marker
  {
    mdcmWarningMacro("ByteSwaping Private SQ: " << TagField);
    assert(VRField == VR::SQ);
    try
    {
      if (!ValueIO<CP246ExplicitDataElement, SwapperDoOp>::Read(is, *ValueField, readvalues))
      {
        assert(0 && "Should not happen");
      }
    }
    catch (std::exception &)
    {
      ValueLengthField = ValueField->GetLength();
    }
    return is;
  }
#endif
  if (!ValueIO<CP246ExplicitDataElement, TSwap>::Read(is, *ValueField, readvalues))
  {
    // Might be the famous UN 16bits
    ParseException pe;
    pe.SetLastElement(*this);
    throw pe;
  }
  return is;
}

template <typename TSwap>
std::istream &
CP246ExplicitDataElement::ReadWithLength(std::istream & is, VL & length)
{
  (void)length;
  return Read<TSwap>(is);
}

} // end namespace mdcm

#endif // MDCMCP246EXPLICITDATAELEMENT_TXX
