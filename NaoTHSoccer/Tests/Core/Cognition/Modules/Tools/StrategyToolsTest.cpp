#include <ModuleFramework/Module.h>
#include "ModuleFramework/ModuleManager.h"
#include "Tools/StrategyTools.h"
#include <gtest/gtest.h>

using namespace std;

class StrategyToolsTest : public ::testing::Test
{
public:
	StrategyTools* strategyTools;

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
  

  strategyTools->arrangeRobots(playersPoints, setPiecePoints, placesToRobots);

  std::vector<int> expectedPlacesToRobots;
  expectedPlacesToRobots.push_back(0);
  expectedPlacesToRobots.push_back(1);
  expectedPlacesToRobots.push_back(2);

  for(unsigned int i = 0; i < placesToRobots.size(); i++)
  {
    if(placesToRobots[i] != expectedPlacesToRobots[1])
    {
      cout << "Assignement Wrong" << endl;
    }
  }//end for
}//end testArrangeRobots