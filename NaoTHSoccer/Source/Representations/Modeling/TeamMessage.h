/* 
 * File:   TeamMessage.h
 * Author: thomas
 *
 * Created on 7. April 2009, 09:45
 */

#ifndef _TEAMMESSAGE_H
#define _TEAMMESSAGE_H

#include <limits>
#include <vector>

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Serializer.h"

#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/FrameInfo.h"


#include "Messages/Messages.pb.h"

class TeamMessage : public naoth::Printable
{
public:


  TeamMessage()
  {
  }

  struct Opponent
  {
    int playerNum;
    Pose2D poseOnField;
  };

  struct Data
  {
    Data ()
      : playerNum(0),teamNumber(0),
        ballAge(-1), fallen(false),
        bodyID("unknown"),
        timeToBall(std::numeric_limits<unsigned int>::max()),
        wasStriker(false),
        wantsToBeStriker(false),
        isPenalized(false),
        batteryCharge(0.0f),
        temperature(0.0f),
        timestamp(0)
    {
    }

    naoth::FrameInfo frameInfo;

    unsigned int playerNum;

    unsigned int teamNumber;

    /**
     * @brief Own robot pose
     */
    Pose2D pose;
    /**
     * @brief milliseconds since this robot last saw the ball.
     * -1 if we haven't seen it
     */
    int ballAge;
    /**
     * @brief position of ball relative to the robot
     * coordinates in millimeters
     * 0,0 is in centre of the robot
     */
    Vector2d ballPosition;
    /**
     * @brief velocity of the ball
     */
    Vector2d ballVelocity;
    /**
     * @brief true means that the robot is fallen, false means that the robot can play
     */
    bool fallen;

    // BEGIN BERLIN UNITED SPECIFIC
    std::string bodyID;
    unsigned int timeToBall;
    bool wasStriker; //TODO: deprecated - rename to isPlayingStriker
    bool wantsToBeStriker;
    bool isPenalized;
    std::vector<Opponent> opponents;
    float batteryCharge;
    float temperature;
    unsigned long long timestamp;
    // END BERLIN UNITED SPECIFIC

  };

  typedef std::map<unsigned int, Data> PlayerMessageMap;
  PlayerMessageMap data;

  virtual ~TeamMessage() {}

  virtual void print(std::ostream& stream) const
  {
    for(PlayerMessageMap::const_iterator i=data.begin();
        i != data.end(); ++i)
    {
      const Data& d = i->second;
      stream << "From " << i->first << " ["<< d.bodyID <<"]"<< std::endl;
      i->second.frameInfo.print(stream);
      stream << "Message: " << std::endl;
      stream << "Timestamp" << d.timestamp << std::endl;
      stream << "\tPos (x; y; rotation) = "
             << d.pose.translation.x << "; "
             << d.pose.translation.y << "; "
             << d.pose.rotation << std::endl;
      stream << "\tBall (x; y) = "
             << d.ballPosition.x << "; "
             << d.ballPosition.y << std::endl;
      stream << "TimeSinceBallwasSeen: " << d.ballAge << std::endl;
      stream << "TimeToBall: "<< d.timeToBall << std::endl;
      stream << "wasStriker: " << (d.wasStriker ? "yes" : "no") << std::endl;
      stream << "isPenalized: " << (d.isPenalized ? "yes" : "no") << std::endl;
      stream << "fallenDown: " << (d.fallen ? "yes" : "no") << std::endl;
      stream << "team number: " << d.teamNumber << std::endl;
      for(unsigned int i=0; i < d.opponents.size(); i++)
      {
        stream << "opponent " << d.opponents[i].playerNum << " : "
               << d.opponents[i].poseOnField.translation.x << "; "
               << d.opponents[i].poseOnField.translation.y << "; "
               << d.opponents[i].poseOnField.rotation
               << std::endl;
      }
      stream << "------------------------" << std::endl;
    }//end for

    stream << "----------------------"  << std::endl;
    stream << "active team-members: " << data.size() << std::endl;

  }//end print
};

namespace naoth {
  template<>
  class Serializer<TeamMessage>
  {
  public:
    static void serialize(const TeamMessage& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, TeamMessage& representation);
  };
}

#endif  /* _TEAMMESSAGE_H */

