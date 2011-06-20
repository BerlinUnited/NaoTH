#include "SPLGameController.h"
#include <cstdlib>

using namespace naoth;

SPLGameController::SPLGameController()
  :socket(NULL)
{
  GError* err = bindAndListen();
  if(err)
  {
    g_warning("could not listen for SPLGameController: %s", err->message);
    socket = NULL;
    g_error_free(err);
  }

  buffer = (char*) malloc(sizeof(RoboCupGameControlData));
}

GError* SPLGameController::bindAndListen(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  if(err) return err;
  g_socket_set_blocking(socket, false);

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, port);

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  if (err) return err;
}

void SPLGameController::update(GameData& gameData)
{
  if(socket == NULL)
  {
    return;
  }

  int size = g_socket_receive(socket, buffer, sizeof(RoboCupGameControlData), NULL, NULL);
  if(size == sizeof(RoboCupGameControlData))
  {
    RoboCupGameControlData* data = (RoboCupGameControlData*) buffer;
    std::string header;
    header.assign(data->header, 4);
    if(header == GAMECONTROLLER_STRUCT_HEADER)
    {
      int teamInfoIndex = -1;
      TeamInfo tinfo;

      bool isRed = true;

      if (data->teams[TEAM_RED].teamNumber == gameData.teamNumber)
      {
        isRed = true;
      }
      else if (data->teams[TEAM_BLUE].teamNumber == gameData.teamNumber)
      {
        isRed = false;
      }
      else
      {
        // no message for us invalidate data
        return;
      }

      if(isRed)
      {
        tinfo = data->teams[TEAM_RED];
        teamInfoIndex = TEAM_RED;
        gameData.teamColor = GameData::red;
      }
      else
      {
        tinfo = data->teams[TEAM_BLUE];
        teamInfoIndex = TEAM_BLUE;
        gameData.teamColor = GameData::blue;
      }

      if (teamInfoIndex >= 0)
      {
        switch (data->state)
        {
          case STATE_INITIAL:
            gameData.gameState = GameData::initial;
            break;
          case STATE_READY:
            gameData.gameState = GameData::ready;
            break;
          case STATE_SET:
            gameData.gameState = GameData::set;
            break;
          case STATE_PLAYING:
            gameData.gameState = GameData::playing;
            break;
          case STATE_FINISHED:
            gameData.gameState = GameData::finished;
            break;
        }

        gameData.numOfPlayers = data->playersPerTeam;

        if ( gameData.gameState == GameData::playing )
        {
          //TODO: check more conditions (time, etc.)
          gameData.playMode = (data->kickOffTeam == teamInfoIndex) ? GameData::kick_off_own : GameData::kick_off_opp;
        }

        unsigned char playerNumberForGameController = gameData.playerNumber - 1; // gamecontroller starts counting at 0

        if(playerNumberForGameController < MAX_NUM_PLAYERS)
        {
          RobotInfo rinfo =
            data->teams[teamInfoIndex].players[playerNumberForGameController];
          if (rinfo.penalty != PENALTY_NONE)
          {
            gameData.gameState = GameData::penalized;
          }
        }
      }
    } // end if header correct
  } // end if size correct
} // end updateWLAN

SPLGameController::~SPLGameController()
{
  if(socket != NULL)
  {
    g_object_unref(socket);
  }

  free(buffer);
}
