#include "GameController.h"

#include "Tools/Debug/DebugRequest.h"
#include <PlatformInterface/Platform.h>

GameController::GameController()
  : lastChestButtonEventCounter(0)
{
  DEBUG_REQUEST_REGISTER("gamecontroller:play", "force the play state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:penalized", "force the penalized state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:initial", "force the initial state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:ready", "force the ready state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set", "force the set state", false);

  getPlayerInfo().gameData.gameState = GameData::initial;
}

void GameController::readButtons()
{
  // state change?
  if (getButtonData().eventCounter[ButtonData::Chest] > lastChestButtonEventCounter )
  {
    lastChestButtonEventCounter = getButtonData().eventCounter[ButtonData::Chest];
    switch (getPlayerInfo().gameData.gameState)
    {
      case GameData::initial :

          getPlayerInfo().gameData.gameState = GameData::penalized;
        break;
      case GameData::playing :

          getPlayerInfo().gameData.gameState = GameData::penalized;
        break;
      case GameData::penalized :

          getPlayerInfo().gameData.gameState = GameData::playing;
        break;
     default:
        break;
    }
  }

  // re-set team color or kickoff in initial
  if (getPlayerInfo().gameData.gameState == GameData::initial)
  {
    if (getButtonData().numOfFramesPressed[ButtonData::LeftFootLeft] == 1
      || getButtonData().numOfFramesPressed[ButtonData::LeftFootRight] == 1)
    {
      // switch team color
      GameData::TeamColor oldColor = getPlayerInfo().gameData.teamColor;
      if (oldColor == GameData::blue)
      {
        getPlayerInfo().gameData.teamColor = GameData::red;
      }
      else if (oldColor == GameData::red)
      {
        getPlayerInfo().gameData.teamColor = GameData::blue;
      }
    }

    if (getButtonData().numOfFramesPressed[ButtonData::RightFootLeft] == 1
      || getButtonData().numOfFramesPressed[ButtonData::RightFootRight] == 1)
    {
      // switch kick off team
      if ( getPlayerInfo().gameData.playMode == GameData::kick_off_own )
      {
        getPlayerInfo().gameData.playMode = GameData::kick_off_opp;
      }
      else
      {
        getPlayerInfo().gameData.playMode = GameData::kick_off_own;
      }
    }
  }
    // go back from penalized to initial both foot bumpers are pressed for over 30 frames
  else if (getPlayerInfo().gameData.gameState == GameData::penalized &&
    (getButtonData().numOfFramesPressed[ButtonData::LeftFootLeft] > 30
    || getButtonData().numOfFramesPressed[ButtonData::LeftFootRight] > 30
    )
    &&
    (getButtonData().numOfFramesPressed[ButtonData::RightFootLeft] > 30
    || getButtonData().numOfFramesPressed[ButtonData::RightFootRight] > 30
    )
    )
  {
    getPlayerInfo().gameData.gameState = GameData::initial;
  }

} // end readButtons

void GameController::execute()
{
  GameData::GameState oldState = getPlayerInfo().gameData.gameState;
  GameData::TeamColor oldTeamColor = getPlayerInfo().gameData.teamColor;
  GameData::PlayMode oldPlayMode = getPlayerInfo().gameData.playMode;

  readButtons();
  if(getGameData().valid)
  {
    getPlayerInfo().gameData = getGameData();
  }

  DEBUG_REQUEST("gamecontroller:initial",
    getPlayerInfo().gameData.gameState = GameData::initial;
  );
  DEBUG_REQUEST("gamecontroller:ready",
    getPlayerInfo().gameData.gameState = GameData::ready;
  );
  DEBUG_REQUEST("gamecontroller:set",
    getPlayerInfo().gameData.gameState = GameData::set;
  );
  DEBUG_REQUEST("gamecontroller:play",
    getPlayerInfo().gameData.gameState = GameData::playing;
  );
  DEBUG_REQUEST("gamecontroller:penalized",
    getPlayerInfo().gameData.gameState = GameData::penalized;
  );

  if(oldState != getPlayerInfo().gameData.gameState
    || oldTeamColor != getPlayerInfo().gameData.teamColor
    || oldPlayMode != getPlayerInfo().gameData.playMode
    || getPlayerInfo().gameData.gameState == GameData::initial)
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
  switch (getPlayerInfo().gameData.gameState)
  {
    case GameData::ready :
      std::cout << "in ready" << std::endl;
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 1.0;
      break;
    case GameData::set :
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 1.0;
      getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
      break;
    case GameData::playing :
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 1.0;
      break;
    case GameData::penalized :
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
      break;
    default:
      break;
  }

  // show team color on left foot
  if (getPlayerInfo().gameData.teamColor == GameData::red)
  {
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::RED] = 1.0;
  }
  else if (getPlayerInfo().gameData.teamColor == GameData::blue)
  {
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::BLUE] = 1.0;
  }

  // show kickoff state on right foot in initial, ready and set
  if (getPlayerInfo().gameData.gameState == GameData::initial
    || getPlayerInfo().gameData.gameState == GameData::ready
    || getPlayerInfo().gameData.gameState == GameData::set)
  {
    if (getPlayerInfo().gameData.playMode == GameData::kick_off_own)
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
