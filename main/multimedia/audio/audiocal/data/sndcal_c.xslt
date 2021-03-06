<?xml version="1.0" encoding="utf-16"?>

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text" encoding="us-ascii"/>
  <xsl:template match="/CALIBRATION_DATABASE">
<![CDATA[
/** title("Dual Mode Subscriber Station")
** subtitle("Audio Calibration Database")
*/ 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S O U N D    C A L I B R A T I O N    D A T A B A S E
               D A T A   S T O R A G E  A N D   R E T R I E V A L
                       
DESCRIPTION
  The actual calibration values and the functions to gain access to the
  data.  This version is based on constant ROM data, it may be replaced
  by a file-system based version.
  ***NOTE: Please do not modify the content of this always autogenerated file***

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  snd_cal_get_audio_control
    Get a pointer to an audio control structure.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"          /* Customer Specific Definitions              */
#include "target.h"            /* Target specific definitions                */
#include "msg.h"               /* Message logging macro                      */
#include "err.h"               /* Error logging macro                        */
#include "comdef.h"            /* Definitions for byte, word, etc.           */
#include "sndcal.h"            /* Definitions for audio calibration database */
#include "snddev.h"            /* Definitions for audio devices and methods  */

/* ===========================================================================
**
**                M A I N  C O N F I G U R A T I O N   A N D
**                     C A L I B R A T I O N   D A T A
**  
** =========================================================================*/
/***************************************************************************
*** HANDSET VOLUME TABLES
****************************************************************************/
]]>
<xsl:apply-templates select="CALIBRATION/DATAUNITS[@filename='snd_cal']"/>
<xsl:apply-templates select="CALIBRATION/CAL_DB[@filename='snd_cal']"/>
<![CDATA[
/* ===========================================================================
**
**                    A C C E S S   F U N C T I O N S
**  
** =========================================================================*/
/*===========================================================================

FUNCTION snd_cal_get_audio_control

DESCRIPTION
  Get the pointer to the audio calibration data according to this 
  [device, method]
  
DEPENDENCIES
  None.

RETURN VALUE
  SND_CAL_SUCCESS - data retrieved successfully
  SND_CAL_FAILED  - could not get data

SIDE EFFECTS
  None.

===========================================================================*/
snd_cal_return_type  snd_cal_get_audio_control (
  snd_device_type        device,
  snd_method_type        method,
  snd_cal_control_type   **audio_config
)
{
  if ((device < SND_DEVICE_MAX) && (method < SND_METHOD_MAX)) {
    *audio_config = (snd_cal_control_type *) 
                                    snd_cal_control_data[device][method];
  } else {
    ERR_FATAL("Illegal [device, method] option",0,0,0);
  }
  return (SND_CAL_SUCCESS);
} /* snd_cal_get_audio_control */
]]>
</xsl:template>

  <!-- DATAUNITS Template -->
  <xsl:template match="CALIBRATION/CAL_DB[@filename='snd_cal']">
    <xsl:text>&#xa;</xsl:text>
    <xsl:value-of select="@typequalifier"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@datatype"/>
    <xsl:if test="@vartype = string('PTR')">
      <xsl:text>* </xsl:text>
    </xsl:if>
    <xsl:text>&#xa;&#x9;</xsl:text>
    <xsl:value-of select="@name"/>
    <!--DB is always 2D ARRAY-->
    <xsl:for-each select="INDICES/INDEX">
      <xsl:text>[</xsl:text>
      <xsl:value-of select="@value"/>
      <xsl:text>] </xsl:text>
    </xsl:for-each>
    <xsl:text>= { &#xa;</xsl:text>
    <xsl:call-template name="process_data"/>
    <xsl:text> &#xa;};&#xa;</xsl:text>
  </xsl:template>

  <!-- DATAUNITS Template -->
  <xsl:template match="CALIBRATION/DATAUNITS[@filename='snd_cal']">
    <xsl:call-template name="process_data"/>
  </xsl:template>

  <xsl:include href="src_tmpl.xslt"/>
  
</xsl:stylesheet>
