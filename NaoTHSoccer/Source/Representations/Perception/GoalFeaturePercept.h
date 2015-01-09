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
#include "Tools/ImageProcessing/Edgel.h"

class GoalFeaturePercept : public naoth::Printable
{ 
public:
  GoalFeaturePercept() {}

  class Feature
  {
  public:
    Vector2i begin;
    Vector2i center;
    Vector2i end;

    Vector2d responseAtBegin;
    Vector2d responseAtEnd;

    bool possibleObstacle;

    Feature()
    :
      begin(-1,-1),
      center(-1,-1),
      end(-1, -1),
      responseAtBegin(0.0, 0.0),
      responseAtEnd(0.0, 0.0),
      possibleObstacle(false)
    {}
  };

  //NOTE: experimental
  std::vector<std::vector<EdgelT<double> > > edgel_features;

  //
  std::vector<std::vector<Feature> > features;

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

