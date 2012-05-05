#include <ModuleFramework/Module.h>
#include "ModuleFramework/ModuleManager.h"
#include "Cognition/Modules/Modeling/PotentialFieldProvider/PotentialFieldProvider.h"
#include <gtest/gtest.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(PotentialFieldProviderTest)
PROVIDE(LocalGoalModel)
PROVIDE(SelfLocGoalModel)
PROVIDE(CompassDirection)
PROVIDE(BallModel)
PROVIDE(RawAttackDirection)
END_DECLARE_MODULE(PotentialFieldProviderTest)

class PotentialFieldProviderTest : public testing::Test,
  public PotentialFieldProviderTestBase, private ModuleManager
{
public:
  virtual void execute()  {}

  PotentialFieldProvider* potentialFieldProvider;

protected:
  virtual void SetUp()
  {
    ModuleCreator<PotentialFieldProvider>* m =
        registerModule<PotentialFieldProvider>("PotentialFieldProvider");
    m->setEnabled(true);
    potentialFieldProvider = (PotentialFieldProvider*) m->getModule();
  }

};

TEST_F(PotentialFieldProviderTest, StableOnMovedRobot)
{
  Vector2<double> simulatedBall(100.0,250.0);

  Vector2<double> simulatedLeftPost(500, 3000);
  Vector2<double> simulatedRightPost(-500, 3000);

  getCompassDirection().angle = 0;

  // deactivate the local model
  getLocalGoalModel().opponentGoalIsValid = false;

  getBallModel().positionPreview = simulatedBall;
  getSelfLocGoalModel().goal.leftPost = simulatedLeftPost;
  getSelfLocGoalModel().goal.rightPost = simulatedRightPost;

  potentialFieldProvider->execute();
  Vector2<double> result1 = getRawAttackDirection().attackDirection;

  ASSERT_NE(0, result1.x);
  ASSERT_NE(0, result1.y);

  // phase 1: only translation is changed

  // move robot (and ball/goal)
  Pose2D pose2(0.0, 0.0, 1000.0);

  getBallModel().positionPreview = pose2/simulatedBall;
  getSelfLocGoalModel().goal.leftPost = pose2/simulatedLeftPost;
  getSelfLocGoalModel().goal.leftPost = pose2/simulatedRightPost;

  potentialFieldProvider->execute();
  Vector2<double> result2 = getRawAttackDirection().attackDirection;

  ASSERT_NE(0, result2.x);
  ASSERT_NE(0, result2.y);
  ASSERT_EQ(result1.x, result2.x);
  ASSERT_EQ(result1.y, result2.y);
}
