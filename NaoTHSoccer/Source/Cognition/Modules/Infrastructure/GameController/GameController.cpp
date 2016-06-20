#include "GameController.h"

#include "Tools/Debug/DebugRequest.h"
#include <PlatformInterface/Platform.h>

GameController::GameController()
  : 
  lastWhistleCount(0),
  returnMessage(GameReturnData::alive)
{
  DEBUG_REQUEST_REGISTER("gamecontroller:play", "force the play state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:penalized", "force the penalized state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:initial", "force the initial state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:ready", "force the ready state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set", "force the set state", false);

  // TODO: make it parameters?
  // load values from config
  const Configuration& config = naoth::Platform::getInstance().theConfiguration;

  if (config.hasKey("player", "NumOfPlayer")) {
    getPlayerInfo().playersPerTeam = config.getInt("player", "NumOfPlayer");
  } else {
    std::cerr << "[GameData] " << "No number of players (NumOfPlayers) given" << std::endl;
  }

  if (config.hasKey("player", "PlayerNumber")) {
    getPlayerInfo().playerNumber = config.getInt("player", "PlayerNumber");
  } else {
    std::cerr << "[PlayerInfo] " << "No player number (PlayerNumber) given" << std::endl;
    getPlayerInfo().playerNumber = 3;
  }

  if (config.hasKey("player", "TeamNumber")) {
    getPlayerInfo().teamNumber = config.getInt("player", "TeamNumber");
  } else {
    std::cerr << "[PlayerInfo] " << "No team number (TeamNumber) given" << std::endl;
    getPlayerInfo().teamNumber = 0;
  }

  if (config.hasKey("player", "TeamName")) {
    getPlayerInfo().teamName = config.getString("player", "TeamName");
  } else {
    std::cerr << "[PlayerInfo] " << "No team name (TeamName) given" << std::endl;
    getPlayerInfo().teamName = "unknown";
  }

  // NOTE: default team color is red
  getPlayerInfo().teamColor = GameData::red;
  if (config.hasKey("player", "TeamColor"))
  {
    getPlayerInfo().teamColor = GameData::teamColorFromString(config.getString("player", "TeamColor"));
    if (getPlayerInfo().teamColor == GameData::unknown_team_color)
    {
      std::cerr << "[GameData] " << "Invalid team color (TeamColor) \""
        << config.getString("player", "TeamColor") << "\" given" << std::endl;
    }
  } else {
    std::cerr << "[GameData] " << "No team color (TeamColor) given" << std::endl;
  }
}

void GameController::execute()
{
  PlayerInfo::RobotState oldRobotState = getPlayerInfo().robotState;
  GameData::TeamColor oldTeamColor = getPlayerInfo().teamColor;

  // try update from the game controller message
  if ( getGameData().valid ) 
  {
    getPlayerInfo().update(getGameData());

    // reset return message if old message was accepted
    if( returnMessage == GameReturnData::manual_penalise
        && getGameData().getOwnRobotInfo(getPlayerInfo().playerNumber).penalty != GameData::none)
    {
      returnMessage = GameReturnData::alive;
    }
    else if(returnMessage == GameReturnData::manual_unpenalise
            && getGameData().getOwnRobotInfo(getPlayerInfo().playerNumber).penalty == GameData::none)
    {
      returnMessage = GameReturnData::alive;
    }
  }
  
  // keep the manual penalized state
  if(returnMessage == GameReturnData::manual_penalise) {
    getPlayerInfo().robotState = PlayerInfo::penalized;
  }

  handleButtons();
  handleHeadButtons();
  
  DEBUG_REQUEST("gamecontroller:initial",
    getPlayerInfo().robotState = PlayerInfo::initial;
  );
  DEBUG_REQUEST("gamecontroller:ready",
    getPlayerInfo().robotState = PlayerInfo::ready;
  );
  DEBUG_REQUEST("gamecontroller:set",
    getPlayerInfo().robotState = PlayerInfo::set;
  );
  DEBUG_REQUEST("gamecontroller:play",
    getPlayerInfo().robotState = PlayerInfo::playing;
  );

  
  // remember the whistle counter before set
  if(getPlayerInfo().robotState == PlayerInfo::ready) {
    lastWhistleCount = getWhistlePercept().counter;
  }
  // whistle overrides gamecontroller when in set
  else if(getGameData().gameState == GameData::set)
  {
    // switch from set to play
    if(getWhistlePercept().counter > lastWhistleCount) {
      getPlayerInfo().robotState = PlayerInfo::playing;
    }
  }


  if(  oldRobotState != getPlayerInfo().robotState
    || oldTeamColor  != getPlayerInfo().teamColor
    || getPlayerInfo().robotState == PlayerInfo::initial)
  {
    updateLEDs();
  }

  // provide the return message
  getGameReturnData().team = getPlayerInfo().teamNumber;
  getGameReturnData().player = getPlayerInfo().playerNumber;
  getGameReturnData().message = returnMessage;
} // end execute


