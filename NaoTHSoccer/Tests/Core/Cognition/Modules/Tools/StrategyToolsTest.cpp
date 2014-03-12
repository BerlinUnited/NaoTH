#include <ModuleFramework/Module.h>
#include "ModuleFramework/ModuleManager.h"
#include "Tools/StrategyTools.h"
#include <gtest/gtest.h>

using namespace std;

class StrategyToolsTest : public ::testing::Test
{
protected:
  StrategyToolsTest(){}
  virtual void SetUp()
  {
  }


};

TEST_F(StrategyToolsTest, arrangeRobots)
{
  std::vector<Vector2d> setPiecePoints;
  //input of positions
  setPiecePoints.push_back(Vector2d(-1700.0, -1700.0));
  setPiecePoints.push_back(Vector2d(-2400.0,     0.0));
  setPiecePoints.push_back(Vector2d(-1700.0,  1700.0));

  std::vector<int> placesToRobots(3);

  // init
  for(unsigned int i = 0; i < placesToRobots.size(); i++)
  {
    placesToRobots[i] = i;
  }//end for

  std::vector<Vector2d > playersPoints;
  playersPoints.push_back(Vector2d(2400.0,      0.0));
  playersPoints.push_back(Vector2d(3400.0,    500.0));
  playersPoints.push_back(Vector2d(2800.0,   -700.0));
  playersPoints.push_back(Vector2d(-2500.0,     0.0));
  playersPoints.push_back(Vector2d(-1600.0,  1600.0));
  playersPoints.push_back(Vector2d(-1600.0, -1700.0));

  StrategyTools::arrangeRobots(playersPoints, setPiecePoints, placesToRobots);

  std::vector<int> expectedPlacesToRobots;
  expectedPlacesToRobots.push_back(5);
  expectedPlacesToRobots.push_back(3);
  expectedPlacesToRobots.push_back(4);

  ASSERT_EQ(expectedPlacesToRobots[0], placesToRobots[0]);
  ASSERT_EQ(expectedPlacesToRobots[1], placesToRobots[1]);
  ASSERT_EQ(expectedPlacesToRobots[2], placesToRobots[2]);
}//end testArrangeRobots