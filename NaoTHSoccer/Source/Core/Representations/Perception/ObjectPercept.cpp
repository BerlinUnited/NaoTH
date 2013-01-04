/**
 * @file ObjectPercept.cpp
 * 
 * Definition of class ObjectPercept
 */ 

#include "ObjectPercept.h"


using namespace naoth;
using namespace std;

void ObjectPercept::print(ostream& stream) const
{
  stream << "Object Count: " << numOfSeenObjects << "/" << (MAXNUMBEROFLINES + MAXNUMBEROFOBSTACLES + MAXNUMBEROFBALLS) << endl;

  stream << "Line Edgels:" << endl << "------" << endl;
  for(unsigned int i = 0; i < scanLineEdgels.size(); i++)
  {
    stream << "Edgel " << i << endl;
    stream << "  Begin = " << scanLineEdgels[i].begin << " angle = " << scanLineEdgels[i].begin_angle << endl;
    stream << "  Center = " << scanLineEdgels[i].center << " angle = " << scanLineEdgels[i].center_angle << endl;
    stream << "  End = " << scanLineEdgels[i].end << " angle = " << scanLineEdgels[i].end_angle << endl;
    stream << "  ScanLine = " << scanLineEdgels[i].ScanLineID << " run = " << scanLineEdgels[i].runID << endl;
    stream << "  is valid = " << (scanLineEdgels[i].valid ? "true" : "false") << endl;
  }
  stream << "Line Count: " << numOfSeenLines << "/" << MAXNUMBEROFLINES << endl << endl;

  stream << "Lines:" << endl << "------" << endl;
  for(unsigned int i = 0; i < numOfSeenLines; i++)
  {
    stream << "Line " << i << "/" << MAXNUMBEROFLINES << ": " << endl;
    stream << "  Slope = " << lines[i].slope << endl;
    stream << "  Thickness = " << lines[i].thickness << endl;
    stream << "  is valid = " << (lines[i].valid ? "true" : "false") << endl;
  }
  stream << "Line Count: " << numOfSeenLines << "/" << MAXNUMBEROFLINES << endl << endl;

  stream << "choosen Ball: " << bestBallIndex << endl;
  stream << "Balls:" << endl << "------" << endl;
  for(unsigned int i = 0; i < numOfSeenBalls; i++)
  {
    stream << "Ball " << i << "/" << MAXNUMBEROFBALLS << ": " << endl;
    stream << "  Mass = " << balls[i].mass << endl;
    stream << "  Center = " << balls[i].centerInImage << endl;
    stream << "  Radius inside Image = " << balls[i].radiusInImage << endl;
    stream << "  Calculated Radius = " << balls[i].radius << endl;
    stream << "  Color = " << ColorClasses::getColorName(balls[i].color) << endl;
    stream << "  bearing Offset = " << balls[i].bearingBasedOffsetOnField << endl;
    stream << "  Ball distance = " << balls[i].distance << endl;
    stream << "  gamma = " << balls[i].gamma << endl;
    stream << "  alpha = " << balls[i].alpha << endl;
    stream << "  height = " << balls[i].height << endl;
  }
  stream << "Ball Count: " << numOfSeenBalls << "/" << MAXNUMBEROFBALLS << endl << endl;

  stream << "Obstacles:" << endl << "---------" << endl;
  for(unsigned int i = 0; i < numOfSeenObstacles; i++)
  {
    stream << "Obstacle " << i << "/" << MAXNUMBEROFOBSTACLES << ": " << endl;
    stream << "  Center = " << obstacles[i].centerInImage << endl;
    stream << "  Radius = " << obstacles[i].radiusInImage << endl;
    stream << "  Color = " << ColorClasses::getColorName(obstacles[i].color) << endl;
    stream << "  Mass = " << obstacles[i].mass << endl;
  }
  stream << "Obstacle Count: " << numOfSeenObstacles << "/" << MAXNUMBEROFOBSTACLES << endl;
}//end print


void Serializer<ObjectPercept>::serialize(const ObjectPercept& representation, std::ostream& stream)
{
  
}//end serialize

void Serializer<ObjectPercept>::deserialize(std::istream& stream, ObjectPercept& representation)
{
  
}//end deserialize