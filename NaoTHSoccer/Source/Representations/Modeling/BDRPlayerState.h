/**
 * @file BDRPlayerState.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _BDRPlayerState_H_
#define _BDRPlayerState_H_

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Representations/Infrastructure/FrameInfo.h>

class BDRPlayerState : public naoth::Printable
{
public:

  BDRPlayerState()
    : 
      activity(doing_nothing),
      message(ready),
      wartungs_reason(none),
      wartung_max_target(0),
      sitting(false),
      localized_on_field(false),
      time_playing(0),
      goalsLeft(0),
      goalsRight(0)
  {}

  enum Activity
  {
      pre_playing,
      playing,

      pre_entertaining,
      entertaining,

      pre_servicing,
      servicing,

      doing_nothing,
      initializing,

      num_of_activities
  } activity;

  enum Message
  {
      charging,
      failed_charging,
      fallen,
      ready,
      too_many_players_on_field,
      cooling_down_needed,
      charging_needed,
      lets_play,
      won,
      lost,
      num_of_messages
  } message;

  enum Reason
  {
      temperature,
      battery_charge,
      none,
      num_of_reasons
  } wartungs_reason;

  double wartung_max_target;

  bool sitting;
  bool localized_on_field;

  double time_playing;
  int goalsLeft;
  int goalsRight;

  virtual void print(std::ostream& stream) const
  {
    stream << "BDRPlayerState\n";
    stream << "  activity: " << getActivityName(activity) << "\n";
    stream << "  message: "  << getMessageString(message) << "\n";
    stream << "  sitting: "  << sitting << "\n";
    stream << "  localized_on_field: " << localized_on_field << "\n";
    stream << "  playing time: " << time_playing << "\n";
    stream << "  goalsLeft: " << goalsLeft << "\n";
    stream << "  goalsRight: " << goalsRight << "\n";
    stream << "  service reason: " << wartungs_reason << "\n";
    stream << "  service max target: " << wartung_max_target << "\n";
  }

  static std::string getActivityName(Activity id);

  static std::string getMessageName(Message m);
  static std::string getMessageString(Message m);

  inline std::string getMessageName() const { return getMessageName(message); }
  inline std::string getMessageString() const { return getMessageString(message); }

  static std::string getReasonName(Reason m);
};

/*
namespace naoth
{
  template<>
  class Serializer<BDRPlayerState>
  {
  public:
    static void serialize(const BDRPlayerState& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BDRPlayerState& representation);
  };
}
*/
#endif // _BDRPlayerState_H_
