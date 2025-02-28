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
#ifndef MDCMSYSTEM_H
#define MDCMSYSTEM_H

#include "mdcmTypes.h"

namespace mdcm
{

class MDCM_EXPORT System
{
public:
  static bool
  FileExists(const char *);
  static bool
  FileIsDirectory(const char *);
  static bool
  FileIsSymlink(const char *);
  static time_t
  FileTime(const char *);
  static const char *
  GetLastSystemError();
  static std::wstring
  ConvertToUtf16(const char *);
  static size_t
  FileSize(const char *);
  static const char *
  GetCurrentProcessFileName(); // not thread safe
  // In the following the size '22' is explicitly listed. You need to pass in
  // at least 22bytes of array. If the string is an output it will be
  // automatically padded ( array[21] == 0 ) for you.
  // Those functions: GetCurrentDateTime / FormatDateTime / ParseDateTime do
  // not return the &YYZZ part of the DT structure as defined in DICOM PS 3.5 -
  // 2008 In this case it is simple to split the date[22] into a DA and TM
  // structure
  // Return the current data time, and format it as ASCII text.
  // This is simply a call to gettimeofday + FormatDateTime, since WIN32 do not have an
  // implementation for gettimeofday, this is more portable.
  // The call time(0) is not precise for our resolution
  static bool
  GetCurrentDateTime(char[22]);
  // format as ASCII text a time_t with milliseconds
  // See VR::DT from DICOM PS 3.5
  // milliseconds is in the range [0, 999999]
  static bool
  FormatDateTime(char[22], time_t, long = 0);
  // Parse a date stored as ASCII text into a time_t structured (discard millisecond if any)
  static bool
  ParseDateTime(time_t &, const char[22]);
  // Parse a date stored as ASCII text into a time_t structured and millisecond
  static bool
  ParseDateTime(time_t &, long &, const char[22]);
  // Return the value for Timezone Offset From UTC as string.
  static const char *
  GetTimezoneOffsetFromUTC();
  // Used internally by the UIDGenerator class to convert a uuid tape to a
  // DICOM VR:UI type
  static size_t
  EncodeBytes(char *, const unsigned char *, int);
  // consistent func for C99 spec of strcasecmp/strncasecmp
  static int
  StrCaseCmp(const char *, const char *);
  static int
  StrNCaseCmp(const char *, const char *, size_t); // n != 0
  // strtok_r
  static char *
  StrTokR(char *, const char *, char **);
  // strsep
  // param stringp is passed by pointer, it may be modified, you'll need to
  // make a copy, in case you want to free the memory pointed at
  static char *
  StrSep(char **, const char *);
};

} // end namespace mdcm

#endif // MDCMSYSTEM_H
