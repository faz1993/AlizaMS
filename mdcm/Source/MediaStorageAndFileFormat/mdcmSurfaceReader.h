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
#ifndef MDCMSURFACEREADER_H
#define MDCMSURFACEREADER_H

#include <mdcmSegmentReader.h>
#include <mdcmSurface.h>

namespace mdcm
{

/**
 * This class defines a SURFACE IE reader.
 * It reads surface mesh module attributes.
 *
 * PS 3.3 A.1.2.18 , A.57 and C.27
 */
class MDCM_EXPORT SurfaceReader : public SegmentReader
{
public:
  SurfaceReader();
  virtual ~SurfaceReader();
  virtual bool
  Read();
  unsigned long
  GetNumberOfSurfaces() const;

protected:
  bool
  ReadSurfaces();
  bool
  ReadSurface(const Item &, const unsigned long);
  bool
  ReadPointMacro(SmartPointer<Surface>, const DataSet &);
};

} // namespace mdcm

#endif // MDCMSURFACEREADER_H
