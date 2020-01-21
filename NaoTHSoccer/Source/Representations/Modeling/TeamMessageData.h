#ifndef TEAMMESSAGEDATA_H
#define TEAMMESSAGEDATA_H

/*
 * INFO:
 *  To add a new field to the teamcomm message, do as follows:
 *  - edit the "Representation.proto" file and extend the "BUUserTeamMessage" definition
 *  - (re-)compile the protobuf files
 *  - add a class variable to the TeamMessageCustom class
 *  - use constructor to initialize/copy your variable
 *  - edit the toProto() & parseFromProto() and make sure your variable gets written/parsed to/from the protobuf message
 * [ - add the variable to print(), so it gets printed out ... ]
 */


#include <vector>

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
//#include "Tools/DataStructures/Serializer.h"
#include "MessagesSPL/SPLStandardMessage.h"

// needed by BUUserTeamMessage
#include "Messages/TeamMessage.pb.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Infrastructure/Roles.h"

using namespace naoth;

// this key is sent with every team message to indicate that the message belongs to us
#define NAOTH_TEAMCOMM_MESAGE_KEY "naoth"

/** Contains data for a NTP request. */
struct NtpRequest
{
    NtpRequest(unsigned int p = 0, unsigned long long s = 0, unsigned long long r = 0)
        : playerNumber(p), sent(s), received(r)
    {}
    unsigned int playerNumber;
    unsigned long long sent;
    unsigned long long received;
};

// this message is communicated as the user part of the standard spl message
class TeamMessageCustom : public naoth::Printable 
{

public:

  // we define this struct private because no one else needs to know about the internals of the MixedTeamHeader
  struct MixedTeamHeader
  {
      // 1 bit == penalized, 2 bit == striker goalie
      int8_t data;
      int8_t role;
//    uint64_t timestamp;
//    int8_t teamID;
//    int8_t isPenalized;
//    int8_t whistleDetected;
//    int8_t intention;
  };

public:
  TeamMessageCustom();

  std::string key; // key making shur that the message was sent by one of our robots

  unsigned long long timestamp; // timestamp in ms when the message was send; 0 if it wasn't send
  std::string bodyID;         // the body ID of the robot
  bool wasStriker;
  bool wantsToBeStriker;
  unsigned int timeToBall;    // the shorest time, in which the robot can reach the ball [ms]
  double batteryCharge;       // the battery charge
  double temperature;         // the max. temperature of the left or right leg!
  double cpuTemperature;      // the temperature of the cpu
  bool whistleDetected;       // whether the robot heard/detected the whistle
  int whistleCount;           // who many whistle the robot detected
  Vector2d teamBall;          // global position of the team ball for visualization in RC!!
  std::vector<NtpRequest> ntpRequests; // ntp requests to teammates
  Vector2d ballVelocity;      // velocity of the ball
  // opponents ?

  PlayerInfo::RobotState robotState; // state of the robot (initial, ready, set, play, finish, penalized)
  Roles::Role robotRole;    // role of the robot (static & dynamic)

  bool readyToWalk;         // indicates, whether the robot is ready to walk

  /** Sets the data according to the protobuf message. */
  void parseFromProto(const naothmessages::BUUserTeamMessage& userData);

  /** Creates a protobuf message with the registered data. */
  naothmessages::BUUserTeamMessage toProto() const;

  /** Sets the data according to a binary mixed team header */
  void parseFromMixedTeamHeader(const uint8_t *rawHeader, size_t headerSize);

  /** Creates a binary array containing some of the registered data in the mixed team header "format" */
  void toMixedTeamHeader(MixedTeamHeader& header) const;

  static size_t getCustomOffset() {return sizeof(MixedTeamHeader);}

  void print(std::ostream &stream) const;
};

class TeamMessageData : public naoth::Printable
{
public:
    FrameInfo frameInfo;
    unsigned long long timestampParsed; //Timestamp when the message was parsed.
    /*** BEGIN +++ TEAMMESSAGEFIELDS *****************************************/
    /*************************************************************************/
    /* SPL-Message-Fields ****************************************************/
    /*************************************************************************/
    unsigned int playerNumber;  // the number of the player
    unsigned int teamNumber;    // the number of the team
    bool fallen;                // true means that the robot is fallen, false means that the robot can play
    Pose2D pose;                // robot pose
    double ballAge;             // milliseconds since this robot last saw the ball. -1 if we haven't seen it
    Vector2d ballPosition;      // position of ball relative to the robot coordinates in millimeters; 0,0 is in centre of the robot
    
    // custom BU-Message-Fields
    TeamMessageCustom custom;
    /*** END +++ TEAMMESSAGEFIELDS *******************************************/

    bool isBerlinUnitedMessage() { return custom.key == NAOTH_TEAMCOMM_MESAGE_KEY; }

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
