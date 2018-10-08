#ifndef FREEKICKMODULE_H
#define FREEKICKMODULE_H

#include "ModuleFramework/Module.h"


BEGIN_DECLARE_MODULE(FreeKickModule)
  //PROVIDE(DebugModify)
  //PROVIDE(DebugRequest)
  //PROVIDE(DebugDrawings)
  //PROVIDE(DebugParameterList)

  //REQUIRE(FrameInfo)
  //REQUIRE(TeamMessage)
  //REQUIRE(RobotPose)
  //REQUIRE(TeamMessageNTP)
  //REQUIRE(TeamMessagePlayersState)

  //PROVIDE(TeamBallModel)
END_DECLARE_MODULE(FreeKickModule)


class FreeKickModule : public FreeKickModuleBase
{
public:
    FreeKickModule() {}
    ~FreeKickModule() {}

    virtual void execute();
};

#endif // FREEKICKMODULE_H
