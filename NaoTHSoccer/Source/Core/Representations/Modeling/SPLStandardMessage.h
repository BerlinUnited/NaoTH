#ifndef SPLSTANDARDMESSAGE_H
#define SPLSTANDARDMESSAGE_H

#include <cstdint>

#include <Representations/Infrastructure/FrameInfo.h>

static const int SPL_STANDARD_MESSAGE_STRUCT_VERSION = 1;
static const int SPL_STANDARD_MESSAGE_DATA_SIZE = 800;


// DO NOT MODIFY THE STRUCT
struct SPLStandardMessage 
{
  char header[4];        // "SPL "
  uint8_t version;       // has to be set to SPL_STANDARD_MESSAGE_STRUCT_VERSION
  uint8_t playerNum;     // 1-5
  uint16_t team;         // 0 is red 1 is blue (second byte is padding byte)

  // position and orientation of robot
  // coordinates in millimeters
  // 0,0 is in centre of field
  // +ve x-axis points towards the goal we are attempting to score on
  // +ve y-axis is 90 degrees counter clockwise from the +ve x-axis
  // angle in radians, 0 along the +x axis, increasing counter clockwise
  float pose[3];      // x,y,theta

  // Ball information
  int32_t ballAge;        // milliseconds since this robot last saw the ball. -1 if we haven't seen it

  // position of ball relative to the robot
  // coordinates in millimeters
  // 0,0 is in centre of the robot
  // +ve x-axis points forward from the robot
  // +ve y-axis is 90 degrees counter clockwise from the +ve x-axis
  float ball[2];

  // velocity of the ball (same coordinate system as above)
  float ballVel[2];

  // milliseconds since the robot has been fallen.  -1 if not fallen
  uint32_t fallen;

  // number of bytes that is actually used by the data array
  uint16_t numOfDataBytes;

  // buffer for arbitrary data
  uint8_t data[SPL_STANDARD_MESSAGE_DATA_SIZE];

  // constructor
  SPLStandardMessage()
  {
    header[0] = 'S';
    header[1] = 'P';
    header[2] = 'L';
    header[3] = ' ';
    version = SPL_STANDARD_MESSAGE_STRUCT_VERSION;
    playerNum = 0;
    team = 0;
    pose[0] = 0.f;
    pose[1] = 0.f;
    pose[2] = 0.f;
    ballAge = -1;
    ball[0] = 0.f;
    ball[1] = 0.f;
    ballVel[0] = 0.f;
    ballVel[1] = 0.f;
    fallen = -1;
    numOfDataBytes = 0;
  }
};

class ExtendedSPLStandardMessage
{
public:
  naoth::FrameInfo frameInfo;
  SPLStandardMessage msg;

  ExtendedSPLStandardMessage();
  virtual ~ExtendedSPLStandardMessage();

};

template<>
class Serializer<ExtendedSPLStandardMessage>
{
  public:
    static void serialize(const ExtendedSPLStandardMessage& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, ExtendedSPLStandardMessage& representation);
};


#endif // SPLSTANDARDMESSAGE_H
