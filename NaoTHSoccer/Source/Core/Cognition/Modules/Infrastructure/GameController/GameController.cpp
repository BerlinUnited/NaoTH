#include "GameController.h"

#include "Tools/Debug/DebugRequest.h"
#include <Representations/Infrastructure/Configuration.h>
#include <PlatformInterface/Platform.h>

GameController::GameController()
  : lastChestButtonEventCounter(0)
{
  DEBUG_REQUEST_REGISTER("gamecontroller:play", "force the play state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:penalized", "force the penalized state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:initial", "force the initial state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:ready", "force the ready state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set", "force the set state", false);

  loadPlayerInfoFromFile();
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
  // TODO: WLAN GameController support
}

void GameController::execute()
{
  PlayerInfo::GameState oldState = getPlayerInfo().gameState;
  PlayerInfo::TeamColor oldTeamColor = getPlayerInfo().teamColor;
  bool oldOwnKickOff = getPlayerInfo().ownKickOff;


  readButtons();

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
}
