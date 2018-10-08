#ifndef FREEKICKMODULE_H
#define FREEKICKMODULE_H

#include "ModuleFramework/Module.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamMessage.h"

BEGIN_DECLARE_MODULE(FreeKickModule)
  //PROVIDE(DebugModify)
  //PROVIDE(DebugRequest)
  //PROVIDE(DebugDrawings)
  //PROVIDE(DebugParameterList)

  //REQUIRE(FrameInfo)
  REQUIRE(GameData)
  REQUIRE(PlayerInfo)
  //REQUIRE(RobotPose)
  REQUIRE(TeamMessage)
  //REQUIRE(TeamMessagePlayersState)

  //PROVIDE(TeamBallModel)
END_DECLARE_MODULE(FreeKickModule)


class FreeKickModule : public FreeKickModuleBase
{
public:
    FreeKickModule() {}
    ~FreeKickModule() {}

    virtual void execute();

private:
    GameData::SetPlay lastSetPlay = GameData::set_none;
};

#endif // FREEKICKMODULE_H
