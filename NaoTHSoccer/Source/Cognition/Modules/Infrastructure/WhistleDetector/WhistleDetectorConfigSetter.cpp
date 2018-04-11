#include "WhistleDetectorConfigSetter.h"

WhistleDetectorConfigSetter::WhistleDetectorConfigSetter(){
	getDebugParameterList().add(&params);
}

WhistleDetectorConfigSetter::~WhistleDetectorConfigSetter()
{
	getDebugParameterList().remove(&params);
}

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

