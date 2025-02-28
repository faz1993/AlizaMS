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
#include "mdcmCAPICryptoFactory.h"
#include "mdcmCAPICryptographicMessageSyntax.h"

namespace mdcm
{

CAPICryptoFactory::CAPICryptoFactory(CryptoLib id)
  : CryptoFactory(id)
{
  mdcmDebugMacro("CAPI Factory registered." << std::endl);
}

CryptographicMessageSyntax *
CAPICryptoFactory::CreateCMSProvider()
{
  CAPICryptographicMessageSyntax * capicms = new CAPICryptographicMessageSyntax();
  if (capicms && !capicms->GetInitialized())
  {
    delete capicms;
    return NULL;
  }
  return capicms;
}

} // end namespace mdcm
