#ifndef _AUDIO_CONFIG_SETTER_H
#define _AUDIO_CONFIG_SETTER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/AudioControl.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>


using namespace naoth;

BEGIN_DECLARE_MODULE(AudioConfigSetter)
	PROVIDE(DebugParameterList)
	PROVIDE(AudioControl)
END_DECLARE_MODULE(AudioConfigSetter)


/**
 * Sets the Parameters for the whistledetector, also loads whistledetector Parameters from config.
 */


class AudioConfigSetter : public AudioConfigSetterBase
{
public:
	AudioConfigSetter();
	virtual ~AudioConfigSetter();

	virtual void execute();

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("AudioConfig")
    {
      PARAMETER_REGISTER(activeChannels)   = "1010";
      PARAMETER_REGISTER(onOffSwitch)      = 0;
      syncWithConfig();
    }
    std::string activeChannels;
    int onOffSwitch;
  } params;

private:

};

#endif // _AUDIO_CONFIG_SETTER_H
