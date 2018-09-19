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
    Parameters() : ParameterList("WhistleDetectorParameters")
    {
      PARAMETER_REGISTER(whistleListFile)  = "whistles.lst";
      PARAMETER_REGISTER(activeChannels)   = "1010";
      PARAMETER_REGISTER(threshold)        = 0.25;
      PARAMETER_REGISTER(checkAllWhistles) = true;
      PARAMETER_REGISTER(saveRawAudio)     = false;
      PARAMETER_REGISTER(onOffSwitch)      = 0;
      syncWithConfig();
    }
    std::string whistleListFile;
    std::string activeChannels;
    double threshold;
    bool checkAllWhistles;
    bool saveRawAudio;
    int onOffSwitch;
  } params;

private:
};

#endif // _AUDIO_CONFIG_SETTER_H
