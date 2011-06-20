#include "GameController.h"

#include "Tools/Debug/DebugRequest.h"
#include <Representations/Infrastructure/Configuration.h>
#include <PlatformInterface/Platform.h>

GameController::GameController()
  : lastChestButtonEventCounter(0), socket(NULL)
{
  DEBUG_REQUEST_REGISTER("gamecontroller:play", "force the play state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:penalized", "force the penalized state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:initial", "force the initial state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:ready", "force the ready state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set", "force the set state", false);

  getPlayerInfo().gameState = PlayerInfo::inital;

  loadPlayerInfoFromFile();

  GError* err = bindAndListen();
  if(err)
  {
    g_warning("could not listen for GameController: %s", err->message);
    socket = NULL;
    g_error_free(err);
  }
}

GError* GameController::bindAndListen(unsigned int port)
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

void GameController::loadPlayerInfoFromFile()
{
  // load player.cfg
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;

  if (config.hasKey("player", "NumOfPlayer"))
  {
    getPlayerInfo().numOfPlayers = config.getInt("player", "NumOfPlayer");
  } else
  {
    std::cerr << "No number of players (NumOfPlayers) given" << std::endl;
  }

  if (config.hasKey("player", "PlayerNumber"))
  {
    getPlayerInfo().playerNumber = config.getInt("player", "PlayerNumber");
  } else
  {
    std::cerr << "No player number (PlayerNumber) given" << std::endl;
    getPlayerInfo().playerNumber = 3;
  }

  if (config.hasKey("player", "TeamColor"))
  {
    getPlayerInfo().teamColor = PlayerInfo::teamColorFromString(config.getString("player", "TeamColor"));
    if (getPlayerInfo().teamColor == PlayerInfo::numOfTeamColor)
    {
      getPlayerInfo().teamColor = PlayerInfo::red;
      std::cerr << "Invalid team color (TeamColor) \""
        << config.getString("player", "TeamColor") << "\" given" << std::endl;
    }
  } else
  {
    std::cerr << "No team color (TeamColor) given" << std::endl;
    getPlayerInfo().teamColor = PlayerInfo::red;
  }

  if (config.hasKey("player", "TeamNumber"))
  {
    getPlayerInfo().teamNumber = config.getInt("player", "TeamNumber");
  } else
  {
    std::cerr << "No team number (TeamNumber) given" << std::endl;
    getPlayerInfo().teamNumber = 0;
  }
} // end loadPlayerInfoFromFile

void GameController::readButtons()
{

  // state change?
  if (getButtonData().eventCounter[ButtonData::Chest] > lastChestButtonEventCounter )
  {
    lastChestButtonEventCounter = getButtonData().eventCounter[ButtonData::Chest];
    switch (getPlayerInfo().gameState)
    {
      case PlayerInfo::inital :

          getPlayerInfo().gameState = PlayerInfo::penalized;
        break;
      case PlayerInfo::playing :

          getPlayerInfo().gameState = PlayerInfo::penalized;
        break;
      case PlayerInfo::penalized :

          getPlayerInfo().gameState = PlayerInfo::playing;
        break;
     default:
        break;
    }
  }

  // re-set team color or kickoff in initial
  if (getPlayerInfo().gameState == PlayerInfo::inital)
  {
    if (getButtonData().numOfFramesPressed[ButtonData::LeftFootLeft] == 1
      || getButtonData().numOfFramesPressed[ButtonData::LeftFootRight] == 1)
    {
      // switch team color
      PlayerInfo::TeamColor oldColor = getPlayerInfo().teamColor;
      if (oldColor == PlayerInfo::blue)
      {
        getPlayerInfo().teamColor = PlayerInfo::red;
      }
      else if (oldColor == PlayerInfo::red)
      {
        getPlayerInfo().teamColor = PlayerInfo::blue;
      }
    }

    if (getButtonData().numOfFramesPressed[ButtonData::RightFootLeft] == 1
      || getButtonData().numOfFramesPressed[ButtonData::RightFootRight] == 1)
    {
      // switch kickof team
      getPlayerInfo().ownKickOff = !getPlayerInfo().ownKickOff;
    }
  }
    // go back from penalized to initial both foot bumpers are pressed for over 30 frames
  else if (getPlayerInfo()  .gameState == PlayerInfo::penalized &&
    (getButtonData().numOfFramesPressed[ButtonData::LeftFootLeft] > 30
    || getButtonData().numOfFramesPressed[ButtonData::LeftFootRight] > 30
    )
    &&
    (getButtonData().numOfFramesPressed[ButtonData::RightFootLeft] > 30
    || getButtonData().numOfFramesPressed[ButtonData::RightFootRight] > 30
    )
    )
  {
    getPlayerInfo().gameState = PlayerInfo::inital;
  }

} // end readButtons

