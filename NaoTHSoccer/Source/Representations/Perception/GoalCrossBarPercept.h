/** 
* @file GoalCrossBarPercept.h
*
* Declaration of class GoalCrossBarPercept
*/

#ifndef _GoalCrossBarPercept_h_
#define _GoalCrossBarPercept_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/ColorClasses.h"
#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

class GoalCrossBarPercept : public naoth::Printable
{ 
public:
  GoalCrossBarPercept() : numberOfSeenBars(0)
  {}

  class GoalCrossBar
  {
  public:
    /* the left posts index */
    int leftPostIdx;

    /* the right posts index */
    int rightPostIdx;

    /* indicates whether left and/or right post are connected to th crossbar */
    bool positionReliable;

    /*detected width of the crossbar*/
    double seenWidth;

    GoalCrossBar()
    :
      leftPostIdx(-1),
      rightPostIdx(-1),
      positionReliable(false),
      seenWidth(0)
    {}

  };//end class GoalCrossBar


public:
  /* maximum number of posts */
  const static unsigned int MAXNUMBEROFBARS = 6;


private:
  /* number of seen posts */
  unsigned int numberOfSeenBars;

  /* array of recognized posts in arbitrary order */
  GoalCrossBar crossBars[MAXNUMBEROFBARS];


public:

  void add(const GoalCrossBar& crossBar)
  {
    if(numberOfSeenBars < MAXNUMBEROFBARS)
    {
      crossBars[numberOfSeenBars] = crossBar;
      numberOfSeenBars++;
    }
  }


  void reset()
  {
    numberOfSeenBars = 0;
  }


  virtual void print(std::ostream& stream) const
  {
    
    for(unsigned int n=0; n<numberOfSeenBars; n++)
    {
      stream << "=====Crossbar " << n << "=====" << std::endl;
      stream << "leftPostIdx=" << crossBars[n].leftPostIdx << std::endl;
      stream << "rightPostIdx=(" << crossBars[n].rightPostIdx << std::endl;
      stream << "seenWidth=" << crossBars[n].seenWidth << std::endl;
      stream << "reliable=" << (crossBars[n].positionReliable? "true" : "false") << std::endl;
    }//end for
  }//end print


  //////////////////////////////////////
  // getters
  int getNumberOfSeenCrossBars() const { return (int)numberOfSeenBars; }
  const GoalCrossBar& getCrossBar(unsigned int i) const { ASSERT(i < numberOfSeenBars); return crossBars[i]; }
  GoalCrossBar& getCrossBar(unsigned int i) { ASSERT(i < numberOfSeenBars); return crossBars[i]; }

//private:
//  friend class naoth::Serializer<GoalCrossBarPercept>;
};//end GoalCrossBarPercept

class GoalCrossBarPerceptTop : public GoalCrossBarPercept
{
public:
  virtual ~GoalCrossBarPerceptTop() {}
};

//namespace naoth
//{
//  template<>
//  class Serializer<GoalCrossBarPercept>
//  {
//  public:
//    static void serialize(const GoalCrossBarPercept& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, GoalCrossBarPercept& representation);
//  };
//
//  template<>
//  class Serializer<GoalCrossBarPerceptTop> : public Serializer<GoalCrossBarPercept>
//  {};
//}

#endif // _GoalCrossBarPercept_h_

