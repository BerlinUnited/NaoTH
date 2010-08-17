/**
 * @file SimSparkGameInfo.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief the game information in SimSpark
 */

#include "SimSparkGameInfo.h"
#include <iostream>

using namespace std;

#define STR_TI_LEFT "left"
#define STR_TI_RIGHT "right"
#define STR_TI_NULL "unknown"

SimSparkGameInfo::TeamIndex SimSparkGameInfo::getTeamIndexByName(const string& name)
    {
        if (name == STR_TI_LEFT)
            return TI_LEFT;
        if (name == STR_TI_RIGHT)
            return TI_RIGHT;
        return TI_NULL;
}

string SimSparkGameInfo::getTeamIndexName(SimSparkGameInfo::TeamIndex ti)
{
  switch (ti) {
  case TI_LEFT:
    return STR_TI_LEFT;
  case TI_RIGHT:
    return STR_TI_RIGHT;
  default:
    return STR_TI_NULL;
  }
}

SimSparkGameInfo::SimSparkGameInfo() :
theGameTime(0),
//thePlayMode(PlayerInfo::numOfPlayMode),
thePlayerNum(0),
theTeamIndex(TI_NULL),
halfTime(5 * 60) // 5 minutes
{
}

SimSparkGameInfo::~SimSparkGameInfo()
{
}

// Example message: "(GS (t 0.00) (pm BeforeKickOff))"
bool SimSparkGameInfo::update(const sexp_t* sexp)
{
  bool ok = true;
  string name;
  while (sexp)
  {
    const sexp_t* t = sexp->list;
    if (SexpParser::parseValue(t, name))
    {
      if ("t" == name) // time
      {
        if (!SexpParser::parseValue(t->next, theGameTime))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get time value\n";
        }
      } else if ("pm" == name) // play mode
      {
        std::string pm;
        if (!SexpParser::parseValue(t->next, pm))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get play mode value\n";
        }
//        thePlayMode = PlayerInfo::getPlayModeByName(pm);
      } else if ("unum" == name) // unum
      {
        if (!SexpParser::parseValue(t->next, thePlayerNum))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get unum value\n";
        }
      } else if ("team" == name) // side
      {
        string team;
        if (!SexpParser::parseValue(t->next, team))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get team index value\n";
        }
        theTeamIndex = getTeamIndexByName(team);
      } else
      {
        ok = false;
        cerr << "SimSparkGameInfo::update unknown name: " << name << '\n';
      }
    } else
    {
      ok = false;
      cerr << "SimSparkGameInfo::update can not get the name!\n";
    }
    sexp = sexp->next;
  }

  return ok;
}
