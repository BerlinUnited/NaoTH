/**
 * @file UltraSoundObstacleLocator.h
 *
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 * Declaration of class ObstacleLocator
 */

#ifndef _UltraSoundObstacleLocator_h_
#define _UltraSoundObstacleLocator_h_

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Cognition/Cognition.h"


#include "Tools/Math/Geometry.h"


// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/NaoTHAssert.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(UltraSoundObstacleLocator)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(UltraSoundReceiveData)

  PROVIDE(UltraSoundSendData)
  PROVIDE(LocalObstacleModel)
END_DECLARE_MODULE(UltraSoundObstacleLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class UltraSoundObstacleLocator : private UltraSoundObstacleLocatorBase
{
public:
  UltraSoundObstacleLocator();

  ~UltraSoundObstacleLocator()
  {
  };

  // Dimensions of the grid
  enum {CELL_SIZE = 60, CELLS_X = 40, CELLS_Y = 40};

  class Cell
  {
    public:
      unsigned int lastUpdate;                  // The point of time of the last change of the state
      enum State {FREE = 0, MAX_VALUE = 3};     // Minimum and maximum value of state
      int state;                                // The state of the cell, i.e. the number of positive measurements within the last few frames

      // Constructor
      Cell():lastUpdate(0),state(FREE) {}

      void update(unsigned int timestemp, int iStateDifference)
      {
        lastUpdate = timestemp;
        state += iStateDifference;
        if(state < FREE)
          state = FREE;
        if(state > MAX_VALUE)
          state = MAX_VALUE;
      }
  
      void DEBUGupdate(unsigned int timestemp, int iStateDifference, int x, int y)
      {
        lastUpdate = timestemp;
        state += iStateDifference;
        if(state < FREE)
          state = FREE;
        if(state > MAX_VALUE)
          state = MAX_VALUE;
        DOUT ("Update(" << x << "," << y << ") to: " << state << " time: " << lastUpdate << "\n");
      }
  };

  class Grid
  {
    private:
      Cell grid[CELLS_X][CELLS_Y]; // the grid itself, dimensions = 40 * 40, (60mm)
      unsigned int ageThreshold; /* max age of the considered data */
      FrameInfo theFrameInfo;

    public:
      // Constructor
      Grid(){
        for(int x=0; x<CELLS_X; x++)
          for(int y=0; y<CELLS_Y; y++){
            grid[x][y].state=Cell::FREE;
            grid[x][y].lastUpdate = 0;
          }
        //ageThreshold = _ageThreshold;
        //theFrameInfo = _theFrameInfo;
      }

      // init
      void init(unsigned int _ageThreshold, FrameInfo _theFrameInfo){
        for(int x=0; x<CELLS_X; x++)
          for(int y=0; y<CELLS_Y; y++){
            grid[x][y].state=Cell::FREE;
            grid[x][y].lastUpdate = 0;
          }
        ageThreshold = _ageThreshold;
        theFrameInfo = _theFrameInfo;
      }
      //setter/getter
      void setGrid(int x, int y, Cell value){
        ASSERT(x>=0);
        ASSERT(x<CELLS_X);
        ASSERT(y>=0);
        ASSERT(y>=CELLS_Y);
        if(x<0)
          x=0;
        if(x>=CELLS_X)
          x=CELLS_X-1;
        if(y<0)
          y=0;
        if(y>=CELLS_Y)
          y=CELLS_Y-1;
        grid[x][y] = value;
      }

      void setState(int x, int y, int value){
        ASSERT(x>=0);
        ASSERT(x<CELLS_X);
        ASSERT(y>=0);
        ASSERT(y>=CELLS_Y);
        if(x<0)
          x=0;
        if(x>=CELLS_X)
          x=CELLS_X-1;
        if(y<0)
          y=0;
        if(y>=CELLS_Y)
          y=CELLS_Y-1;
        grid[x][y].state = value;
      }

      void setLastUpdate(int x, int y, unsigned int value){
        ASSERT(x>=0);
        ASSERT(x<CELLS_X);
        ASSERT(y>=0);
        ASSERT(y>=CELLS_Y);
        if(x<0)
          x=0;
        if(x>=CELLS_X)
          x=CELLS_X-1;
        if(y<0)
          y=0;
        if(y>=CELLS_Y)
          y=CELLS_Y-1;
        grid[x][y].lastUpdate = value;
      }

      void setUpdate(int x, int y, unsigned int timestemp, int iStateDifference){
        ASSERT(x>=0);
        ASSERT(x<CELLS_X);
        ASSERT(y>=0);
        ASSERT(y>=CELLS_Y);
        if(x<0)
          x=0;
        if(x>=CELLS_X)
          x=CELLS_X-1;
        if(y<0)
          y=0;
        if(y>=CELLS_Y)
          y=CELLS_Y-1;
//        grid[x][y].DEBUGupdate(timestemp, iStateDifference, x, y);
        grid[x][y].update(timestemp, iStateDifference);
      }

      Cell getGrid(int x, int y){
        ASSERT(x>=0);
        ASSERT(x<CELLS_X);
        ASSERT(y>=0);
        ASSERT(y>=CELLS_Y);
        if(x<0)
          x=0;
        if(x>=CELLS_X)
          x=CELLS_X-1;
        if(y<0)
          y=0;
        if(y>=CELLS_Y)
          y=CELLS_Y-1;
        
        if((theFrameInfo.getTimeSince(grid[x][y].lastUpdate) > (int)ageThreshold))
          setState(x,y,Cell::FREE);
        return grid[x][y];
      }
      //helper
      Vector2<double> gridToWorld(Vector2<int> i){
        Vector2<double> vd;
        vd.x = (i.x-CELLS_X/2)*CELL_SIZE; 
        vd.y = -(i.y-CELLS_Y/2)*CELL_SIZE;
        return vd;
      }
      Vector2<double> gridToWorld(int x, int y){
        Vector2<double> vd;
        vd.x = (x-CELLS_X/2)*CELL_SIZE; 
        vd.y = -(y-CELLS_Y/2)*CELL_SIZE;
        return vd;
      }
      Vector2<int> worldToGrid(Vector2<double> vd){
        Vector2<int> vi;
        vi.x = (int)(vd.x/CELL_SIZE + CELLS_X/2 + 0.5); 
        vi.y = (int)(-vd.y/CELL_SIZE + CELLS_Y/2 + 0.5);
        return vi;
      }
  };

  virtual void execute();

private:
  Grid grid; // the grid itself, dimensions = 40 * 40, (60mm)

  // store around 3s
  RingBuffer<ObstacleModel::Obstacle, 75 > obstacleBuffer;
  unsigned int lastTimeObstacleWasSeen;

  OdometryData lastRobotOdometry;

  double maxValidDistance; /* max distance that is concidered valid */
  double minValidDistance; /* min distance that is concidered valid */

	unsigned int ageThreshold; /* max age of the considered data */
  double usOpeningAngle; // opening angle of the ultrasound detector (in radians)

	// unsigned int callCounter;
	// unsigned int calcEveryCall;

  // Functionality
  Vector2<double> getMean();
  Vector2<double> getMinimum();
  Vector2<double> getDampedMinimum();

	void updateBuffer();
	void ageBuffer();
  void updateGrid();
	void ageGrid();
  void provideToLocalObstacleModel();
  void updateByOdometry(const Pose2D& odometryDelta);	
  void drawObstacleModel();
  void drawObstacleModelNumbers();
	//virtual void calculatePoints();?
};

#endif //_UltraSoundObstacleLocator_h_


