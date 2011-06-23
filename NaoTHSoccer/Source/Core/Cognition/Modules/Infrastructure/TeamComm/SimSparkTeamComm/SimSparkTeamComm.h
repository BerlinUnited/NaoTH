/**
 * @file SimSparkTeamComm.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef SIMSPARK_TEAM_COMM_H
#define SIMSPARK_TEAM_COMM_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>

#include <Representations/Infrastructure/TeamMessageData.h>

#include "Representations/Modeling/TeamMessage.h"

BEGIN_DECLARE_MODULE(SimSparkTeamComm)
  REQUIRE(TeamMessageData)
  REQUIRE(FrameInfo)

  PROVIDE(TeamMessage)
  PROVIDE(RobotMessageData)
END_DECLARE_MODULE(SimSparkTeamComm)


class SimSparkTeamComm : public SimSparkTeamCommBase
{
public:
  SimSparkTeamComm();

  virtual ~SimSparkTeamComm();

  virtual void execute();

private:

  void readMessage();

  void sendMessage();
};

#endif // SIMSPARK_TEAM_COMM_H
