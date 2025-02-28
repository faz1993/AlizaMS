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
#ifndef MDCMPROGRESSEVENT_H
#define MDCMPROGRESSEVENT_H

#include "mdcmEvent.h"
#include "mdcmTag.h"

namespace mdcm
{

/**
 * ProgressEvent
 *
 */

class ProgressEvent : public AnyEvent
{
public:
  typedef ProgressEvent Self;
  typedef AnyEvent      Superclass;
  ProgressEvent(double p = 0)
    : m_Progress(p)
  {}
  ProgressEvent(const Self & s)
    : AnyEvent(s)
  {}
  ~ProgressEvent() {}

  const char *
  GetEventName() const override
  {
    return "ProgressEvent";
  }

  bool
  CheckEvent(const ::mdcm::Event * e) const override
  {
    return ((dynamic_cast<const Self *>(e)) ? true : false);
  }

  ::mdcm::Event *
  MakeObject() const override
  {
    return new Self;
  }

  void
  SetProgress(double p)
  {
    m_Progress = p;
  }

  double
  GetProgress() const
  {
    return m_Progress;
  }

private:
  void
         operator=(const Self &);
  double m_Progress;
};

} // end namespace mdcm

#endif // MDCMPROGRESSEVENT_H
