
#include "KickActionModel.h"

std::string KickActionModel::getName(ActionId id)
{
  switch(id)
  {
    case none: return "none";
    case kick_short: return "kick_short";
    case kick_long: return "kick_long";
    case dribble_kick: return "dribble_kick";
    case sidekick_left: return "sidekick_left";
    case sidekick_right: return "sidekick_right";
    default: return "unknown action";
  }
}
/*
double KickActionModel::getRotation(){
  return rotation;
}
*/