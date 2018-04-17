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
    : activity(doing_nothing)
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

  bool sitting;
  bool localized_on_field;

  virtual void print(std::ostream& stream) const
  {
    stream << "BDRPlayerState\n";
    stream << "  activity: " << getActivityName(activity) << "\n";
    stream << "  sitting: " << sitting << "\n";
    stream << "  localized_on_field: " << localized_on_field << "\n";
  }

  static std::string getActivityName(Activity id);
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
