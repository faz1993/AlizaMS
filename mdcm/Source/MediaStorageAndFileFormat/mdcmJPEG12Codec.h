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
#ifndef MDCMJPEG12CODEC_H
#define MDCMJPEG12CODEC_H

#include "mdcmJPEGCodec.h"

namespace mdcm
{

class JPEGInternals_12BIT;
class ByteValue;

/**
 * Class for JPEG 12 bits (lossy & lossless)
 */
class JPEG12Codec : public JPEGCodec
{
public:
  JPEG12Codec();
  ~JPEG12Codec() override;
  bool
  DecodeByStreams(std::istream &, std::ostream &) override;
  bool
  InternalCode(const char *, size_t, std::ostream &) override;
  bool
  GetHeaderInfo(std::istream &, TransferSyntax &) override;

protected:
  bool
  IsStateSuspension() const override;
  bool
  EncodeBuffer(std::ostream &, const char *, size_t) override;

private:
  JPEGInternals_12BIT * Internals;
};

} // end namespace mdcm

#endif // MDCMJPEG12CODEC_H
