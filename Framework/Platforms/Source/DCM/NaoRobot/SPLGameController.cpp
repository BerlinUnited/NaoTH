#include "SPLGameController.h"
#include <cstdlib>
#include <PlatformInterface/Platform.h>
#include <sys/socket.h>
#include "Tools/Communication/NetAddr.h"

using namespace naoth;
using namespace std;

void* socketLoopWrap(void* c)
{
  SPLGameController* ctr = static_cast<SPLGameController*> (c);
  ctr->socketLoop();
  return NULL;
}//end motionLoopWrap

SPLGameController::SPLGameController()
  :exiting(false), port(GAMECONTROLLER_PORT),
    socket(NULL),
    gamecontrollerAddress(NULL),
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
    dataOut.team = static_cast<uint8_t>(data.teamNumber);
    dataOut.player = static_cast<uint8_t>(data.playerNumber);
    dataOut.message = GAMECONTROLLER_RETURN_MSG_ALIVE;

    if (!g_thread_supported()) {
      g_thread_init(NULL);
	}
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
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, static_cast<guint16>(port));

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

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
  if(header == GAMECONTROLLER_STRUCT_HEADER && dataIn.version == GAMECONTROLLER_STRUCT_VERSION)
  {
    bool teamFound = false;
    TeamInfo tinfo;

    for(unsigned int i=0; i < 2; i++)
    {
      if(dataIn.teams[i].teamNumber == data.teamNumber)
      {
        tinfo = dataIn.teams[i];
        teamFound = true;
        data.teamColor = tinfo.teamColour == TEAM_BLUE ? GameData::blue : GameData::red;
        break;
      }
    }


    if(teamFound)
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
          data.playMode = (dataIn.kickOffTeam == tinfo.teamColour) ? GameData::kick_off_own : GameData::kick_off_opp;
        }
      }
      else if ( dataIn.secondaryState == STATE2_PENALTYSHOOT )
      {
        data.playMode = (dataIn.kickOffTeam == tinfo.teamColour) ? GameData::penalty_kick_own : GameData::penalty_kick_opp;
      }
      else if ( dataIn.secondaryState == STATE2_OVERTIME )
      {
        data.playMode = GameData::game_over;
      }

      unsigned char playerNumberForGameController = static_cast<unsigned char>(data.playerNumber - 1); // gamecontroller starts counting at 0

      if(playerNumberForGameController < MAX_NUM_PLAYERS)
      {
        RobotInfo rinfo =
            tinfo.players[playerNumberForGameController];

        data.penaltyState = (GameData::PenaltyState)rinfo.penalty;
        data.msecsTillUnpenalised = rinfo.secsTillUnpenalised * 1000;
        if (rinfo.penalty != PENALTY_NONE)
        {
          data.gameState = GameData::penalized;
        }
      }
    } // endif teamFound
    else
    {
      // no message for us invalidate data
      return false;
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

  if(gamecontrollerAddress != NULL)
  {
    g_object_unref(gamecontrollerAddress);
  }
}

void SPLGameController::sendData(const RoboCupGameControlReturnData& data)
{
  GError *error = NULL;
  if(gamecontrollerAddress != NULL)
  {
    gssize result = g_socket_send_to(socket, gamecontrollerAddress, (char*)(&data), sizeof(data), NULL, &error);
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
}

void SPLGameController::socketLoop()
{
  if(socket == NULL)
  {
    return;
  }

  while(!exiting)
  {
    GSocketAddress* receiverAddress = NULL;
    int size = g_socket_receive_from(socket, &receiverAddress,
                                     (char*)(&dataIn),
                                     sizeof(RoboCupGameControlData),
                                     NULL, NULL);

    if(receiverAddress != NULL)
    {
      // construct a proper return address from the receiver
      GInetAddress* rawAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(receiverAddress));
      if(gamecontrollerAddress != NULL)
      {
        g_object_unref(gamecontrollerAddress);
      }
      gamecontrollerAddress = g_inet_socket_address_new(rawAddress, static_cast<guint16>(port));
      g_object_unref(receiverAddress);
    }

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

