/* 
 * File:   TeamMessage.h
 * Author: thomas
 *
 * Created on 7. April 2009, 09:45
 */

#ifndef _TEAMMESSAGE_H
#define  _TEAMMESSAGE_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"

#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include "Messages/Messages.pb.h"

class TeamMessage : public naoth::Printable
{
public:


  TeamMessage()
  {
  };

  struct Data
  {
    naoth::FrameInfo frameInfo;
    naothmessages::TeamCommMessage message;
  };

  std::map<unsigned int, Data> data;

  // used for select opponent to send
  map<unsigned int, unsigned int> lastFrameNumberHearOpp;

  virtual ~TeamMessage() {}

  virtual void print(std::ostream& stream) const
  {
    for(std::map<unsigned int, Data>::const_iterator i=data.begin();
        i != data.end(); ++i)
    {
      const naothmessages::TeamCommMessage& message = i->second.message;
      stream << "From " << i->first << " ["<< message.bodyid() <<"]"<< endl;
      i->second.frameInfo.print(stream);
      stream << "Message: " << endl;
      stream << "\tPos (x; y; rotation) = "
             << message.positiononfield().translation().x() << "; "
             << message.positiononfield().translation().y() << "; "
             << message.positiononfield().rotation() << endl;
      stream << "\tBall (x; y) = "
             << message.ballposition().x() << "; "
             << message.ballposition().y() << endl;
      stream << "TimeSinceBallwasSeen: " << message.timesinceballwasseen() << endl;
      stream << "TimeToBall: "<< message.timetoball() <<endl;
      stream << "wasStriker: " << (message.wasstriker() ? "yes" : "no") << endl;
      stream << "isPenalized: " << (message.ispenalized() ? "yes" : "no") << endl;
      stream << "isFallenDown: " << (message.isfallendown() ? "yes" : "no") << endl;
      if ( message.has_opponent() ) {
        stream << "opponent " << message.opponent().number() << " : "
               << message.opponent().poseonfield().translation().x() << "; "
               << message.opponent().poseonfield().translation().y() << "; "
               << message.opponent().poseonfield().rotation()
               << "\n";
      }
      stream << "------------------------" << endl;
    }//end for

    stream << "----------------------"  << endl;
    stream << "active team-members: " << data.size() << endl;

  }//end print
};

#endif  /* _TEAMMESSAGE_H */

