#include "GameController.h"

#include "Tools/Debug/DebugRequest.h"
#include <PlatformInterface/Platform.h>

GameController::GameController()
  : 
  debug_whistle_heard(false),
  play_by_whistle(false)
{
  DEBUG_REQUEST_REGISTER("gamecontroller:game_state:play", "force the play state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:game_state:penalized", "force the penalized state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:game_state:initial", "force the initial state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:game_state:ready", "force the ready state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:game_state:set", "force the set state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:game_state:finished", "force the finished state", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set_play:none", "force the setPlay state to none", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set_play:goal_free_kick", "force the setPlay state to goal free kick", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set_play:pushing_free_kick", "force the setPlay state to pushing free kick", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set_play:corner_kick", "force the setPlay state to corner kick", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:set_play:kick_in", "force the setPlay state to kick-in", false);

  DEBUG_REQUEST_REGISTER("gamecontroller:gamephase:normal", "force the gamephase", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:gamephase:penaltyshoot", "force the gamephase", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:gamephase:overtime", "force the gamephase", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:gamephase:timeout", "force the gamephase", false);

  DEBUG_REQUEST_REGISTER("gamecontroller:kickoff:own", "forces the kickoff to be ours", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:kickoff:opp", "forces the kickoff to be opponents", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:secondaryTime:30", "sets the secondary time of the gamecontroller to 30s", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:secondaryTime:20", "sets the secondary time of the gamecontroller to 20s", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:secondaryTime:10", "sets the secondary time of the gamecontroller to 10s", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:secondaryTime:5", "sets the secondary time of the gamecontroller to 5s", false);
  DEBUG_REQUEST_REGISTER("gamecontroller:secondaryTime:0", "sets the secondary time of the gamecontroller to 0s", false);

  DEBUG_REQUEST_REGISTER("gamecontroller:blow_whistle", "the robot recognizes a whistle", false);

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

  // use the team configuration if avaliable
  const std::string& name = naoth::Platform::getInstance().theRobotName;
  if (config.hasKey("team", name)) {
    getPlayerInfo().playerNumber = config.getInt("team", name);
  }

  if (config.hasKey("player", "InitialState")) {
    const auto state  = config.getString("player", "InitialState");
    const auto states = {PlayerInfo::initial,  PlayerInfo::ready,
                         PlayerInfo::set,      PlayerInfo::playing,
                         PlayerInfo::finished, PlayerInfo::penalized,
                         PlayerInfo::unstiff};
    for (const auto& s : states) {
      if (state == PlayerInfo::toString(s)) {
          getPlayerInfo().robotState = s;
      }
    }
  }

  // print out the "final" player number for loggin purposes
  std::cout << "[PlayerInfo] " << "playerNumber: " << getPlayerInfo().playerNumber << std::endl;
}

void GameController::execute()
{
  PlayerInfo::RobotState oldRobotState = getPlayerInfo().robotState;
  GameData::TeamColor oldTeamColor = getPlayerInfo().teamColor;

  // reset the whistle state if the game state is not play anymore
  if(getPlayerInfo().robotState != PlayerInfo::playing) {
    play_by_whistle = false;
  }

  // try update from the game controller message if not manually overwritten
  if ( getGameData().valid && getWifiMode().wifiEnabled ) 
  {
    // HACK: needed by SimSpark - overide the player number
    if(getGameData().newPlayerNumber > 0) {
      getPlayerInfo().playerNumber = getGameData().newPlayerNumber;
    }

    // ignore information from game controller when unstiff
    if (getPlayerInfo().robotState != PlayerInfo::unstiff) {
      getPlayerInfo().update(getGameData());
    }

    // take the ownership of the play state
    if(getPlayerInfo().robotState == PlayerInfo::playing) {
      play_by_whistle = false;
    }
  }

  handleButtons();
  handleHeadButtons();
  handleDebugRequest();

  // whistle overrides state when in set
  if(getPlayerInfo().robotState == PlayerInfo::set)
  {
    // switch from set to play
    if(getWhistlePercept().whistleDetected || debug_whistle_heard || play_by_whistle) {
      getPlayerInfo().robotState = PlayerInfo::playing;
      play_by_whistle = true;
    }
  }

  if(  oldRobotState != getPlayerInfo().robotState
    || oldTeamColor  != getPlayerInfo().teamColor
    || getPlayerInfo().robotState == PlayerInfo::initial
    || getPlayerInfo().robotState == PlayerInfo::unstiff)
  {
    updateLEDs();
  }

  // provide the return message
  getGameReturnData().teamNum = getPlayerInfo().teamNumber;
  getGameReturnData().playerNum = getPlayerInfo().playerNumber;
  // TODO: this is not correctly set
  getGameReturnData().fallen = getWifiMode().wifiEnabled ? GameReturnData::ROBOT_CAN_PLAY : GameReturnData::ROBOT_FALLEN;
} // end execute


void GameController::handleDebugRequest()
{
  PlayerInfo::RobotState debugState = getPlayerInfo().robotState;

  // DebugRequests for the game state
  DEBUG_REQUEST("gamecontroller:game_state:initial",
    debugState = PlayerInfo::initial;
  );
  DEBUG_REQUEST("gamecontroller:game_state:ready",
    debugState = PlayerInfo::ready;
  );
  DEBUG_REQUEST("gamecontroller:game_state:set",
    debugState = PlayerInfo::set;
  );
  DEBUG_REQUEST("gamecontroller:game_state:play",
    debugState = PlayerInfo::playing;
    // take the ownership of the play state
    play_by_whistle = false;
  );
  DEBUG_REQUEST("gamecontroller:game_state:penalized",
    debugState = PlayerInfo::penalized;
  );
  DEBUG_REQUEST("gamecontroller:game_state:finished",
    debugState = PlayerInfo::finished;
  );

  // DebugRequests for the set play state (free kicks)
  DEBUG_REQUEST("gamecontroller:set_play:none",
    getPlayerInfo().robotSetPlay = PlayerInfo::set_none;
  );
  DEBUG_REQUEST("gamecontroller:set_play:goal_free_kick",
    getPlayerInfo().robotSetPlay = PlayerInfo::goal_free_kick;
  );
  DEBUG_REQUEST("gamecontroller:set_play:pushing_free_kick",
    getPlayerInfo().robotSetPlay = PlayerInfo::pushing_free_kick;
  );
  DEBUG_REQUEST("gamecontroller:set_play:corner_kick",
    getPlayerInfo().robotSetPlay = PlayerInfo::corner_kick;
  );
  DEBUG_REQUEST("gamecontroller:set_play:kick_in",
    getPlayerInfo().robotSetPlay = PlayerInfo::kick_in;
  );

  DEBUG_REQUEST("gamecontroller:gamephase:normal",
    getPlayerInfo().gamePhase = PlayerInfo::normal;
  );
  DEBUG_REQUEST("gamecontroller:gamephase:penaltyshoot",
    getPlayerInfo().gamePhase = PlayerInfo::penaltyshoot;
  );
  DEBUG_REQUEST("gamecontroller:gamephase:overtime",
    getPlayerInfo().gamePhase = PlayerInfo::overtime;
  );
  DEBUG_REQUEST("gamecontroller:gamephase:timeout",
    getPlayerInfo().gamePhase = PlayerInfo::timeout;
  );

  // DebugRequests for the kickoff state
  DEBUG_REQUEST("gamecontroller:kickoff:opp",
    getPlayerInfo().kickoff = false;
  );
  DEBUG_REQUEST("gamecontroller:kickoff:own",
    getPlayerInfo().kickoff = true;
  );

  // DebugRequests for the secondary game time (eg. free kick)
  DEBUG_REQUEST("gamecontroller:secondaryTime:30", getGameData().secondaryTime = 30;);
  DEBUG_REQUEST("gamecontroller:secondaryTime:20", getGameData().secondaryTime = 20;);
  DEBUG_REQUEST("gamecontroller:secondaryTime:10", getGameData().secondaryTime = 10;);
  DEBUG_REQUEST("gamecontroller:secondaryTime:5", getGameData().secondaryTime = 5;);
  DEBUG_REQUEST("gamecontroller:secondaryTime:0", getGameData().secondaryTime = 0;);

  debug_whistle_heard = false;
  DEBUG_REQUEST("gamecontroller:blow_whistle",
    // kinda "hack": we don't increment the whistle counter, instead ...
    debug_whistle_heard = true;
  );

  // NOTE: same behavior as the button interface
  if(debugState != getPlayerInfo().robotState) {
    getPlayerInfo().robotState = debugState;
  }

} // end handleDebugRequest


void GameController::handleButtons()
{

  if (getButtonState()[ButtonState::Chest].isSingleClick())
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
      break;
    }
    case PlayerInfo::penalized:
    {
      getPlayerInfo().robotState = PlayerInfo::playing;
      // take the ownership of the play state
      play_by_whistle = false;
      break;
    }
    case PlayerInfo::unstiff:
    {
      getPlayerInfo().robotState = PlayerInfo::initial;
      break;
    }
    default:
      ASSERT(false);
    }
  }

  // re-set team color or kickoff in initial
  if (getPlayerInfo().robotState == PlayerInfo::initial)
  {
    if (getButtonState()[ButtonState::LeftFootLeft] == ButtonEvent::PRESSED ||
        getButtonState()[ButtonState::LeftFootRight] == ButtonEvent::PRESSED)
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

    if (getButtonState()[ButtonState::RightFootLeft] == ButtonEvent::PRESSED ||
        getButtonState()[ButtonState::RightFootRight] == ButtonEvent::PRESSED)
    {
      // switch kick off team
      getPlayerInfo().kickoff = !getPlayerInfo().kickoff;
    }
  }

  // go back from penalized to initial both foot bumpers are pressed for more than 1s
  else if (getPlayerInfo().robotState == PlayerInfo::penalized && ( 
     (getButtonState()[ButtonState::LeftFootLeft].isPressed && getButtonState()[ButtonState::LeftFootLeft].timeSinceEvent() > 1000) || 
     (getButtonState()[ButtonState::LeftFootRight].isPressed && getButtonState()[ButtonState::LeftFootRight].timeSinceEvent() > 1000 )
    )
    && (
     (getButtonState()[ButtonState::RightFootLeft].isPressed && getButtonState()[ButtonState::RightFootLeft].timeSinceEvent() > 1000) ||
     (getButtonState()[ButtonState::RightFootRight].isPressed && getButtonState()[ButtonState::RightFootRight].timeSinceEvent() > 1000))
    )
  {
    getPlayerInfo().robotState = PlayerInfo::initial;
  }
} // end handleButtons


void GameController::handleHeadButtons()
{
  if(getButtonState().buttons[ButtonState::HeadMiddle] == ButtonEvent::CLICKED
    && (getPlayerInfo().robotState == PlayerInfo::initial || getPlayerInfo().robotState == PlayerInfo::finished))
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


  if((getButtonState().buttons[ButtonState::HeadFront].isPressed && getButtonState()[ButtonState::HeadFront].timeSinceEvent() > 1000) &&
    (getButtonState()[ButtonState::HeadMiddle].isPressed && getButtonState()[ButtonState::HeadMiddle].timeSinceEvent() > 1000) &&
    (getButtonState()[ButtonState::HeadRear].isPressed && getButtonState()[ButtonState::HeadRear].timeSinceEvent() > 1000)) {

    getPlayerInfo().robotState = PlayerInfo::unstiff;
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
    case PlayerInfo::unstiff:
      // handle blinking chest button for unstiff state
      if (getFrameInfo().getFrameNumber() % 8 < 4) {
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 1.0;
      }
      else {
        getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 0.0;
      }
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
