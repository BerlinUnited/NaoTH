#include "TeamCommunicator.h"

#include <Representations/Infrastructure/Configuration.h>
#include <PlatformInterface/Platform.h>

#define TEAMCOMM_MAX_MSG_SIZE 4096

TeamCommunicator::TeamCommunicator()
  : initialized(false), socket(NULL), lastSentTimestamp(0),
    lanBroadcastAddress(NULL), wlanBroadcastAddress(NULL)
{
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  unsigned int port = 10700;
  if(config.hasKey("teamcomm", "port"))
  {
    port = config.getInt("teamcomm", "port");
  }

  GError* err = bindAndListen(port);
  if(err)
  {
    g_warning("could not initialize teamcomm: %s", err->message);
    if(socket)
    {
      g_object_unref(socket);
      socket = NULL;
    }
    g_error_free(err);
  }
  else
  {
    g_message("Team Communication started on port %d", port);

    g_socket_set_blocking(socket, false);

    if(config.hasKey("teamcomm", "wlan"))
    {
      GInetAddress* address = g_inet_address_new_from_string(config.getString("teamcomm", "wlan").c_str());
      wlanBroadcastAddress = g_inet_socket_address_new(address, port);
      g_object_unref(address);
    }
    if(config.hasKey("teamcomm", "lan"))
    {
      GInetAddress* address = g_inet_address_new_from_string(config.getString("teamcomm", "lan").c_str());
      lanBroadcastAddress = g_inet_socket_address_new(address, port);
      g_object_unref(address);
    }
  }

}

GError* TeamCommunicator::bindAndListen(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  if(err) return err;

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, port);

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  if (err) return err;
}

void TeamCommunicator::execute()
{
  if(socket != NULL)
  {
    // receveive data from others
    char buffer[TEAMCOMM_MAX_MSG_SIZE];

    gssize result = -1;
    do
    {
      result = g_socket_receive(socket, buffer,
        TEAMCOMM_MAX_MSG_SIZE, NULL, NULL);
      if(result > 0)
      {
        handleMessage(buffer, result);
      }
    } while(result > 0);

    // only send data in intervals of 500ms
    if(getFrameInfo().getTimeSince(lastSentTimestamp) > 500)
    {
      // send data
      naothmessages::TeamCommMessage msg;
      createMessage(msg);

      std::string s = msg.SerializeAsString();
      if(s.size() > TEAMCOMM_MAX_MSG_SIZE)
      {
        g_warning("tried to send too big TeamComm message, please adjust TEAMCOMM_MAX_MSG_SIZE");
      }
      else
      {
        if(lanBroadcastAddress)
        {
          g_socket_send_to(socket, lanBroadcastAddress, s.c_str(), s.size(), NULL, NULL);
        }
        if(wlanBroadcastAddress)
        {
          g_socket_send_to(socket, wlanBroadcastAddress, s.c_str(), s.size(), NULL, NULL);
        }

      }
      lastSentTimestamp = getFrameInfo().time;
    }
  }
}

void TeamCommunicator::handleMessage(char *buffer, gsize size)
{
  naothmessages::TeamCommMessage msg;
  msg.ParseFromArray(buffer, size);

  int num = msg.playernumber();
  if(num <= MAX_NUM_PLAYERS)
  {
    getTeamMessage().messageReceived[num] = true;
    getTeamMessage().timeWhenReceived[num] = getFrameInfo().time;
    getTeamMessage().frameNumberWhenReceived[num] = getFrameInfo().frameNumber;
    getTeamMessage().message[num] = msg;
  }
}

void TeamCommunicator::createMessage(naothmessages::TeamCommMessage &msg)
{

  msg.set_playernumber(getPlayerInfo().playerNumber);
  msg.set_teamnumber(getPlayerInfo().teamNumber);
  msg.set_ispenalized(getPlayerInfo().gameState == PlayerInfo::penalized);
  msg.set_wasstriker(getPlayerInfo().isPlayingStriker);

  // TODO: set falldown state in teamcomm message
  /*
  msg.set_isfallendown(theBodyState.fall_down_state != BodyState::upright
    || theMotionStatus.currentMotion == MotionRequestID::stand_up_from_back
    || theMotionStatus.currentMotion == MotionRequestID::stand_up_from_front);
  */

  // TODO: set ball and pose info in teamcomm message
  /*
  msg.set_timetoball(theSoccerStrategy.timeToBall);

  msg.set_timesinceballwasseen(
    getFrameInfo().getTimeSince(theBallModel.frameInfoWhenBallWasSeen.time));
  msg.mutable_ballposition()->set_x(theBallModel.position.x);
  msg.mutable_ballposition()->set_y(theBallModel.position.y);

  msg.mutable_positiononfield()->set_rotation(theRobotPose.rotation);
  msg.mutable_positiononfield()->mutable_translation()->set_x(theRobotPose.translation.x);
  msg.mutable_positiononfield()->mutable_translation()->set_y(theRobotPose.translation.y);
*/
}

TeamCommunicator::~TeamCommunicator()
{
  if(socket != NULL)
  {
    g_object_unref(socket);
  }
  if(lanBroadcastAddress != NULL)
  {
    g_object_unref(lanBroadcastAddress);
  }
  if(wlanBroadcastAddress != NULL)
  {
    g_object_unref(wlanBroadcastAddress);
  }
}
