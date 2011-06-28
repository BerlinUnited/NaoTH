/**
 * @file SimSparkTeamComm.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * @brief team communicator via 'say' and 'hear' in SimSpark
 * M essage may consist of 20 characters, which may be taken from the ASCII printing character
 * subset [0x20, 0x7E] except the white space character and the normal brackets ( and ).
 */

#ifndef SIMSPARK_TEAM_COMM_H
#define SIMSPARK_TEAM_COMM_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>
#include <Tools/Communication/ASCIIEncoder.h>

#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/FieldInfo.h>

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/PlayerInfo.h"

BEGIN_DECLARE_MODULE(SimSparkTeamComm)
  REQUIRE(TeamMessageDataIn)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(PlayerInfo)

  PROVIDE(TeamMessage)
  PROVIDE(TeamMessageDataOut)
END_DECLARE_MODULE(SimSparkTeamComm)


class SimSparkTeamComm : public SimSparkTeamCommBase
{
public:
  SimSparkTeamComm();

  virtual ~SimSparkTeamComm();

  virtual void execute();

private:
  ASCIIEncoder encoder;

  void readMessage();

  void sendMessage();
};

#endif // SIMSPARK_TEAM_COMM_H
