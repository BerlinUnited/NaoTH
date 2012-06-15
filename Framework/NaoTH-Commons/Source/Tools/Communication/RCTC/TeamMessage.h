#ifndef _TeamMessage_H_
#define _TeamMessage_H_

#include <map>
#include <vector>
#include <ostream>
#include <stdint.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <inttypes.h>
#include <netinet/in.h>
#endif

/**
 * Initial draft of a general team comm between players of different teams.
 * This method will be used in a demo match @ RC Mexico 2012
 *
 * This Module receives messages, stores information and sends out local information to every team member
 *.
 *
 * HEADER ['RCTC']
 *   (network byte order)
 *
 * TEAM ID [0, 1]
 *   Senders Team ID
 *
 * PLAYER_ID [0..N]
 *   Sender ID
 *
 * GOALIE_ID
 *    The goalie is the player with the lowest ID.
 *    He is also the team captain and decides which robot should appraoch the ball (nearest to the ball)
 *
 * MAB (May Approach Ball)
 *   MY_ID == MAB_ID : Robot may go to the ball
 *
 * REL_BALL_POSX [in cm]
 *   positive x axis pointing forward set to 32767 if no ball was seen (network byte order)
 * REL_BALL_POSY [in cm]
 *   positive y axis pointing to the left set to 32767 if no ball was seen (network byte order)
 *
 *    | [4] Header | [1] TEAM_ID | [1] PLAYER_ID
 *    | [3] ROLE_IDS | [1] MAB_ID | [2] REL_BALL_POSX | [2] REL_BALL_POSY
 */

namespace rctc
{

  class Message
  {
  public:
    Message() 
      : 
      teamID(0), 
      playerID(0), 
      goalieID(0), 
      mabID(0), 
      ballPosX(0), 
      ballPosY(0)
    {}
  
    uint8_t teamID;
    uint8_t playerID;
	  uint8_t goalieID;
	  uint8_t mabID;
	  int16_t ballPosX;
	  int16_t ballPosY;
  };


  // constants
  const int PACKET_SIZE              = 12;
  const int RCTC_HEADER_SIZE         = 4;
  const uint32_t RCTC_HEADER_CONTENT = 0x52435443; //'RCTC'; // gcc warning: multi-character character constant

  
  // types
  typedef enum 
  {
	  // header
	  RCTC_HEADER     = 0,
	  RCTC_TEAMID     = 4,
	  RCTC_PID        = 5,
	  RCTC_GOALIE_ID  = 6, // why do we need it?
	  RCTC_MAB_ID     = 7,
	  RCTC_BALL_POS_X = 8, // address low byte
	  RCTC_BALL_POS_Y = 10
  } RCTC_PACKETFIELDS;

  typedef std::map<uint8_t, Message> MsgMap;

 
  // functions
  /**
    *
    */
	bool binaryToMessage(const uint8_t* data, Message &msg);


  /**
    *
    */
  void messageToBinary(const Message& msg, uint8_t *data);

  
  void print(const Message& msg);

  std::ostream& operator<< (std::ostream& stream, const rctc::Message& msg);

}//end namespace rctc





#endif /* _TeamMessage_H_ */
