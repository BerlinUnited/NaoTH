/**
* @file ColorProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ColorProvider
*/

#include "ColorProvider.h"

// needed to load the colortable
#include <PlatformInterface/Platform.h>

using namespace std;

ColorProvider::ColorProvider()
{
  const string colorTablePath = naoth::Platform::getInstance().theConfigDirectory + "/colortable.c64";
  getColorTable64().loadFromFile(colorTablePath);
  getColorClassificationModel().setColorTable(getColorTable64());
  getColorClassificationModelTop().setColorTable(getColorTable64());
}


void ColorProvider::execute()
{
  //field color bottom image
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

  //field color top image
  const unsigned int fcFrameNrTop = getFieldColorPerceptTop().lastUpdated.getFrameNumber();
  if
  (
    fcFrameNrTop == getFrameInfo().getFrameNumber()
    || 
    fcFrameNrTop == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModelTop().setFieldColorPercept(getFieldColorPerceptTop());
  }
  else
  {
    getColorClassificationModelTop().invalidateFieldColorPercept();
  }

  lastFrameInfo = getFrameInfo();
}//end execute
