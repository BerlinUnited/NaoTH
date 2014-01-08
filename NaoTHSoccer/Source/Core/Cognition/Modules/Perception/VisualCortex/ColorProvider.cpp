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

  //base color bottom image
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

  //base color top image
  const unsigned int bcFrameNrTop = getBaseColorRegionPerceptTop().lastUpdated.getFrameNumber();
  if
  (
    bcFrameNrTop == getFrameInfo().getFrameNumber()
    || 
    bcFrameNrTop == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModelTop().setBaseColorRegionPercept(getBaseColorRegionPerceptTop());
  }
  else
  {
    getColorClassificationModelTop().invalidateBaseColorRegionPercept();
  }

  //simple goal color bottom image
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

  //simple goal color top image
  const unsigned int sgcFrameNrTop = getSimpleGoalColorPerceptTop().lastUpdated.getFrameNumber();
  if
  (
    sgcFrameNrTop == getFrameInfo().getFrameNumber()
    || 
    sgcFrameNrTop == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModelTop().setSimpleGoalColorPercept(getSimpleGoalColorPerceptTop());
  }
  else
  {
    getColorClassificationModelTop().invalidateSimpleGoalColorPercept();
  }

  //simple ball color bottom image
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

  //simple ball color top image
  const unsigned int sbcFrameNrTop = getSimpleBallColorPerceptTop().lastUpdated.getFrameNumber();
  if
  (
    sbcFrameNrTop == getFrameInfo().getFrameNumber()
    || 
    sbcFrameNrTop == lastFrameInfo.getFrameNumber()
  )
  {
    getColorClassificationModelTop().setSimpleBallColorPercept(getSimpleBallColorPerceptTop());
  }
  else
  {
    getColorClassificationModelTop().invalidateSimpleBallColorPercept();
  }

  lastFrameInfo = getFrameInfo();
}//end execute
