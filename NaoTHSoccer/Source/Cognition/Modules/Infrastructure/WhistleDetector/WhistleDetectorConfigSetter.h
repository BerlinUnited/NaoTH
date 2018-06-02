#ifndef WHISTLEDETECTORCONFIGSETTER_H
#define WHISTLEDETECTORCONFIGSETTER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/WhistleControl.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>


using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorConfigSetter)
	PROVIDE(DebugParameterList)
  PROVIDE(WhistleControl)
END_DECLARE_MODULE(WhistleDetectorConfigSetter)


/**
 * Sets the Parameters for the whistledetector, also loads whistledetector Parameters from config.
 */


class WhistleDetectorConfigSetter : public WhistleDetectorConfigSetterBase
{
public:
	WhistleDetectorConfigSetter();
	virtual ~WhistleDetectorConfigSetter();

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
      PARAMETER_REGISTER(saveRawAudio)     = true;
      syncWithConfig();
    }
    std::string whistleListFile;
    std::string activeChannels;
    double threshold;
    bool checkAllWhistles;
    bool saveRawAudio;
  } params;

private:
};

#endif // WHISTLEDETECTORCONFIGSETTER_H
