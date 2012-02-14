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
  getColorClassificationModel().setFieldColorPercept(getFieldColorPercept());
  getColorClassificationModel().setBaseColorRegionPercept(getBaseColorRegionPercept());
}


void ColorProvider::execute()
{
  if(getFieldColorPercept().lastUpdated.getFrameNumber() == getFrameInfo().getFrameNumber())
  {
    getColorClassificationModel().validateFieldColorPercept();
  }
  else
  {
    getColorClassificationModel().invalidateFieldColorPercept();
  }

  if(getBaseColorRegionPercept().lastUpdated.getFrameNumber() == getFrameInfo().getFrameNumber())
  {
    getColorClassificationModel().validateBaseColorRegionPercept();
  }
  else
  {
    getColorClassificationModel().invalidateBaseColorRegionPercept();
  }
}//end execute
