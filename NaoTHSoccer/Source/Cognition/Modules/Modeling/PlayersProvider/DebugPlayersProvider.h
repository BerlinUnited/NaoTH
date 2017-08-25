/**
* @file DebugPlayersProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* provides soccer strategy
*/

#ifndef _DebugPlayersProvider_H_
#define _DebugPlayersProvider_H_

#include <ModuleFramework/Module.h>

// Representations
#include <Representations/Infrastructure/OptiTrackData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/RobotPose.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(DebugPlayersProvider)

  REQUIRE(FrameInfo)
  REQUIRE(OptiTrackData)
  REQUIRE(RobotPose)

  PROVIDE(PlayersModel)
END_DECLARE_MODULE(DebugPlayersProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class DebugPlayersProvider : public DebugPlayersProviderBase
{
public:
  DebugPlayersProvider();
  virtual ~DebugPlayersProvider(){}

  virtual void execute();
  
private:
  void trackableToPlayer(const Pose3D& trackable, PlayersModel::Player& player) const;
};

#endif //_DebugPlayersProvider_H_
