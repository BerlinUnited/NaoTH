#include "AudioConfigSetter.h"

AudioConfigSetter::AudioConfigSetter(){
	getDebugParameterList().add(&params);
}

AudioConfigSetter::~AudioConfigSetter()
{
	getDebugParameterList().remove(&params);
}

void AudioConfigSetter::execute()
{
  if(params.activeChannels != getAudioControl().activeChannels)
  {
	  getAudioControl().activeChannels = params.activeChannels;
  }
  if (params.capture != getAudioControl().capture)
  {
	  getAudioControl().capture = params.capture;
  }
}

