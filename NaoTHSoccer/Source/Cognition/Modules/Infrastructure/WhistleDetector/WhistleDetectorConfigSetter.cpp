#include "WhistleDetectorConfigSetter.h"

void WhistleDetectorConfigSetter::execute()
{
  if(params.whistleListFile != getWhistleControl().whistleListFile)
  {
    getWhistleControl().whistleListFile = params.whistleListFile;
  }
  if(params.activeChannels != getWhistleControl().activeChannels)
  {
    getWhistleControl().activeChannels = params.activeChannels;
  }
}

