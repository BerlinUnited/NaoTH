#include "SPLGameController.h"
#include <cstdlib>
#include <PlatformInterface/Platform.h>
#include <sys/socket.h>
#include "Tools/Communication/NetAddr.h"

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
    lastGetTime(0),
    dataMutex(NULL),
    returnDataMutex(NULL)
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
    // init player number, team number and etc.
    data.loadFromCfg( naoth::Platform::getInstance().theConfiguration );
    // init return data
    strcpy(dataOut.header, GAMECONTROLLER_RETURN_STRUCT_HEADER);
    dataOut.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
    dataOut.team = data.teamNumber;
    dataOut.player = data.playerNumber;
    dataOut.message = GAMECONTROLLER_RETURN_MSG_ALIVE;

    if (!g_thread_supported())
      g_thread_init(NULL);
    dataMutex = g_mutex_new();
    returnDataMutex = g_mutex_new();

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

  string broadcastAddr = NetAddr::getBroadcastAddr("wlan0");
  GInetAddress* address = g_inet_address_new_from_string(broadcastAddr.c_str());
  broadcastAddress = g_inet_socket_address_new(address, port);
  g_object_unref(address);

  return err;
}

bool SPLGameController::get(GameData& gameData, unsigned int time)
{
  gameData.valid = false;
  bool ok = false; // TODO: do we need this ok?

  if ( g_mutex_trylock(dataMutex) )
  {
    if ( data.valid )
    {
      if (   data.gameState == GameData::playing
          || data.gameState == GameData::penalized )
      {
        data.gameTime += (time - lastGetTime);
      }

      if ( gameData.gameState != data.gameState )
      {
        data.timeWhenGameStateChanged = time;
      }

      if ( gameData.playMode != data.playMode )
      {
        data.timeWhenPlayModeChanged = time;
      }

      gameData = data;
      data.valid = false;
      lastGetTime = time;
      ok = true;

    }//end if data.valid
    g_mutex_unlock(dataMutex);
  }//end if trylock

  return ok;
}//end get gameData

bool SPLGameController::update()
{
  std::string header;
  header.assign(dataIn.header, 4);
  if(header == GAMECONTROLLER_STRUCT_HEADER)
  {
    int teamInfoIndex = -1;
    TeamInfo tinfo;

    bool isRed = true;

    if (dataIn.teams[TEAM_RED].teamNumber == data.teamNumber)
    {
      isRed = true;
    }
    else if (dataIn.teams[TEAM_BLUE].teamNumber == data.teamNumber)
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
      data.teamColor = GameData::red;
    }
    else
    {
      tinfo = dataIn.teams[TEAM_BLUE];
      teamInfoIndex = TEAM_BLUE;
      data.teamColor = GameData::blue;
    }

    if (teamInfoIndex >= 0)
    {
      switch (dataIn.state)
      {
      case STATE_INITIAL:
        data.gameState = GameData::initial;
        break;
      case STATE_READY:
        data.gameState = GameData::ready;
        break;
      case STATE_SET:
        data.gameState = GameData::set;
        break;
      case STATE_PLAYING:
        data.gameState = GameData::playing;
        break;
      case STATE_FINISHED:
        data.gameState = GameData::finished;
        break;
      }

      data.numOfPlayers = dataIn.playersPerTeam;
      data.msecsRemaining = dataIn.secsRemaining * 1000;
      data.firstHalf = (dataIn.firstHalf == 1);

      if ( dataIn.secondaryState == STATE2_NORMAL )
      {
        if ( data.gameState == GameData::initial
            || data.gameState == GameData::ready
            || data.gameState == GameData::set
            || data.gameState == GameData::playing )
        {
          //TODO: check more conditions (time, etc.)
          data.playMode = (dataIn.kickOffTeam == teamInfoIndex) ? GameData::kick_off_own : GameData::kick_off_opp;
        }
      }
      else if ( dataIn.secondaryState == STATE2_PENALTYSHOOT )
      {
        data.playMode = (dataIn.kickOffTeam == teamInfoIndex) ? GameData::penalty_kick_own : GameData::penalty_kick_opp;
      }
      else if ( dataIn.secondaryState == STATE2_OVERTIME )
      {
        data.playMode = GameData::game_over;
      }

      unsigned char playerNumberForGameController = data.playerNumber - 1; // gamecontroller starts counting at 0

      if(playerNumberForGameController < MAX_NUM_PLAYERS)
      {
        RobotInfo rinfo =
            dataIn.teams[teamInfoIndex].players[playerNumberForGameController];
        data.penaltyState = (GameData::PenaltyState)rinfo.penalty;
        data.msecsTillUnpenalised = rinfo.secsTillUnpenalised * 1000;
        if (rinfo.penalty != PENALTY_NONE)
        {
          data.gameState = GameData::penalized;
        }
      }
    }
    return true;
  } // end if header correct
  return false;
}

void SPLGameController::setReturnData(const naoth::GameReturnData& data)
{
  if ( g_mutex_trylock(returnDataMutex) )
  {
    dataOut.message = data.message;
    g_mutex_unlock(returnDataMutex);
  }
}

SPLGameController::~SPLGameController()
{
  exiting = true;

  g_thread_join(socketThread);
  g_mutex_free(dataMutex);
  g_mutex_free(returnDataMutex);

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
    int size = g_socket_receive(socket, (char*)(&dataIn), sizeof(RoboCupGameControlData), NULL, NULL);
    if(size == sizeof(RoboCupGameControlData))
    {
      g_mutex_lock(dataMutex);
      if ( update() )
      {
        data.valid = true;
      }
      g_mutex_unlock(dataMutex);

      g_mutex_lock(returnDataMutex);
      sendData(dataOut);
      g_mutex_unlock(returnDataMutex);
    }
  }
}

