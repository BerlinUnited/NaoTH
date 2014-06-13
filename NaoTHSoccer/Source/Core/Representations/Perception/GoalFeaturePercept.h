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
  GoalFeaturePercept(){}

  class Feature
  {
  public:
    Vector2i begin;
    Vector2i center;
    Vector2i end;

    Vector2d responseAtBegin;
    Vector2d responseAtEnd;

    //double width;

    bool possibleObstacle;
    //bool used;

    Feature()
    :
      begin(-1,-1),
      center(-1,-1),
      end(-1, -1),
      responseAtBegin(0.0, 0.0),
      responseAtEnd(0.0, 0.0),
      //width(0.0),
      possibleObstacle(false)//,
      //used(false)
    {

    }
  };

  //NOTE: experimental
  std::vector<std::vector<EdgelT<double> > > edgel_features;

  //
  std::vector<std::vector<Feature> > features;
  Vector2d horizonNormal; 

  void reset(size_t length)
  {
    features.resize(length);
  }

  virtual void print(std::ostream& stream) const
  {
    //stream << "angleToSeenGoal=" << angleToSeenGoal << std::endl;
    //stream << "goalCentroid=" << goalCentroid << std::endl;
    //stream << "horizonScan=" << (horizonScan?"true":"false") << std::endl;
    //
    //for(unsigned int n=0; n<numberOfSeenPosts; n++)
    //{
    //  stream << "=====Post " << n << "=====" << std::endl;
    //  stream << "post type= " << GoalPost::getPostTypeName(post[n].type) << std::endl;
    //  stream << "basePoint=(" << post[n].basePoint.x << ";" << post[n].basePoint.y << ")" << std::endl;
    //  stream << "position=(" << post[n].position.x << ";" << post[n].position.y << ")" << std::endl;
    //  stream << "color=" << ColorClasses::getColorName(post[n].color) << std::endl;
    //  stream << "reliable=" << (post[n].positionReliable? "true" : "false") << std::endl;
    //}//end for
  }//end print


  ////////////////////////////////////////
  //// getters
  //int getNumberOfSeenPosts() const { return (int)numberOfSeenPosts; }
  //const GoalPost& getPost(unsigned int i) const { ASSERT(i < numberOfSeenPosts); return post[i]; }
  //GoalPost& getPost(unsigned int i) { ASSERT(i < numberOfSeenPosts); return post[i]; }

private:
};//end GoalFeaturePercept

class GoalFeaturePerceptTop : public GoalFeaturePercept
{
public:
  virtual ~GoalFeaturePerceptTop() {}
};

#endif // _GoalFeaturePercept_h_

