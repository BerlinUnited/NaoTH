/**
 * @file SimSparkGameInfo.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief the game information in SimSpark
 */

#ifndef _SIMSPARK_GAME_INFO_H
#define	_SIMSPARK_GAME_INFO_H

#include "sfsexp/SexpParser.h"
//#include "Representations/Modeling/PlayerInfo.h"

class SimSparkGameInfo
{
public:
  SimSparkGameInfo();

  ~SimSparkGameInfo();

  bool update(const sexp_t* sexp);

  enum TeamIndex
  {
    TI_LEFT = 0,
    TI_RIGHT,
    TI_NULL /*< this must be the last entry >*/
  };

  static TeamIndex getTeamIndexByName(const std::string& name);
  static std::string getTeamIndexName(TeamIndex ti);

public:
  double theGameTime;

  //PlayerInfo::PlayMode thePlayMode;

  /** information of robot player */
  unsigned int thePlayerNum;
  TeamIndex theTeamIndex;

  // static information
  double halfTime;
};

#endif	/* _SIMSPARK_GAME_INFO_H */

