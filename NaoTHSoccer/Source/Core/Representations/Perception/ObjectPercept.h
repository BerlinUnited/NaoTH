/** 
 * @file ObjectPercept.h
 *
 * Declaration of class ObjectPercept
 */

#ifndef _ObjectPercept_h_
#define _ObjectPercept_h_

#include <vector>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Polygon.h"
#include "Tools/ColorClasses.h"

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include "Tools/ImageProcessing/LineDetectorConstParameters.h"
#include "Tools/ImageProcessing/Edgel.h"


#include "Representations/Infrastructure/FrameInfo.h"


class ObjectPercept : public naoth::Printable
{ 
public:
  ObjectPercept()
    :
    ballWasSeen(false),
    obstacleWasSeen(false),
    lineWasSeen(false),
    bestBallIndex(0),
    numOfSeenObjects(0),
    numOfSeenBalls(0),
    numOfSeenObstacles(0),
    numOfSeenLines(0)
  {
    scanLineEdgels.reserve(SCANLINE_COUNT * (SCANLINE_RESUME_COUNT + 1));
    for(unsigned int i = 0; i < MAXNUMBEROFLINES; i++)
    {
      lines[i].thickness = 0;
      lines[i].slope = 0.0;
      lines[i].angle = 0.0;
      lines[i].valid = false;
    }
    for(unsigned int i = 0; i < MAXNUMBEROFBALLS; i++)
    {
      balls[i].radiusInImage = 0.0;
      balls[i].radius = 0.0;
      balls[i].mass = 0;
      balls[i].distance = 0.0;
      balls[i].gamma = 0.0;
      balls[i].alpha = 0.0;
      balls[i].height = 0.0;
    }
    for(unsigned int i = 0; i < MAXNUMBEROFOBSTACLES; i++)
    {
      obstacles[i].radiusInImage = 0.0;
      obstacles[i].mass = 0;
    }
  }

  const static unsigned int MAXNUMBEROFLINES = 22;
  const static unsigned int MAXNUMBEROFBALLS = 20;
  const static unsigned int MAXNUMBEROFOBSTACLES = 20;

  /* indicates wether a ball was seen, i.e., the percept is valid */
  bool ballWasSeen;
  /* indicates wether a ball was seen, i.e., the percept is valid */
  bool obstacleWasSeen;
  /* indicates wether a ball was seen, i.e., the percept is valid */
  bool lineWasSeen;

  unsigned int bestBallIndex;

  unsigned int numOfSeenObjects;
  unsigned int numOfSeenBalls;
  unsigned int numOfSeenObstacles;
  unsigned int numOfSeenLines;

   struct FieldLine
   {
    Vector2<int> begin;
    Vector2<int> end;
    Vector2<double> beginOnTheField;
    Vector2<double> endOnTheField;
    Math::Polygon<40> shape;
    int thickness;
    double slope;
    double angle;
    bool valid;
  };

  struct Ball
  {
    /* center of ball in image coordinates */
    Vector2<double> centerInImage;
    /* radius of ball in image */
    double radiusInImage;
    double radius;
    ColorClasses::Color color;
    /* relative ball position to the robot based on the camera matrix*/
    Vector2<double> bearingBasedOffsetOnField;
    int mass;
    double distance;
    double gamma;
    double alpha;
    double height;
  };

  struct Obstacle
  {
    /* center of obstacle in image coordinates */
    Vector2<double> centerInImage;
    /* radius of ball in image */
    double radiusInImage;
    ColorClasses::Color color;
    /* relative obstacleposition to the robot based on the camera matrix*/
    Vector2<double> bearingBasedOffsetOnField;

    int mass;
  };

  FieldLine lines[MAXNUMBEROFLINES];
  Ball balls[MAXNUMBEROFBALLS];
  Obstacle obstacles[MAXNUMBEROFOBSTACLES];

  std::vector<Edgel> scanLineEdgels;

  void addLine(const FieldLine& line)
  {
    if(numOfSeenLines < MAXNUMBEROFLINES)
    {
      if(numOfSeenLines == 0)
      {
        lineWasSeen = true;
      }
      lines[numOfSeenLines] = line;
      numOfSeenLines++;
      numOfSeenObjects++;
    }//end if
  }//end add

  void addObstacle(const Obstacle& obstacle)
  {
    if(numOfSeenObstacles < MAXNUMBEROFLINES)
    {
      if(numOfSeenObstacles == 0)
      {
        obstacleWasSeen = true;
      }
      obstacles[numOfSeenObstacles] = obstacle;
      numOfSeenObstacles++;
      numOfSeenObjects++;
    }//end if
  }//end add

  void addBall(const Ball& ball)
  {
    if(numOfSeenBalls < MAXNUMBEROFLINES)
    {
      if(numOfSeenBalls == 0)
      {
        ballWasSeen = true;
      }
      balls[numOfSeenBalls] = ball;
      numOfSeenBalls++;
      numOfSeenObjects++;
    }//end if
  }//end add

  /* reset percept */
  void reset()
  {
    numOfSeenObjects = 0;
    numOfSeenBalls = 0;
    numOfSeenObstacles = 0;
    numOfSeenLines = 0;

    obstacleWasSeen = false;
    ballWasSeen = false;
    lineWasSeen = false;

    bestBallIndex = 0;

    scanLineEdgels.clear();
  }//end reset

  virtual void print(std::ostream& stream) const;
};

namespace naoth
{
  template<>
  class Serializer<ObjectPercept>
  {
  public:
    static void serialize(const ObjectPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, ObjectPercept& representation);
  };
}

#endif //_ObjectPercept_h_


