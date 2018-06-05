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

  if(params.threshold != getWhistleControl().threshold)
  {
    getWhistleControl().threshold = params.threshold;
  }
  
  if(params.checkAllWhistles != getWhistleControl().checkAllWhistles)
  {
    getWhistleControl().checkAllWhistles = params.checkAllWhistles;
  }
  
  if(params.saveRawAudio != getWhistleControl().saveRawAudio)
  {
    getWhistleControl().saveRawAudio = params.saveRawAudio;
  }
}

