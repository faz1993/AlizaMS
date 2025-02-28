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
#ifndef MDCMIMPLICITDATAELEMENT_TXX
#define MDCMIMPLICITDATAELEMENT_TXX

#include "mdcmSequenceOfItems.h"
#include "mdcmValueIO.h"
#include "mdcmSwapper.h"
#ifdef MDCM_WORDS_BIGENDIAN
#  include "mdcmTagToVR.h"
#endif

namespace mdcm
{

template <typename TSwap>
std::istream &
ImplicitDataElement::Read(std::istream & is)
{
  ReadPreValue<TSwap>(is);
  return ReadValue<TSwap>(is);
}

template <typename TSwap>
std::istream &
ImplicitDataElement::ReadPreValue(std::istream & is)
{
  TagField.Read<TSwap>(is);
  // See PS 3.5, 7.1.3 Data Element Structure With Implicit VR
  if (!is)
  {
    if (!is.eof()) // FIXME This should not be needed
    {
      assert(0 && "Should not happen");
    }
    return is;
  }
  const Tag itemStartItem(0xfffe, 0xe000);
  if (TagField == itemStartItem)
    return is;
  // Read Value Length
  if (!ValueLengthField.Read<TSwap>(is))
  {
    throw std::logic_error("Impossible ValueLengthField");
  }
  return is;
}

template <typename TSwap>
std::istream &
ImplicitDataElement::ReadValue(std::istream & is, bool readvalues)
{
  if (is.eof())
    return is;
  const Tag itemStartItem(0xfffe, 0xe000);
  assert(TagField != itemStartItem);
  /*
   * Technically this should not be needed, but what if an implementor, forgot
   * to set VL = 0, then we should make sure to exit early.
   */
  const Tag itemDelItem(0xfffe, 0xe00d);
  if (TagField == itemDelItem)
  {
    if (ValueLengthField != 0)
    {
      mdcmWarningMacro("VL should be set to 0");
    }
    ValueField = 0;
    return is;
  }
  if (ValueLengthField == 0)
  {
    ValueField = 0;
    return is;
  }
  else if (ValueLengthField.IsUndefined())
  {
    // FIXME what if I am reading the pixel data...
    // assert(TagField != Tag(0x7fe0,0x0010));
    if (TagField != Tag(0x7fe0, 0x0010))
    {
      ValueField = new SequenceOfItems;
    }
    else
    {
      mdcmErrorMacro(
        "Undefined value length is impossible in non-encapsulated Transfer Syntax. Proceeding with caution");
      ValueField = new SequenceOfFragments;
    }
  }
  else
  {
    if (true /*ValueLengthField < 8 */)
    {
      ValueField = new ByteValue;
    }
    else
    {
      // In the following we read 4 more bytes in the Value field
      // to find out if this is a SQ or not
      // there is still work to do to handle the PMS featured SQ
      // where item Start is in fact 0xfeff, 0x00e0 ... sigh
      const Tag itemStart(0xfffe, 0xe000);
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
      const Tag itemPMSStart(0xfeff, 0x00e0);
      const Tag itemPMSStart2(0x3f3f, 0x3f00);
#endif
      Tag item;
      // TODO FIXME why not `peek` directly?
      item.Read<TSwap>(is);
      // Maybe seek back is slow
      is.seekg(-4, std::ios::cur);
      if (item == itemStart)
      {
        assert(TagField != Tag(0x7fe0, 0x0010));
        ValueField = new SequenceOfItems;
      }
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
      else if (item == itemPMSStart)
      {
        // MR_Philips_Intera_No_PrivateSequenceImplicitVR.dcm
        mdcmWarningMacro("Illegal Tag for Item starter: " << TagField << " should be: " << itemStart);
        // TODO: We READ Explicit ok... but we store Implicit!
        // Indeed when copying the VR will be saved.
        ValueField = new SequenceOfItems;
        ValueField->SetLength(ValueLengthField);
        std::streampos start = is.tellg();
        try
        {
          if (!ValueIO<ExplicitDataElement, SwapperDoOp>::Read(is, *ValueField, readvalues))
          {
            assert(0 && "Should not happen");
          }
          mdcmWarningMacro("Illegal: Explicit SQ found in a file with "
                           "TransferSyntax=Implicit for tag: "
                           << TagField);
        }
        catch (std::logic_error &)
        {
          // MR_ELSCINT1_00e1_1042_SQ_feff_00e0_Item.dcm
          std::streampos current = is.tellg();
          std::streamoff diff = start - current;
          is.seekg(diff, std::ios::cur);
          assert(diff == -14);
          ValueIO<ImplicitDataElement, SwapperDoOp>::Read(is, *ValueField, readvalues);
        }
        catch (std::exception &)
        {
          ValueLengthField = ValueField->GetLength();
        }
        return is;
      }
      else if (item == itemPMSStart2 && false)
      {
        mdcmWarningMacro("Illegal: SQ start with " << itemPMSStart2 << " instead of " << itemStart
                                                   << " for tag: " << TagField);
        ValueField = new SequenceOfItems;
        ValueField->SetLength(ValueLengthField);
        if (!ValueIO<ImplicitDataElement, TSwap>::Read(is, *ValueField, readvalues))
        {
          assert(0 && "Should not happen");
        }
        return is;
      }
#endif
      else
      {
        ValueField = new ByteValue;
      }
    }
  }
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
  // THE WORST BUG EVER. From GE Workstation
  if (ValueLengthField == 13)
  {
    // Historically mdcm did not enforce proper length
    // thus Theralys started writing illegal DICOM images:
    const Tag theralys1(0x0008, 0x0070);
    const Tag theralys2(0x0008, 0x0080);
    if (TagField != theralys1 && TagField != theralys2)
    {
      mdcmWarningMacro(
        "GE,13: Replacing VL=0x000d with VL=0x000a, for Tag=" << TagField << " in order to read a buggy DICOM file.");
      ValueLengthField = 10;
    }
  }
#endif
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
  if (ValueLengthField == 0x31f031c && TagField == Tag(0x031e, 0x0324))
  {
    // TestImages/elbow.pap
    mdcmWarningMacro("Replacing a VL. To be able to read a supposively "
                     "broken Papyrus file.");
    ValueLengthField = 202; // 0xca
  }
#endif
  // We have the length we should be able to read the value
  this->SetValueFieldLength(ValueLengthField, readvalues);
  bool failed;
#ifdef MDCM_WORDS_BIGENDIAN
  VR vrfield = GetVRFromTag(TagField);
  if (vrfield & VR::VRASCII || vrfield == VR::INVALID)
  {
    failed = !ValueIO<ImplicitDataElement, TSwap>::Read(is, *ValueField, readvalues);
  }
  else
  {
    assert(vrfield & VR::VRBINARY);
    unsigned int vrsize = vrfield.GetSize();
    assert(vrsize == 1 || vrsize == 2 || vrsize == 4 || vrsize == 8);
    if (vrfield == VR::AT)
      vrsize = 2;
    if (vrfield == VR::OF || vrfield == VR::OD || vrfield == VR::FD || vrfield == VR::FL)
    {
      failed = !ValueIO<ImplicitDataElement, TSwap>::Read(is, *ValueField, readvalues);
    }
    else
    {
      switch (vrsize)
      {
        case 1:
          failed = !ValueIO<ImplicitDataElement, TSwap, uint8_t>::Read(is, *ValueField, readvalues);
          break;
        case 2:
          failed = !ValueIO<ImplicitDataElement, TSwap, uint16_t>::Read(is, *ValueField, readvalues);
          break;
        case 4:
          failed = !ValueIO<ImplicitDataElement, TSwap, uint32_t>::Read(is, *ValueField, readvalues);
          break;
        case 8:
          failed = !ValueIO<ImplicitDataElement, TSwap, uint64_t>::Read(is, *ValueField, readvalues);
          break;
        default:
          failed = true;
          assert(0);
      }
    }
  }
#else
  failed = !ValueIO<ImplicitDataElement, TSwap>::Read(is, *ValueField, readvalues);
#endif
  if (failed)
  {
    // Special handling for PixelData tag:
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
    if (TagField == Tag(0x7fe0, 0x0010))
    {
      mdcmAlwaysWarnMacro("Pixel Data may be corrupted");
      is.clear();
    }
    else
#endif /* MDCM_SUPPORT_BROKEN_IMPLEMENTATION */
    {
      throw std::logic_error("Should not happen (imp)");
    }
    return is;
  }
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
  // dcmtk 3.5.4 is resilient to broken explicit SQ length and will properly
  // recompute it as long as each of the Item lengths are correct
  VL dummy = ValueField->GetLength();
  if (ValueLengthField != dummy)
  {
    mdcmWarningMacro("ValueLengthField was bogus");
    assert(0);
    ValueLengthField = dummy;
  }
#else
  assert(ValueLengthField == ValueField->GetLength());
  assert(VRField == VR::INVALID);
#endif
  return is;
}

template <typename TSwap>
std::istream &
ImplicitDataElement::ReadWithLength(std::istream & is, VL & length, bool readvalues)
{
  ReadPreValue<TSwap>(is);
  return ReadValueWithLength<TSwap>(is, length, readvalues);
}

template <typename TSwap>
std::istream &
ImplicitDataElement::ReadValueWithLength(std::istream & is, VL & length, bool readvalues)
{
  if (is.eof())
    return is;
  const Tag itemStartItem(0xfffe, 0xe000);
  if (TagField == itemStartItem)
    return is;
  /*
   * Technically this should not be needed, but what if an implementor, forgot
   * to set VL = 0, then we should make sure to exit early.
   */
  const Tag itemDelItem(0xfffe, 0xe00d);
  if (TagField == itemDelItem)
  {
    if (ValueLengthField != 0)
    {
      mdcmAlwaysWarnMacro("VL should be set to 0, correcting");
    }
    ValueField = 0;
    return is;
  }
  if (ValueLengthField > length && !ValueLengthField.IsUndefined())
  {
    mdcmAlwaysWarnMacro("ValueLengthField > length && !ValueLengthField.IsUndefined()");
    throw std::logic_error("Impossible (more)");
  }
  if (ValueLengthField == 0)
  {
    ValueField = 0;
    return is;
  }
  else if (ValueLengthField.IsUndefined())
  {
    if (TagField != Tag(0x7fe0, 0x0010))
    {
      ValueField = new SequenceOfItems;
    }
    else
    {
      mdcmAlwaysWarnMacro("Undefined value length is impossible in non-encapsulated Transfer Syntax");
      ValueField = new SequenceOfFragments;
    }
  }
  else
  {
    if (true /*ValueLengthField < 8*/)
    {
      ValueField = new ByteValue;
    }
    else
    {
      // In the following we read 4 more bytes in the Value field
      // to find out if this is a SQ or not
      // there is still work to do to handle the PMS featured SQ
      // where item Start is in fact 0xfeff, 0x00e0 ... sigh
      const Tag itemStart(0xfffe, 0xe000);
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
      const Tag itemPMSStart(0xfeff, 0x00e0);
      const Tag itemPMSStart2(0x3f3f, 0x3f00);
#endif
      Tag item;
      // TODO FIXME why not `peek` directly ?
      item.Read<TSwap>(is);
      // Maybe seek back is slow
      is.seekg(-4, std::ios::cur);
      if (item == itemStart)
      {
        assert(TagField != Tag(0x7fe0, 0x0010));
        ValueField = new SequenceOfItems;
      }
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
      else if (item == itemPMSStart)
      {
        // MR_Philips_Intera_No_PrivateSequenceImplicitVR.dcm
        mdcmWarningMacro("Illegal Tag for Item starter: " << TagField << " should be: " << itemStart);
        // TODO: We READ Explicit ok...but we store Implicit !
        // Indeed when copying the VR will be saved.
        ValueField = new SequenceOfItems;
        ValueField->SetLength(ValueLengthField); // perform realloc
        std::streampos start = is.tellg();
        try
        {
          if (!ValueIO<ExplicitDataElement, SwapperDoOp>::Read(is, *ValueField, readvalues))
          {
            assert(0 && "Should not happen");
          }
          mdcmAlwaysWarnMacro("Illegal: Explicit SQ found in a file with "
                              "TransferSyntax=Implicit for tag: "
                              << TagField);
        }
        catch (std::logic_error &)
        {
          // MR_ELSCINT1_00e1_1042_SQ_feff_00e0_Item.dcm
          std::streampos current = is.tellg();
          // could be bad, if the specific implementation does not support negative streamoff values.
          std::streamoff diff = start - current;
          is.seekg(diff, std::ios::cur);
          assert(diff == -14);
          ValueIO<ImplicitDataElement, SwapperDoOp>::Read(is, *ValueField, readvalues);
        }
        catch (std::exception &)
        {
          ValueLengthField = ValueField->GetLength();
        }
        return is;
      }
      else if (item == itemPMSStart2)
      {
        assert(0); // FIXME: Sync Read/ReadWithLength
        mdcmAlwaysWarnMacro("Illegal: SQ start with " << itemPMSStart2 << " instead of " << itemStart
                                                      << " for tag: " << TagField);
        ValueField = new SequenceOfItems;
        ValueField->SetLength(ValueLengthField);
        if (!ValueIO<ImplicitDataElement, TSwap>::Read(is, *ValueField, readvalues))
        {
          assert(0 && "Should not happen");
        }
        return is;
      }
#endif
      else
      {
        ValueField = new ByteValue;
      }
    }
  }
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
  // THE WORST BUG EVER. From GE Workstation.
  if (ValueLengthField == 13)
  {
    // Historically mdcm did not enforce proper length
    // thus Theralys started writing illegal DICOM images.
    const Tag theralys1(0x0008, 0x0070);
    const Tag theralys2(0x0008, 0x0080);
    if (TagField != theralys1 && TagField != theralys2)
    {
      mdcmAlwaysWarnMacro(
        "GE,13: Replacing VL=0x000d with VL=0x000a, for Tag=" << TagField << " in order to read a buggy DICOM file.");
      ValueLengthField = 10;
    }
  }
#endif
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
  if (ValueLengthField == 0x31f031c && TagField == Tag(0x031e, 0x0324))
  {
    // TestImages/elbow.pap
    mdcmAlwaysWarnMacro("Replacing a VL. To be able to read a supposively broken Payrus file.");
    ValueLengthField = 202; // 0xca
  }
#endif
  // We have the length we should be able to read the value
  ValueField->SetLength(ValueLengthField); // perform realloc
  bool failed;
#ifdef MDCM_WORDS_BIGENDIAN
  VR vrfield = GetVRFromTag(TagField);
  if (vrfield & VR::VRASCII || vrfield == VR::INVALID)
  {
    failed = !ValueIO<ImplicitDataElement, TSwap>::Read(is, *ValueField, readvalues);
  }
  else
  {
    assert(vrfield & VR::VRBINARY);
    unsigned int vrsize = vrfield.GetSize();
    assert(vrsize == 1 || vrsize == 2 || vrsize == 4 || vrsize == 8);
    if (vrfield == VR::AT)
      vrsize = 2;
    switch (vrsize)
    {
      case 1:
        failed = !ValueIO<ImplicitDataElement, TSwap, uint8_t>::Read(is, *ValueField, readvalues);
        break;
      case 2:
        failed = !ValueIO<ImplicitDataElement, TSwap, uint16_t>::Read(is, *ValueField, readvalues);
        break;
      case 4:
        failed = !ValueIO<ImplicitDataElement, TSwap, uint32_t>::Read(is, *ValueField, readvalues);
        break;
      case 8:
        failed = !ValueIO<ImplicitDataElement, TSwap, uint64_t>::Read(is, *ValueField, readvalues);
        break;
      default:
        failed = true;
        assert(0);
    }
  }
#else
  failed = !ValueIO<ImplicitDataElement, TSwap>::Read(is, *ValueField, readvalues);
#endif
  if (failed)
  {
    // Special handling for PixelData tag
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
    if (TagField == Tag(0x7fe0, 0x0010))
    {
      mdcmAlwaysWarnMacro("Incomplete Pixel Data found, use file at own risk");
      is.clear();
    }
    else
#endif
    {
      throw std::logic_error("Should not happen (imp)");
    }
    return is;
  }
#ifdef MDCM_SUPPORT_BROKEN_IMPLEMENTATION
  // dcmtk 3.5.4 is resilient to broken explicit SQ length and will properly recompute it
  // as long as each of the Item lengths are correct
  VL dummy = ValueField->GetLength();
  if (ValueLengthField != dummy)
  {
    mdcmAlwaysWarnMacro("ValueLengthField was bogus");
    ValueLengthField = dummy;
  }
#else
  assert(ValueLengthField == ValueField->GetLength());
  assert(VRField == VR::INVALID);
#endif
  return is;
}

template <typename TSwap>
const std::ostream &
ImplicitDataElement::Write(std::ostream & os) const
{
  // See PS 3.5, 7.1.3 Data Element Structure With Implicit VR
  // Write Tag
  if (!TagField.Write<TSwap>(os))
  {
    assert(0 && "Should not happen");
    return os;
  }
  // Write Value Length
  const SequenceOfItems * sqi = dynamic_cast<const SequenceOfItems *>(ValueField.GetPointer());
  if (sqi && !ValueLengthField.IsUndefined())
  {
    // Hum, we might have to recompute the length:
    // See TestWriter2, where an explicit SQ is converted to implicit SQ
    VL len = sqi->template ComputeLength<ImplicitDataElement>();
    // assert(len == ValueLengthField);
    if (!len.Write<TSwap>(os))
    {
      assert(0 && "Should not happen");
      return os;
    }
  }
  else // It should be safe to simply use the ValueLengthField as stored
  {
    // Do not allow writing file such as: dcm4che_UndefinedValueLengthInImplicitTS.dcm
    if (TagField == Tag(0x7fe0, 0x0010) && ValueLengthField.IsUndefined())
    {
      throw std::logic_error("VL u/f Impossible");
    }
    if (!ValueLengthField.Write<TSwap>(os))
    {
      assert(0 && "Should not happen");
      return os;
    }
  }
  // Write Value
  if (ValueLengthField)
  {
    assert(ValueField);
    mdcmAssertAlwaysMacro(ValueLengthField == ValueField->GetLength());
    assert(TagField != Tag(0xfffe, 0xe00d) && TagField != Tag(0xfffe, 0xe0dd));
    if (!ValueIO<ImplicitDataElement, TSwap>::Write(os, *ValueField))
    {
      assert(0 && "Should not happen");
      return os;
    }
  }
  return os;
}

} // end namespace mdcm


#endif // MDCMIMPLICITDATAELEMENT_TXX