void GameController::handleButtons()
{
  if (getButtonState()[ButtonState::Chest] == ButtonEvent::CLICKED)
  {
    switch (getPlayerInfo().robotState)
    {
    case PlayerInfo::initial:
    case PlayerInfo::ready:
    case PlayerInfo::set:
    case PlayerInfo::playing:
    case PlayerInfo::finished:
    {
      getPlayerInfo().robotState = PlayerInfo::penalized;
      returnMessage = GameReturnData::manual_penalise;
      break;
    }
    case PlayerInfo::penalized:
    {
      getPlayerInfo().robotState = PlayerInfo::playing;
      returnMessage = GameReturnData::manual_unpenalise;
      break;
    }
    default:
      ASSERT(false);
    }
  }

  // re-set team color or kickoff in initial
  if (getPlayerInfo().robotState == PlayerInfo::initial)
  {
    if ( getButtonState()[ButtonState::LeftFoot] == ButtonEvent::PRESSED )
    {
      // switch team color
      GameData::TeamColor oldColor = getPlayerInfo().teamColor;
      if (oldColor == GameData::blue) {
        getPlayerInfo().teamColor = GameData::red;
      } else if (oldColor == GameData::red) {
        getPlayerInfo().teamColor = GameData::yellow;
      } else if (oldColor == GameData::yellow) {
        getPlayerInfo().teamColor = GameData::black;
      } else if (oldColor == GameData::black) {
        getPlayerInfo().teamColor = GameData::blue;
      } else { // set to red by default
        getPlayerInfo().teamColor = GameData::red;
      }
    }

    if ( getButtonState()[ButtonState::RightFoot] == ButtonEvent::PRESSED )
    {
      // switch kick off team
      getPlayerInfo().kickoff = !getPlayerInfo().kickoff;
    }
  }

  // go back from penalized to initial both foot bumpers are pressed for more than 1s
  else if (getPlayerInfo().robotState == PlayerInfo::penalized &&
    (  getButtonState()[ButtonState::LeftFoot].isPressed && 
       getFrameInfo().getTimeSince(getButtonState()[ButtonState::LeftFoot].timeOfLastEvent) > 1000 )
    &&
    (  getButtonState()[ButtonState::RightFoot].isPressed && 
       getFrameInfo().getTimeSince(getButtonState()[ButtonState::RightFoot].timeOfLastEvent) > 1000 )
    )
  {
    getPlayerInfo().robotState = PlayerInfo::initial;
    returnMessage = GameReturnData::manual_unpenalise;
  }

} // end handleButtons


void GameController::handleHeadButtons()
{
  getSoundPlayData().mute = true;
  getSoundPlayData().soundFile = "";


  if(   getButtonState().buttons[ButtonState::HeadMiddle] == ButtonEvent::CLICKED
     && getPlayerInfo().robotState == PlayerInfo::initial)
  {
    int playerNumber = getPlayerInfo().playerNumber;
    if(playerNumber <= 9)
    {
      std::stringstream ssWav;
      ssWav << playerNumber;
      ssWav << ".wav";
      getSoundPlayData().mute = false;
      getSoundPlayData().soundFile = ssWav.str();
    }
  }
}

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

  for(unsigned int i=LEDData::HeadFrontLeft0; i <= LEDData::HeadRearRight2; i++)
  {
    getGameControllerLEDRequest().request.theMonoLED[i] = 0.0;
  }

  // show game state in torso
  switch (getPlayerInfo().robotState)
  {
    case PlayerInfo::ready:
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 1.0;
      break;
    case PlayerInfo::set:
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 1.0;
      getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
      break;
    case PlayerInfo::playing:
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 1.0;
      break;
    case PlayerInfo::penalized:
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
      break;
    default:
      break;
  }

  // show team color on left foot
  if (getPlayerInfo().teamColor == GameData::red)
  {
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::RED] = 0.3;
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::BLUE] = 0.1;
  }
  else if (getPlayerInfo().teamColor == GameData::blue)
  {
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::BLUE] = 1.0;
  }
  else if(getPlayerInfo().teamColor == GameData::yellow)
  {
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::RED] = 1.0;
    getGameControllerLEDRequest().request.theMultiLED[LEDData::FootLeft][LEDData::GREEN] = 1.0;
  }
  else if(getPlayerInfo().teamColor == GameData::black)
  {
      // LED off
  }

  // show kickoff state on right foot and head in initial, ready and set
  if (getPlayerInfo().robotState == PlayerInfo::initial
    || getPlayerInfo().robotState == PlayerInfo::ready
    || getPlayerInfo().robotState == PlayerInfo::set)
  {
    if (getPlayerInfo().kickoff)
    {
      getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::RED] = 0.7;
      getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::GREEN] = 1.0;
      getGameControllerLEDRequest().request.theMultiLED[LEDData::FootRight][LEDData::BLUE] = 1.0;

      for(unsigned int i=LEDData::HeadFrontLeft0; i <= LEDData::HeadRearRight2; i++)
      {
        getGameControllerLEDRequest().request.theMonoLED[i] = 1.0;
      }
    }
  }
} // end updateLEDs

GameController::~GameController()
{
}
