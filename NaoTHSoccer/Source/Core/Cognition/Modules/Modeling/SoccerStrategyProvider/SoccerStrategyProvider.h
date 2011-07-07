/**
* @file SoccerStrategyProvider.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* provides soccer strategy
*/

#ifndef __SoccerStrategyProvider_h_
#define __SoccerStrategyProvider_h_

#include <ModuleFramework/Module.h>

#include "Tools/Math/Vector3.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/PlayersModel.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SoccerStrategyProvider)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  REQUIRE(PlayerInfo)
  REQUIRE(BodyState)
  REQUIRE(PlayersModel)

  PROVIDE(SoccerStrategy)
END_DECLARE_MODULE(SoccerStrategyProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class SoccerStrategyProvider : public SoccerStrategyProviderBase
{
public:

  class FormationParameters
  {
  public:
    FormationParameters();

    FormationParameters(unsigned int playerNumber);

    ~FormationParameters(){}

    unsigned int num;
    Vector2<double> home;
    Vector3<double> attr;
    Vector2<double> min;
    Vector2<double> max;
    bool behindBall;
    bool penaltyAreaAllowed;
  };

  SoccerStrategyProvider();
  ~SoccerStrategyProvider(){}

  /** executes the module */
  virtual void execute();

protected:
  Vector2<double> calculateForamtion() const;
  
  double estimateTimeToBall() const;

  bool isSomeoneBetweenBallAndMe() const;

private:
  FormationParameters theFormationParameters;
};

#endif //__SoccerStrategyProvider_h_
