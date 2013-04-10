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
PROVIDE(FrameInfo)
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
  getFrameInfo().setTime(5000);

  Vector2<double> simulatedBall(820.0,250.0);

  Vector2<double> simulatedLeftPost(3000, -250);
  Vector2<double> simulatedRightPost(3000, 250);

  getCompassDirection().angle = 0;

  // deactivate the local model
  getLocalGoalModel().opponentGoalIsValid = false;

  // the robot doesn't move
  getBallModel().position = simulatedBall;
  getBallModel().positionPreview = simulatedBall;

  // 
  getSelfLocGoalModel().goal.leftPost = simulatedLeftPost;
  getSelfLocGoalModel().goal.rightPost = simulatedRightPost;

  potentialFieldProvider->execute();
  Vector2<double> result1 = getRawAttackDirection().attackDirection;

  // should not be the trivial case
  ASSERT_NE(0.0, result1.x);
  ASSERT_NE(0.0, result1.y);

  // phase 1: only translation is changed

  // move robot (and ball/goal)
  Pose2D pose2(0.0, 0.0, 1000.0);

  getBallModel().positionPreview = pose2*simulatedBall;
  getSelfLocGoalModel().goal.leftPost = pose2*simulatedLeftPost;
  getSelfLocGoalModel().goal.leftPost = pose2*simulatedRightPost;

  potentialFieldProvider->execute();
  Vector2<double> result2 = getRawAttackDirection().attackDirection;

  // should not be the trivial case
  ASSERT_NE(0.0, result2.x);
  ASSERT_NE(0.0, result2.y);

  // global angle should not be changed
  ASSERT_NEAR((pose2*result1).angle(), (pose2*result2).angle(), 0.001);
}
