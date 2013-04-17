/**
* @file ColorProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ColorProvider
*/

#include "ColorProvider.h"

// needed to load the colortable
#include <PlatformInterface/Platform.h>

ColorProvider::ColorProvider()
{
  const string colorTablePath = naoth::Platform::getInstance().theConfigDirectory + "/colortable.c64";
  getColorTable64().loadFromFile(colorTablePath);
  getColorClassificationModel().setColorTable(getColorTable64());
}


void ColorProvider::execute()
{
  const unsigned int fcFrameNr = getFieldColorPercept().lastUpdated.getFrameNumber();
  if
  (
    fcFrameNr == getFrameInfo().getFrameNumber()
    || 
    fcFrameNr == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModel().setFieldColorPercept(getFieldColorPercept());
  }
  else
  {
    getColorClassificationModel().invalidateFieldColorPercept();
  }

  const unsigned int bcFrameNr = getBaseColorRegionPercept().lastUpdated.getFrameNumber();
  if
  (
    bcFrameNr == getFrameInfo().getFrameNumber()
    || 
    bcFrameNr == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModel().setBaseColorRegionPercept(getBaseColorRegionPercept());
  }
  else
  {
    getColorClassificationModel().invalidateBaseColorRegionPercept();
  }

  const unsigned int sgcFrameNr = getSimpleGoalColorPercept().lastUpdated.getFrameNumber();
  if
  (
    sgcFrameNr == getFrameInfo().getFrameNumber()
    || 
    sgcFrameNr == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModel().setSimpleGoalColorPercept(getSimpleGoalColorPercept());
  }
  else
  {
    getColorClassificationModel().invalidateSimpleGoalColorPercept();
  }

  const unsigned int sbcFrameNr = getSimpleBallColorPercept().lastUpdated.getFrameNumber();
  if
  (
    sbcFrameNr == getFrameInfo().getFrameNumber()
    || 
    sbcFrameNr == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModel().setSimpleBallColorPercept(getSimpleBallColorPercept());
  }
  else
  {
    getColorClassificationModel().invalidateSimpleBallColorPercept();
  }

  lastFrameInfo = getFrameInfo();
}//end execute
