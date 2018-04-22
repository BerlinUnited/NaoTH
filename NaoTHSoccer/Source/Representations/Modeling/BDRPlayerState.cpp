
#include "BDRPlayerState.h"


#include "Messages/BDRMessages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;
/*
void Serializer<BDRPlayerState>::serialize(const BDRPlayerState& representation, std::ostream& stream)
{
  naothmessages::BDRPlayerState p;

  p.set_behaviormode((naothmessages::BDRBehaviorMode)representation.behaviorMode);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<BDRPlayerState>::deserialize(std::istream& stream, BDRPlayerState& representation)
{
  naothmessages::BDRPlayerState p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  if(p.has_behaviormode()) {
    representation.behaviorMode = (BDRPlayerState::BehaviorMode)p.behaviormode();
  }
}//end deserialize
*/

std::string BDRPlayerState::getActivityName(Activity id)
{
  switch(id)
  {
    case pre_playing      : return "pre_playing";
    case playing          : return "playing";
    case pre_entertaining : return "pre_entertaining";
    case entertaining     : return "entertaining";
    case pre_servicing    : return "pre_servicing";
    case servicing        : return "servicing";
    case doing_nothing    : return "doing_nothing";
    case initializing     : return "initializing";
    default : ASSERT(false);
  }
  return "unknown activity";
}

std::string BDRPlayerState::getMessageName(Message m){
  switch(m)
  {
    case charging                  : return "charging";
    case failed_charging           : return "failed_charging";
    case fallen                    : return "fallen";
    case ready                     : return "ready";
    case too_many_players_on_field : return "too_many_players_on_field";
    case cooling_down_needed       : return "cooling_down_needed";
    case charging_needed           : return "charging_needed";
    case lets_play                 : return "lets_play";
    case won                       : return "won";
    case lost                      : return "lost";
    default : ASSERT(false);
  }
  return "unknown message name";
}

std::string BDRPlayerState::getMessageString(Message m){
  switch(m)
  {
    case charging                  : return "Der Akku wird geladen";
    case failed_charging           : return "Es konnte kein Kontakt zum Ladegerät hergestellt werden";
    case fallen                    : return "Ich bin hingefallen";
    case ready                     : return "Ich bin bereit";
    case too_many_players_on_field : return "Es sind zu viele Roboter auf dem Feld";
    case cooling_down_needed       : return "Ich muss meine Knochen schonen";
    case charging_needed           : return "Ich muss meinen Akku laden";
    case lets_play                 : return "Los gehts";
    case won                       : return "Ich habe gewonnen";
    case lost                      : return "Oh nein";
    default : ASSERT(false);
  }
  return "unknown message";
}

std::string BDRPlayerState::getReasonName(Reason id)
{
  switch(id)
  {
    case none           : return "none";
    case temperature    : return "temperature";
    case battery_charge : return "battery_charge";
    default : ASSERT(false);
  }
  return "unknown activity";
}
