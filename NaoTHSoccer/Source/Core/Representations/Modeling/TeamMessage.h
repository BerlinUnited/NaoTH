/* 
 * File:   TeamMessage.h
 * Author: thomas
 *
 * Created on 7. April 2009, 09:45
 */

#ifndef _TEAMMESSAGE_H
#define	_TEAMMESSAGE_H

#include <Tools/Communication/RoboCupGameControlData.h>
#include <Tools/Math/Vector2.h>
#include <Messages/Messages.pb.h>
#include <Tools/DataStructures/Printable.h>

class TeamMessage : public Printable
{
public:

  TeamMessage()
  {
    for(unsigned int i=0; i <= MAX_NUM_PLAYERS; i++)
    {
      messageReceived[i] = false;
      timeWhenReceived[i] = 0;
      frameNumberWhenReceived[i] = 0;
    }
  };

  bool messageReceived[MAX_NUM_PLAYERS+1];
  unsigned int timeWhenReceived[MAX_NUM_PLAYERS+1];
  unsigned int frameNumberWhenReceived[MAX_NUM_PLAYERS+1];
  naothmessages::TeamCommMessage message[MAX_NUM_PLAYERS+1];

  virtual ~TeamMessage() {};

  virtual void print(std::ostream& stream) const
  {
    int count = 0;
    for(int i = 1; i <= MAX_NUM_PLAYERS; i++)
    {
//      if(messageReceived[i])
      {
        Vector2<double> ballPos;
        ballPos.x = message[i].ballposition().x();
        ballPos.y = message[i].ballposition().y();
        stream << "From " << i << endl;
        stream << "Time: " << timeWhenReceived[i] << endl;
        stream << "Frame: " << frameNumberWhenReceived[i] << endl;
        stream << "Message: " << endl;
        stream << "\tPos (x; y; rotation) = "
          << message[i].positiononfield().translation().x() << "; "
          << message[i].positiononfield().translation().y() << "; "
          << message[i].positiononfield().rotation() << endl;
        stream << "\tBall (x; y; distance) = "
          << message[i].ballposition().x() << "; " 
          << message[i].ballposition().y() << "; "
          << ballPos.abs() << endl;
        stream << "TimeSinceBallwasSeen: " << message[i].timesinceballwasseen() << endl;
        stream << "TimeToBall: "<< message[i].timetoball() <<endl;
        stream << "wasStriker: " << (message[i].wasstriker() ? "yes" : "no") << endl;
        stream << "isPenalized: " << (message[i].ispenalized() ? "yes" : "no") << endl;
        if ( message[i].has_opponent() ) {
          stream << "opponent " << message[i].opponent().number() << " : "
            << message[i].opponent().poseonfield().translation().x() << "; "
            << message[i].opponent().poseonfield().translation().y() << "; "
            << message[i].opponent().poseonfield().rotation()
            << "\n";
        }
        count++;
        stream << "------------------------" << endl;
      }//end if
    }//end for

    stream << "----------------------"  << endl;
    stream << "active team-members: " << count << endl;

  }//end print
};

#endif	/* _TEAMMESSAGE_H */

