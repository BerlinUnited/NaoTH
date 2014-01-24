#include "GameController.h"

#include "Tools/Debug/DebugRequest.h"

GameController::GameController()
{
  DEBUG_REQUEST_REGISTER("gamecontroller:play", "force the play state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:penalized", "force the penalized state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:initial", "force the initial state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:ready", "force the ready state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set", "force the set state", false);

  getPlayerInfo().gameData.gameState = GameData::initial;
}

void GameController::execute()
{
  GameData::GameState oldState = getPlayerInfo().gameData.gameState;
  GameData::TeamColor oldTeamColor = getPlayerInfo().gameData.teamColor;
  GameData::PlayMode oldPlayMode = getPlayerInfo().gameData.playMode;

  readButtons();

  if ( getGameData().valid ) {
    getPlayerInfo().gameData = getGameData();
  }
  getPlayerInfo().gameData.frameNumber = getFrameInfo().getFrameNumber();


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

void GameController::readButtons()
{
  // default return message
  getGameReturnData().message = GameReturnData::alive;

  // state change?
  
  if (getButtonState()[ButtonState::Chest] == ButtonEvent::CLICKED)
  {
    switch (getPlayerInfo().gameData.gameState)
    {
    case GameData::initial :
    {
      getPlayerInfo().gameData.gameState = GameData::penalized;
      getPlayerInfo().gameData.timeWhenGameStateChanged = getFrameInfo().getTime();
      getGameReturnData().message = GameReturnData::manual_penalise;
      break;
    }
    case GameData::playing :
    {
      getPlayerInfo().gameData.gameState = GameData::penalized;
      getPlayerInfo().gameData.timeWhenGameStateChanged = getFrameInfo().getTime();
      getGameReturnData().message = GameReturnData::manual_penalise;
      break;
    }
    case GameData::penalized :
    {
      getPlayerInfo().gameData.gameState = GameData::playing;
      getPlayerInfo().gameData.timeWhenGameStateChanged = getFrameInfo().getTime();
      getGameReturnData().message = GameReturnData::manual_unpenalise;
      break;
    }
    default:
      // always go back to play as default
      getPlayerInfo().gameData.gameState = GameData::playing;
      getPlayerInfo().gameData.timeWhenGameStateChanged = getFrameInfo().getTime();
      break;
    }
  }

  // re-set team color or kickoff in initial
  if (getPlayerInfo().gameData.gameState == GameData::initial)
  {
    if ( getButtonState()[ButtonState::LeftFoot] == ButtonEvent::PRESSED )
    {
      // switch team color
      GameData::TeamColor oldColor = getPlayerInfo().gameData.teamColor;
      if (oldColor == GameData::blue) {
        getPlayerInfo().gameData.teamColor = GameData::red;
      } else if (oldColor == GameData::red) {
        getPlayerInfo().gameData.teamColor = GameData::blue;
      }
    }

    if ( getButtonState()[ButtonState::RightFoot] == ButtonEvent::PRESSED )
    {
      // switch kick off team
      if ( getPlayerInfo().gameData.playMode == GameData::kick_off_own ) {
        getPlayerInfo().gameData.playMode = GameData::kick_off_opp;
      } else {
        getPlayerInfo().gameData.playMode = GameData::kick_off_own;
      }
    }
  }
  // go back from penalized to initial both foot bumpers are pressed for more than 1s
  else if (getPlayerInfo().gameData.gameState == GameData::penalized &&
    (  getButtonState()[ButtonState::LeftFoot].isPressed && 
       getFrameInfo().getTimeSince(getButtonState()[ButtonState::LeftFoot].timeOfLastEvent) > 1000 )
    &&
    (  getButtonState()[ButtonState::RightFoot].isPressed && 
       getFrameInfo().getTimeSince(getButtonState()[ButtonState::RightFoot].timeOfLastEvent) > 1000 )
    )
  {
    getPlayerInfo().gameData.gameState = GameData::initial;
  }

} // end readButtons

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
