#ifndef WHISTLEDETECTORCONFIGSETTER_H
#define WHISTLEDETECTORCONFIGSETTER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/WhistleControl.h>
#include <Tools/DataStructures/ParameterList.h>


using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorConfigSetter)
  PROVIDE(WhistleControl)
END_DECLARE_MODULE(WhistleDetectorConfigSetter)


/**
 * Sets the Parameters for the whistledetector, also loads whistledetector Parameters from config.
 */
class WhistleDetectorConfigSetter : public WhistleDetectorConfigSetterBase
{
public:
  class Parameters: public ParameterList
  {
  public:
	
    Parameters() : ParameterList("WhistleDetectorParameters")
    {
      PARAMETER_REGISTER(whistleListFile) = "whistles.lst";
      PARAMETER_REGISTER(activeChannels) = "1010";
      syncWithConfig();
    }
    std::string whistleListFile;
    std::string activeChannels;
  } params;

  WhistleDetectorConfigSetter(){}
  virtual ~WhistleDetectorConfigSetter(){}

  virtual void execute();
private:
};

#endif // WHISTLEDETECTORCONFIGSETTER_H
