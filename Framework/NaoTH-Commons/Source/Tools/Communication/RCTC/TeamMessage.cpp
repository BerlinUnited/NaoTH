
#include "TeamMessage.h"
#include <cstring>
#include <cstdio>

namespace rctc
{
  // functions
  /**
    *
    */
	bool binaryToMessage(const uint8_t* data, Message &msg)
  {
    // check header
	  uint32_t header;
	  memcpy(&header, &data[RCTC_HEADER], 4);
	  header = ntohl(header);
	  if (header != RCTC_HEADER_CONTENT) {
      printf("ERROR: Invalid header RCTC. Header is %c%c%c%c\n", data[0], data[1], data[2], data[3]);
		  return false;
	  }

    msg.teamID     = data[RCTC_TEAMID];
    msg.playerID   = data[RCTC_PID];
	  msg.goalieID   = data[RCTC_GOALIE_ID];
    msg.mabID      = data[RCTC_MAB_ID];
    
    int16_t ballPosX;
	  int16_t ballPosY;
	  memcpy(&ballPosX, &data[RCTC_BALL_POS_X], 2);
	  memcpy(&ballPosY, &data[RCTC_BALL_POS_Y], 2);
    msg.ballPosX = (int16_t) ntohs(ballPosX);
	  msg.ballPosX = (int16_t) ntohs(ballPosY);

    return true;
  }//end binaryToMessage


  /**
    *
    */
  void messageToBinary(const Message& msg, uint8_t *data)
  {
    // set header
	  uint32_t header = htonl(RCTC_HEADER_CONTENT);
	  memcpy(&data[RCTC_HEADER], &header, RCTC_HEADER_SIZE);

    // fill the message
    data[RCTC_TEAMID]    = msg.teamID;
    data[RCTC_PID]       = msg.playerID;
	  data[RCTC_GOALIE_ID] = msg.goalieID;
    data[RCTC_MAB_ID]    = msg.mabID;

    // ball information
	  uint16_t ballPosX = htons( (uint16_t) msg.ballPosX);
	  uint16_t ballPosY = htons( (uint16_t) msg.ballPosY);
    memcpy(&data[RCTC_BALL_POS_X], &ballPosX, 2);
	  memcpy(&data[RCTC_BALL_POS_Y], &ballPosY, 2);
  }//end messageToBinary

  
  void print(const Message& msg)
  {
    printf("teamID:  \t%d\n", msg.teamID);
    printf("playerID:\t%d\n", msg.playerID);
	  printf("goalieID:\t%d\n", msg.goalieID);
    printf("mabID:   \t%d\n", msg.mabID);
    printf("ballPosX:\t%d\n", msg.ballPosX);
    printf("ballPosY:\t%d\n", msg.ballPosY);
  }

  std::ostream& operator<< (std::ostream& stream, const rctc::Message& msg)
  {
    stream << "teamID:  \t" << msg.teamID   << std::endl;
    stream << "playerID:\t" << msg.playerID << std::endl;
    stream << "goalieID:\t" << msg.goalieID << std::endl;
    stream << "mabID:   \t" << msg.mabID << std::endl;
    stream << "ballPosX:\t" << msg.ballPosX << std::endl;
    stream << "ballPosY:\t" << msg.ballPosY << std::endl;
    return stream;
  }//end print

}//end namespace rctc

