/**
 * @file: TeamSymbols.h
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First created on 9. April 2009, 18:10
 */

#ifndef _TEAMSYMBOLS_H
#define _TEAMSYMBOLS_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/TeamMessagePlayerIsAlive.h"

BEGIN_DECLARE_MODULE(TeamSymbols)
  REQUIRE(TeamMessagePlayerIsAlive)
  PROVIDE(PlayerInfo)
  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(TeamSymbols)

class TeamSymbols: public TeamSymbolsBase
{
public:

  TeamSymbols()
  {
    theInstance = this;
  }

  virtual ~TeamSymbols() {}

  virtual void execute() {}

  void registerSymbols(xabsl::Engine& engine);

private:
  static TeamSymbols* theInstance;
  static double getTeamMembersAliveCount();
  static bool calculateIfStriker();
  static bool calculateIfSecondStriker();
  static bool getWasStriker();
  static void setWasStriker(bool striker);
};

#endif  /* _TEAMSYMBOLS_H */
