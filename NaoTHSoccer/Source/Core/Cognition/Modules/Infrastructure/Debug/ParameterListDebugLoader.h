/* 
 * File:   ParameterListDebugLoader.h
 * Author: thomas
 *
 */

#ifndef PARAMETERLISTDEBUGLOADER_H
#define	PARAMETERLISTDEBUGLOADER_H

#include <map>

#include <ModuleFramework/Module.h>
#include <DebugCommunication/DebugCommandExecutor.h>

#include <DebugCommunication/DebugServer.h>

#include <Representations/Infrastructure/CameraSettings.h>
#include "Representations/Infrastructure/FieldInfo.h"

#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/MCSLParameters.h"
#include "Cognition/Modules/Modeling/BallLocator/ParticleFilterBallLocator/PFBLParameters.h"
#include "Cognition/Modules/Modeling/GoalLocator/ActiveGoalLocator/AGLParameters.h"

#include "Representations/Infrastructure/ColorTable64.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(ParameterListDebugLoader)
  PROVIDE(CameraSettingsRequest)  
  PROVIDE(FieldInfo)
  PROVIDE(MCSLParameters)
  PROVIDE(PFBLParameters)

  PROVIDE(ColorTable64)
END_DECLARE_MODULE(ParameterListDebugLoader)

class ParameterListDebugLoader : public ParameterListDebugLoaderBase, public DebugCommandExecutor
{
public:
  ParameterListDebugLoader();
  virtual ~ParameterListDebugLoader();

  virtual void execute();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);

private:
  std::map<std::string, ParameterList*> paramlists;
};

#endif	/* PARAMETERLISTDEBUGLOADER_H */

