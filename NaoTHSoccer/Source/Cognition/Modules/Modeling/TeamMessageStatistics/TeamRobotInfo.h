#ifndef TEAMROBOTINFO_H
#define TEAMROBOTINFO_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Modeling/PlayerInfo.h"

#include "Representations/Modeling/TeamMessage.h"

// Representations
/*
#include "Representations/Infrastructure/CameraInfo.h"
*/

BEGIN_DECLARE_MODULE(TeamRobotInfo)
    REQUIRE(FrameInfo)
    REQUIRE(PlayerInfo)
    REQUIRE(TeamMessage)
END_DECLARE_MODULE(TeamRobotInfo)

class TeamRobotInfo : public TeamRobotInfoBase
{
public:
    TeamRobotInfo();
    ~TeamRobotInfo();
    virtual void execute();
};


#endif // TEAMROBOTINFO_H
