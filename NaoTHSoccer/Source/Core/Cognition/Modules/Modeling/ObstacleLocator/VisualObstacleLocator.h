/**
 * @file VisualObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author Kirill Yasinovskiy
 * Declaration of class RadarObstacleLocator
 */

#ifndef _VisualObstacleLocator_h_
#define _VisualObstacleLocator_h_

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

// tools 
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/Math/Geometry.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/RadarGrid.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(VisualObstacleLocator)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(BallModel)
  REQUIRE(BallPercept)
  REQUIRE(UltraSoundReceiveData)

  PROVIDE(UltraSoundSendData)
  PROVIDE(LocalObstacleModel)
  PROVIDE(RadarGrid)
END_DECLARE_MODULE(VisualObstacleLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class VisualObstacleLocator : private VisualObstacleLocatorBase
{
public:
  VisualObstacleLocator();

  ~VisualObstacleLocator(){};

  virtual void execute();

private:

  //OdometryData lastRobotOdometry;


  RingBuffer<Vector2<double>, 120 > buffer;
  RingBuffer<unsigned int, 120 > timeBuffer;

  //
  double maxValidDistance;
  double minValidDistance;
  double usOpeningAngle;




  class GroundPoint
  {
  public:
    Vector2<double> posOnField;
    ColorClasses::Color color;
  };

  unsigned int lastTimeObstacleWasSeen;
  OdometryData lastRobotOdometry;
  bool onceExecuted;

  unsigned int initialTime;
  unsigned int currentTime;

  Pose2D odometryDelta;

  class GroundPointBuffer
  {
    int colors[ColorClasses::numOfColors];
    RingBuffer<GroundPoint, 10> buffer;

  public:
    GroundPointBuffer()
    {
      for(int i = 0; i < ColorClasses::numOfColors; i++)
        colors[i] = 0;
    }

    void add(const GroundPoint& point)
    {
      (colors[(int)point.color])++;
      buffer.add(point);
    }

    Vector2<double> getMean()
    {
      Vector2<double> result;
      for(int i = 0; i < buffer.getNumberOfEntries(); i++)
      {
        result += buffer[i].posOnField;
      }
      return result / ((double)buffer.getNumberOfEntries());
    }

    ColorClasses::Color getColor()
    {
      int max_count = 0;
      ColorClasses::Color max_color = ColorClasses::none;

      for(int i = 0; i < ColorClasses::numOfColors; i++)
      {
        if(colors[i] > max_count)
        {
          max_count = colors[i];
          max_color = (ColorClasses::Color)i;
        }
      }

      return max_color;
    }
  };

  inline void updateByUltraSoundData();


  std::vector<GroundPointBuffer> obstacleBuffer;
  double angle_offset;

  inline void add(const ScanLineEdgelPercept::EndPoint& point)
  {
    double value = point.posOnField.abs();
    int idx = ((int)Math::round((Math::toDegrees(value) + angle_offset))) % 20;
    ASSERT(idx >= 0 && idx < 18);

    GroundPoint p;
    p.color = point.color;
    p.posOnField = point.posOnField;
    obstacleBuffer[idx].add(p);
  }// end add

};

#endif //_VisualObstacleLocator_h_


