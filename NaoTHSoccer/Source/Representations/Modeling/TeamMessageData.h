#ifndef TEAMMESSAGEDATA_H
#define TEAMMESSAGEDATA_H

/*
 * INFO:
 *  To add a new field to the teamcomm message, do as follows:
 *  - edit the "Representation.proto" file and extend the "BUUserTeamMessage" definition
 *  - (re-)compile the protobuf files
 *  - add a class variable (in the "BU message field" section)
 *  - use constructor to initialize/copy your variable
 *  - edit the getBUUserTeamMessage()/setFromBUUserTeamMessage() and make sure your variable
 *    gets written/parsed to/from the protobuf message
 * [ - do the same for (de-)serialization: serialize()/deserialize() ]
 */


#include <vector>

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
//#include "Tools/DataStructures/Serializer.h"
#include "MessagesSPL/SPLStandardMessage.h"

// needed by BUUserTeamMessage
#include "Messages/Representations.pb.h"
#include "Representations/Infrastructure/FrameInfo.h"

using namespace naoth;

// this key is sent with every team message to indicate that the message belongs to us
#define NAOTH_TEAMCOMM_MESAGE_KEY "naoth"

// this message is communicated as the user part of the standard spl message
class TeamMessageCustom : public naoth::Printable 
{

public:
  TeamMessageCustom();

  std::string key; // key making shur that the message was sent by one of our robots

  unsigned long long timestamp; // timestamp in ms when the message was send; 0 if it wasn't send
  std::string bodyID;         // the body ID of the robot
  bool wasStriker;
  bool wantsToBeStriker;
  unsigned int timeToBall;          // the shorest time, in which the robot can reach the ball [ms]
  bool isPenalized;           // whether the robot is penalized, or not
  double batteryCharge;       // the battery charge
  double temperature;         // the max. temperature of the left or right leg!
  // opponents ?

  /** Sets the data according to the protobuf message. */
  void parseFromProto(const naothmessages::BUUserTeamMessage& userData);

  /** Creates a protobuf message with the registered data. */
  naothmessages::BUUserTeamMessage toProto() const;

  void print(std::ostream &stream) const;
};

class TeamMessageData : public naoth::Printable
{
public:
    FrameInfo frameInfo;
    /*** BEGIN +++ TEAMMESSAGEFIELDS *****************************************/
    /*************************************************************************/
    /* SPL-Message-Fields ****************************************************/
    /*************************************************************************/
    unsigned int playerNumber;  // the number of the player
    unsigned int teamNumber;    // the number of the team
    bool fallen;                // true means that the robot is fallen, false means that the robot can play
    Pose2D pose;                // robot pose
    Vector2d walkingTo;         // the robot's target position on the field
    Vector2d shootingTo;        // the target position of the next shot (either pass or goal shot)
    double ballAge;             // milliseconds since this robot last saw the ball. -1 if we haven't seen it
    Vector2d ballPosition;      // position of ball relative to the robot coordinates in millimeters; 0,0 is in centre of the robot
    Vector2d ballVelocity;      // velocity of the ball
    std::vector<int> suggestion;// what should the teammates do (0-4)
    int intention;
    int averageWalkSpeed;       // the average speed that the robot has (mm/s)
    int maxKickDistance;        // the maximum distance that the ball rolls after a strong kick by the robot (mm)
    unsigned int positionConfidence; // current confidence of a robot about its self-location (0-100%); dummy value: currently always 100%
    unsigned int sideConfidence; // current confidence of a robot about playing in the right direction (0-100%); dummy value: currently always 100%
    
    // custom BU-Message-Fields
    TeamMessageCustom custom;
    /*** END +++ TEAMMESSAGEFIELDS *******************************************/

    TeamMessageData();
    TeamMessageData(FrameInfo fi);

    virtual ~TeamMessageData() {}

    /** Prints all known data to the given output stream. */
    virtual void print(std::ostream& stream) const;

    /** Creates a spl message object with currently known data. */
    SPLStandardMessage createSplMessage() const;

    /** Creates a string of the spl message.
     *  The createSplMessage() method is called and the returned spl object is "converted" to a string.
     */
    std::string createSplMessageString() const;

    /** Parses the informations of the spl message and updates the corresponding fields of this object. */
    bool parseFromSplMessage(const SPLStandardMessage &spl);

    //TODO: do we need to reset the values?! (see TeamCommReceiver::execute())
    //void clear();

private:
    /** Parses the SPL user data to a protobuf message and sets the data accordingly. */
    //void parseFromSplMessageData(const unsigned char *data, uint16_t size);
};

/*
namespace naoth {
template<> class Serializer<TeamMessageCustom>
{
public:
  static void serialize(const TeamMessageCustom& representation, std::ostream& stream);
  static void deserialize(std::istream& stream, TeamMessageCustom& representation);
};
}*/

#endif // TEAMMESSAGEDATA_H
