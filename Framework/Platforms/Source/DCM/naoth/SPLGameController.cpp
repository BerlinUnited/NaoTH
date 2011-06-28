#include "SPLGameController.h"
#include <cstdlib>
#include <PlatformInterface/Platform.h>
#include <sys/socket.h>
#include "Tools/MacAddr.h"

using namespace naoth;

void* socketLoopWrap(void* c)
{
  SPLGameController* ctr = static_cast<SPLGameController*> (c);
  ctr->socketLoop();
  return NULL;
}//end motionLoopWrap

SPLGameController::SPLGameController()
  :exiting(false),
    socket(NULL),
    broadcastAddress(NULL),
    socketThread(NULL),
    dataMutex(NULL),
    lastUpdatedIndex(-1),
    writingIndex(0)
{
  GError* err = bindAndListen();
  if(err)
  {
    g_warning("could not listen for SPLGameController: %s", err->message);
    socket = NULL;
    g_error_free(err);
  }
  else
  {
    if (!g_thread_supported())
      g_thread_init(NULL);
    dataMutex = g_mutex_new();

    g_message("SPLGameController start socket thread");
    socketThread = g_thread_create(socketLoopWrap, this, true, NULL);
    ASSERT(socketThread != NULL);
    g_thread_set_priority(socketThread, G_THREAD_PRIORITY_LOW);
  }
}

GError* SPLGameController::bindAndListen(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  if(err) return err;
  g_socket_set_blocking(socket, true);

  int broadcast = 1;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, port);

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  string broadcastAddr = getBroadcastAddr("wlan0");
  GInetAddress* address = g_inet_address_new_from_string(broadcastAddr.c_str());
  broadcastAddress = g_inet_socket_address_new(address, port);
  g_object_unref(address);

  return err;
}

bool SPLGameController::update(GameData& gameData, unsigned int time)
{
  Data* data = NULL;
  if ( g_mutex_trylock(dataMutex) )
  {
    if ( lastUpdatedIndex >= 0 )
    {
      data = &(theData[lastUpdatedIndex]);
      lastUpdatedIndex = -1;
    }
    g_mutex_unlock(dataMutex);
  }

  // update
  if ( data != NULL )
  {
    RoboCupGameControlData& dataIn = data->dataIn;
    std::string header;
    header.assign(dataIn.header, 4);
    if(header == GAMECONTROLLER_STRUCT_HEADER)
    {
      GameData::GameState lastGameState = gameData.gameState;
      int teamInfoIndex = -1;
      TeamInfo tinfo;

      bool isRed = true;

      if (dataIn.teams[TEAM_RED].teamNumber == gameData.teamNumber)
      {
        isRed = true;
      }
      else if (dataIn.teams[TEAM_BLUE].teamNumber == gameData.teamNumber)
      {
        isRed = false;
      }
      else
      {
        // no message for us invalidate data
        return false;
      }

      if(isRed)
      {
        tinfo = dataIn.teams[TEAM_RED];
        teamInfoIndex = TEAM_RED;
        gameData.teamColor = GameData::red;
      }
      else
      {
        tinfo = dataIn.teams[TEAM_BLUE];
        teamInfoIndex = TEAM_BLUE;
        gameData.teamColor = GameData::blue;
      }

      if (teamInfoIndex >= 0)
      {
        switch (dataIn.state)
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

        gameData.numOfPlayers = dataIn.playersPerTeam;

        if ( gameData.gameState == GameData::ready
            || gameData.gameState == GameData::set
            || gameData.gameState == GameData::playing )
        {
          //TODO: check more conditions (time, etc.)
          gameData.playMode = (dataIn.kickOffTeam == teamInfoIndex) ? GameData::kick_off_own : GameData::kick_off_opp;
        }

        unsigned char playerNumberForGameController = gameData.playerNumber - 1; // gamecontroller starts counting at 0

        if(playerNumberForGameController < MAX_NUM_PLAYERS)
        {
          RobotInfo rinfo =
            dataIn.teams[teamInfoIndex].players[playerNumberForGameController];
          if (rinfo.penalty != PENALTY_NONE)
          {
            gameData.gameState = GameData::penalized;
            gameData.penaltyState = (GameData::PenaltyState)rinfo.penalty;
          }
        }
      }

      if ( lastGameState != gameData.gameState )
      {
        gameData.timeWhenGameStateChanged = time;
      }

      // return data
      RoboCupGameControlReturnData& dataOut = data->dataOut;
      strcpy(dataOut.header, GAMECONTROLLER_RETURN_STRUCT_HEADER);
      dataOut.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
      dataOut.team = gameData.teamNumber;
      dataOut.player = gameData.playerNumber;
      dataOut.message = GAMECONTROLLER_RETURN_MSG_ALIVE;

      return true;
    } // end if header correct
  }

  return false;
}

void SPLGameController::update()
{
  int size = g_socket_receive(socket, (char*)(&(theData[writingIndex].dataIn)), sizeof(RoboCupGameControlData), NULL, NULL);
  if(size == sizeof(RoboCupGameControlData))
  {
    sendData(theData[writingIndex].dataOut);

    g_mutex_lock(dataMutex);
    if ( lastUpdatedIndex < 0 )
    {
      lastUpdatedIndex = writingIndex;

      writingIndex++;
      if (writingIndex>1)
        writingIndex = 0;
    }
    g_mutex_unlock(dataMutex);
  }
}

SPLGameController::~SPLGameController()
{
  exiting = true;

  g_thread_join(socketThread);
  g_mutex_free(dataMutex);

  if(socket != NULL)
  {
    g_object_unref(socket);
  }

  if(broadcastAddress != NULL)
  {
    g_object_unref(broadcastAddress);
  }
}

void SPLGameController::sendData(const RoboCupGameControlReturnData& data)
{
  GError *error = NULL;
  gssize result = g_socket_send_to(socket, broadcastAddress, (char*)(&data), sizeof(data), NULL, &error);
  if ( result != sizeof(data) )
  {
    g_warning("SPLGameController::returnData, sended size = %d", result);
  }
  if (error)
  {
    g_warning("g_socket_send_to error: %s", error->message);
    g_error_free(error);
  }
}

void SPLGameController::socketLoop()
{
  if(socket == NULL)
  {
    return;
  }

  while(!exiting)
  {
    update();
  }
}

