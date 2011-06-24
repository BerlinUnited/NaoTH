/**
 * @file GameInfo.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief the game information in RoboCup
 */

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <string>

#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/Configuration.h"


namespace naoth
{

class GameData : public PlatformInterchangeable, public Printable
{
public:
  enum GameState
  {
    initial,
    ready,
    set,
    playing,
    penalized,
    finished,
    unknown,
    numOfGameState
  };

  static GameState gameStateFromString(const std::string& name);
  static std::string gameStateToString(GameState gameState);

  enum PlayMode
  {
    before_kick_off,
    kick_off_own,
    kick_off_opp,
    play_on,
    kick_in_own,
    kick_in_opp,
    corner_kick_own,
    corner_kick_opp,
    goal_kick_own,
    goal_kick_opp,
    offside_own,
    offside_opp,
    game_over,
    goal_own,
    goal_opp,
    free_kick_own,
    free_kick_opp,
    numOfPlayMode
  };

  static PlayMode playModeFromString(const std::string& name);
  static std::string playModeToString(PlayMode mode);

  enum TeamColor
  {
    red,
    blue,
    numOfTeamColor
  };

  static TeamColor teamColorFromString(const std::string& teamColor);
  static std::string teamColorToString(TeamColor teamColor);

  virtual void print(ostream& stream) const;
  void loadFromCfg(Configuration& config);

  GameData();

public:
  unsigned int frameNumber; // indicates when it is updated

  GameState gameState;
  unsigned int timeWhenGameStateChanged;

  PlayMode playMode;
  unsigned int gameTime; // in ms
  unsigned int timeWhenPlayModeChanged; // ms

  // static information
  unsigned int playerNumber;
  TeamColor teamColor;
  unsigned int teamNumber;
  std::string teamName;
  double halfTime;
  unsigned int numOfPlayers;
};

}// namespace naoth

/**
 * the Operator ! inverts the team color, i.e.,
 * !red = blue, !blue = red, !unknownTeamColor = unknownTeamColor
 * e.g., the color of the opponent team is !thePlayerInfo.teamColor
 */
naoth::GameData::TeamColor operator! (const naoth::GameData::TeamColor& color);

#endif // GAMEDATA_H
