/** 
* @file GoalFeaturePercept.h
*
* Declaration of class GoalFeaturePercept
*/

#ifndef _GoalFeaturePercept_h_
#define _GoalFeaturePercept_h_

#include <vector>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/ColorClasses.h"
#include <Tools/DataStructures/Printable.h>
#include "Tools/ImageProcessing/GoalBarFeature.h"

class GoalFeaturePercept : public naoth::Printable
{ 
public:
  GoalFeaturePercept() {}

  std::vector<std::vector<GoalBarFeature> > features;

  void reset(size_t length) {
    features.resize(length);
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "feature count: " << features.size() << std::endl;
  }//end print

private:
};//end GoalFeaturePercept

class GoalFeaturePerceptTop : public GoalFeaturePercept
{
public:
  virtual ~GoalFeaturePerceptTop() {}
};

#endif // _GoalFeaturePercept_h_

