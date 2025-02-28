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
#ifndef MDCMBITMAP_H
#define MDCMBITMAP_H

#include "mdcmObject.h"
#include "mdcmCurve.h"
#include "mdcmDataElement.h"
#include "mdcmLookupTable.h"
#include "mdcmOverlay.h"
#include "mdcmPhotometricInterpretation.h"
#include "mdcmPixelFormat.h"
#include "mdcmSmartPointer.h"
#include "mdcmTransferSyntax.h"
#include <vector>

namespace mdcm
{

/**
 * Bitmap class
 *
 * A bitmap based image. Used as parent for icon image and the main pixel image,
 * does not contains any world space information.
 */

class MDCM_EXPORT Bitmap : public Object
{
  friend class PixmapReader;
  friend class ImageChangeTransferSyntax;

public:
  Bitmap();
  ~Bitmap();
  unsigned int
  GetNumberOfDimensions() const;
  void
  SetNumberOfDimensions(unsigned int);
  const unsigned int *
  GetDimensions() const;
  unsigned int
  GetDimension(unsigned int) const;
  void
  SetDimensions(const unsigned int[3]);
  void
  SetDimension(unsigned int, unsigned int);
  unsigned int
  GetPlanarConfiguration() const;
  void
  SetPlanarConfiguration(unsigned int);
  void
  Clear();
  bool
  IsEmpty() const;
  const PhotometricInterpretation &
  GetPhotometricInterpretation() const;
  void
  SetPhotometricInterpretation(PhotometricInterpretation const &);
  bool
  IsLossy() const;
  void
  SetLossyFlag(bool);
  unsigned long long
  GetBufferLength() const;
  bool
  GetBuffer(char *) const;
  virtual bool
  AreOverlaysInPixelData() const;
  virtual bool
  UnusedBitsPresentInPixelData() const;
  bool
  GetNeedByteSwap() const;
  void
  SetNeedByteSwap(bool);
  void
  SetTransferSyntax(TransferSyntax const &);
  const TransferSyntax &
  GetTransferSyntax() const;
  bool
  IsTransferSyntaxCompatible(TransferSyntax const &) const;
  void
  SetDataElement(DataElement const &);
  const DataElement &
  GetDataElement() const;
  DataElement &
  GetDataElement();
  void
  SetLUT(LookupTable const &);
  const LookupTable &
  GetLUT() const;
  LookupTable &
  GetLUT();
  void
  SetColumns(unsigned int);
  unsigned int
  GetColumns() const;
  void
  SetRows(unsigned int);
  unsigned int
  GetRows() const;
  const PixelFormat &
  GetPixelFormat() const;
  PixelFormat &
  GetPixelFormat();
  void
  SetPixelFormat(PixelFormat const &);
  void
  Print(std::ostream &) const;

protected:
  bool
  ComputeLossyFlag();
  bool
  TryRAWCodec(char * buffer, bool & lossyflag) const;
  bool
  TryJPEGCodec(char * buffer, bool & lossyflag) const;
  bool
  TryJPEGCodec2(std::ostream & os) const;
  bool
  TryJPEGCodec3(char * buffer, bool & lossyflag) const;
  bool
  TryPVRGCodec(char * buffer, bool & lossyflag) const;
  bool
  TryJPEGLSCodec(char * buffer, bool & lossyflag) const;
  bool
  TryJPEGLSCodec2(char * buffer, bool & lossyflag) const;
  bool
  TryJPEG2000Codec(char * buffer, bool & lossyflag) const;
  bool
  TryJPEG2000Codec2(std::ostream & os) const;
  bool
  TryJPEG2000Codec3(char * buffer, bool & lossyflag) const;
  bool
  TryRLECodec(char * buffer, bool & lossyflag) const;
  unsigned int                      PlanarConfiguration;
  unsigned int                      NumberOfDimensions;
  TransferSyntax                    TS;
  PixelFormat                       PF;
  PhotometricInterpretation         PI;
  std::vector<unsigned int>         Dimensions;
  DataElement                       PixelData;
  typedef SmartPointer<LookupTable> LUTPtr;
  LUTPtr                            LUT;
  bool                              NeedByteSwap;
  bool                              LossyFlag;

private:
  bool
  GetBufferInternal(char *, bool &) const;
};

} // end namespace mdcm

#endif // MDCMBITMAP_H
