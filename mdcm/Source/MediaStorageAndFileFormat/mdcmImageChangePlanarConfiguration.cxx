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

#include "mdcmImageChangePlanarConfiguration.h"
#include "mdcmSequenceOfFragments.h"
#include "mdcmSequenceOfItems.h"
#include "mdcmFragment.h"

namespace mdcm
{
/*
 * C.7.6.3.1.3 Planar Configuration
 * Note: Planar Configuration (0028,0006) is not meaningful when a compression transfer syntax is
 * used that involves reorganization of sample components in the compressed bit stream. In such
 * cases, since the Attribute is required to be sent, then an appropriate value to use may be
 * specified in the description of the Transfer Syntax in PS 3.5, though in all likelihood the value of
 * the Attribute will be ignored by the receiving implementation.
 */

bool
ImageChangePlanarConfiguration::Change()
{
  if (!(PlanarConfiguration == 0 || PlanarConfiguration == 1))
  {
    return false;
  }
  Output = Input;
  if (Input->GetPixelFormat().GetSamplesPerPixel() != 3)
  {
    return true;
  }
  if (Input->GetPlanarConfiguration() == PlanarConfiguration)
  {
    return true;
  }
  const Bitmap &       image = *Input;
  const unsigned int * dims = image.GetDimensions();
  unsigned long long   len = image.GetBufferLength();
  if (len > 0xffffffff)
  {
    mdcmAlwaysWarnMacro("ImageChangePlanarConfiguration::Change(): can not set length " << len);
    return false;
  }
  char * p;
  try
  {
    p = new char[len];
  }
  catch (const std::bad_alloc &)
  {
    return false;
  }
  image.GetBuffer(p);
  assert(len % 3 == 0);
  PixelFormat  pf = Input->GetPixelFormat();
  const size_t ps = pf.GetPixelSize();
  const size_t framesize = dims[0] * dims[1] * ps;
  assert(framesize * dims[2] == len);
  char * copy;
  try
  {
    copy = new char[len];
  }
  catch (const std::bad_alloc &)
  {
    return false;
  }
  size_t size = framesize / 3;
  if (PlanarConfiguration == 0)
  {
    for (unsigned int z = 0; z < dims[2]; ++z)
    {
      const char * frame = p + z * framesize;
      const char * r = frame + 0;
      const char * g = frame + size;
      const char * b = frame + size + size;
      char *       framecopy = copy + z * framesize;
      if (pf.GetBitsAllocated() == 16)
      {
        ImageChangePlanarConfiguration::RGBPlanesToRGBPixels<uint16_t>(
          (uint16_t *)framecopy, (const uint16_t *)r, (const uint16_t *)g, (const uint16_t *)b, size / 2);
      }
      else if (pf.GetBitsAllocated() == 8)
      {
        ImageChangePlanarConfiguration::RGBPlanesToRGBPixels(
          (uint8_t *)framecopy, (const uint8_t *)r, (const uint8_t *)g, (const uint8_t *)b, size);
      }
    }
  }
  else if (PlanarConfiguration == 1)
  {
    for (unsigned int z = 0; z < dims[2]; ++z)
    {
      const char * frame = p + z * framesize;
      char *       framecopy = copy + z * framesize;
      char *       r = framecopy + 0;
      char *       g = framecopy + size;
      char *       b = framecopy + size + size;
      if (pf.GetBitsAllocated() == 16)
      {
        ImageChangePlanarConfiguration::RGBPixelsToRGBPlanes<uint16_t>(
          (uint16_t *)r, (uint16_t *)g, (uint16_t *)b, (const uint16_t *)frame, size / 2);
      }
      else if (pf.GetBitsAllocated() == 8)
      {
        ImageChangePlanarConfiguration::RGBPixelsToRGBPlanes(
          (uint8_t *)r, (uint8_t *)g, (uint8_t *)b, (const uint8_t *)frame, size);
      }
    }
  }
  delete[] p;
  DataElement & de = Output->GetDataElement();
  de.SetByteValue(copy, (uint32_t)len);
  delete[] copy;
  Output->SetPlanarConfiguration(PlanarConfiguration);
  if (Input->GetTransferSyntax().IsImplicit())
  {
    assert(Output->GetTransferSyntax().IsImplicit());
  }
  else if (Input->GetTransferSyntax() == TransferSyntax::ExplicitVRBigEndian)
  {
    Output->SetTransferSyntax(TransferSyntax::ExplicitVRBigEndian);
  }
  else
  {
    Output->SetTransferSyntax(TransferSyntax::ExplicitVRLittleEndian);
  }
  return true;
}

} // end namespace mdcm