void GameController::readWLAN()
{
  if(socket == NULL)
  {
    return;
  }

  char* tmp = (char*) malloc(sizeof(RoboCupGameControlData));
  int size = g_socket_receive(socket, tmp, sizeof(RoboCupGameControlData), NULL, NULL);
  if(size == sizeof(RoboCupGameControlData))
  {
    RoboCupGameControlData* data = (RoboCupGameControlData*) tmp;
    std::string header;
    header.assign(data->header, 4);
    if(header == GAMECONTROLLER_STRUCT_HEADER)
    {
      int teamInfoIndex = -1;
      TeamInfo tinfo;

      bool isRed = true;

      if (data->teams[TEAM_RED].teamNumber == getPlayerInfo().teamNumber)
      {
        isRed = true;
      }
      else if (data->teams[TEAM_BLUE].teamNumber == getPlayerInfo().teamNumber)
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
        getPlayerInfo().teamColor = PlayerInfo::red;
      }
      else
      {
        tinfo = data->teams[TEAM_BLUE];
        teamInfoIndex = TEAM_BLUE;
        getPlayerInfo().teamColor = PlayerInfo::blue;
      }

      if (teamInfoIndex >= 0)
      {
        switch (data->state)
        {
          case STATE_INITIAL:
            getPlayerInfo().gameState = PlayerInfo::inital;
            break;
          case STATE_READY:
            getPlayerInfo().gameState = PlayerInfo::ready;
            break;
          case STATE_SET:
            getPlayerInfo().gameState = PlayerInfo::set;
            break;
          case STATE_PLAYING:
            getPlayerInfo().gameState = PlayerInfo::playing;
            break;
          case STATE_FINISHED:
            getPlayerInfo().gameState = PlayerInfo::finished;
            break;
        }

        getPlayerInfo().numOfPlayers = data->playersPerTeam;

        getPlayerInfo().ownKickOff = (data->kickOffTeam == teamInfoIndex);

        unsigned char playerNumberForGameController =
          getPlayerInfo().playerNumber - 1; // gamecontroller starts counting at 0

        if(playerNumberForGameController < MAX_NUM_PLAYERS)
        {
          RobotInfo rinfo =
            data->teams[teamInfoIndex].players[playerNumberForGameController];
          if (rinfo.penalty != PENALTY_NONE)
          {
            getPlayerInfo().gameState = PlayerInfo::penalized;
          }
        }
      }
    } // end if header correct
  } // end if size correct
  free(tmp);
} // end updateWLAN

void GameController::execute()
{
  PlayerInfo::GameState oldState = getPlayerInfo().gameState;
  PlayerInfo::TeamColor oldTeamColor = getPlayerInfo().teamColor;
  bool oldOwnKickOff = getPlayerInfo().ownKickOff;

  readButtons();
  readWLAN();

  DEBUG_REQUEST("gamecontroller:initial",
    getPlayerInfo().gameState = PlayerInfo::inital;
  );
  DEBUG_REQUEST("gamecontroller:ready",
    getPlayerInfo().gameState = PlayerInfo::ready;
  );
  DEBUG_REQUEST("gamecontroller:set",
    getPlayerInfo().gameState = PlayerInfo::set;
  );
  DEBUG_REQUEST("gamecontroller:play",
    getPlayerInfo().gameState = PlayerInfo::playing;
  );
  DEBUG_REQUEST("gamecontroller:penalized",
    getPlayerInfo().gameState = PlayerInfo::penalized;
  );

  if(oldState != getPlayerInfo().gameState
    || oldTeamColor != getPlayerInfo().teamColor
    || oldOwnKickOff != getPlayerInfo().ownKickOff
    || getPlayerInfo().gameState == PlayerInfo::inital)
  {
    updateLEDs();
  }

} // end execute

void GameController::updateLEDs()
{
  // reset
  getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 0.0;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 0.0;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 0.0;

  getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::RED] = 0.0;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::GREEN] = 0.0;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::BLUE] = 0.0;

  getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::RED] = 0.0;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::GREEN] = 0.0;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::BLUE] = 0.0;

  // show game state in torso
  switch (getPlayerInfo().gameState)
  {
    case PlayerInfo::ready :
      std::cout << "in ready" << std::endl;
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 1.0;
      break;
    case PlayerInfo::set :
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 1.0;
      getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
      break;
    case PlayerInfo::playing :
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 1.0;
      break;
    case PlayerInfo::penalized :
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
      break;
    default:
      break;
  }

  // show team color on left foot
  if (getPlayerInfo().teamColor == PlayerInfo::red)
  {
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::RED] = 1.0;
  }
  else if (getPlayerInfo().teamColor == PlayerInfo::blue)
  {
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::BLUE] = 1.0;
  }

  // show kickoff state on right foot in initial, ready and set
  if (getPlayerInfo().gameState == PlayerInfo::inital
    || getPlayerInfo().gameState == PlayerInfo::ready
    || getPlayerInfo().gameState == PlayerInfo::set)
  {
    if (getPlayerInfo().ownKickOff)
    {
      getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::RED] = 0.7;
      getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::GREEN] = 1.0;
      getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::BLUE] = 1.0;
    }
  }
} // end updateLEDs

GameController::~GameController()
{
  if(socket != NULL)
  {
    g_object_unref(socket);
  }
}
