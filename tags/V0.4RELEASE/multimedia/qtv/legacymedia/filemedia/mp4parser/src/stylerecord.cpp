/* =======================================================================
                              stylerecord.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/stylerecord.cpp#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "stylerecord.h"
#include "atomutils.h"
#include "isucceedfail.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

/* ======================================================================
FUNCTION 
  StyleRecord:: StyleRecord

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
StyleRecord:: StyleRecord (OSCL_FILE *fp) 
{
  _success = true;
  _fileErrorCode = (int32)EVERYTHING_FINE;

  _pRGBA = (uint8*)QTV_Malloc(4);
  if ( !_pRGBA )
  {
    _success = false;
    _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
    return;
  }

  if ( !AtomUtils::read16( fp, _startChar) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read16( fp, _endChar) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read16( fp, _fontID) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read8( fp, _fontStyleFlags) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read8( fp, _fontSize) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::readByteData(fp, 4, _pRGBA) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }
}

/* ======================================================================
FUNCTION 
  StyleRecord:: StyleRecord

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
StyleRecord:: StyleRecord (uint8 *&buf) 
{
  _success = true;
  _fileErrorCode = (int32)EVERYTHING_FINE;

  _pRGBA = (uint8*)QTV_Malloc(4);
  if ( !_pRGBA )
  {
    _success = false;
    _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
    return;
  }

  if ( !AtomUtils::read16( buf, _startChar) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read16( buf, _endChar) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read16( buf, _fontID) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read8( buf, _fontStyleFlags) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::read8( buf, _fontSize) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }

  if ( !AtomUtils::readByteData(buf, 4, _pRGBA) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_FAILED;
  }
}

/* ======================================================================
FUNCTION 
  StyleRecord::~StyleRecord

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
StyleRecord::~StyleRecord()
{
  if ( _pRGBA != NULL )
  {
    QTV_Free(_pRGBA);
  }
}

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
