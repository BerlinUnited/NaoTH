/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef __StepBuffer_h_
#define __StepBuffer_h_

#include "Tools/DataStructures/Printable.h"
#include "Representations/Motion/Request/WalkRequest.h"
#include <list>

#include "Motion/Engine/InverseKinematicsMotion/Motions/Walk2018/FootStep.h"

#include <string>

class Step
{
private:
  unsigned int _id;

public:

  enum StepType {
    STEP_WALK,
    STEP_CONTROL
  };

  Step(unsigned int _id)
    :
    _id(_id),

    planningCycle(0),
    executingCycle(0),

    type(STEP_WALK)
  {}

  FootStep footStep;

  // step duration
  int numberOfCycles;

  // running parameters indicating how far the step is processed
  int planningCycle;
  int executingCycle;

  StepType type;
  // store the request which has been used to create this step
  WalkRequest walkRequest;

  unsigned int id() const { return _id; }
  bool isPlanned() const { return planningCycle >= numberOfCycles; }
  bool isExecuted() const { return executingCycle >= numberOfCycles; }
};

class StepBuffer : public naoth::Printable
{
private:
  unsigned int id;
  std::list<Step> steps;

public:
  StepBuffer() : id(0) {}

  inline Step& add() {
    steps.push_back(Step(id++));
    return steps.back();
  }

  // deligated accessors
  inline const Step& first() const { return steps.front(); }
  inline Step& first() { return steps.front(); }
  inline const Step& last() const { return steps.back(); }
  inline Step& last() { return steps.back(); }
  inline bool empty() const { return steps.empty(); }
  inline void pop() { return steps.pop_front(); }
  inline unsigned int stepId() const { return id; }

  void draw(DrawingCanvas2D& canvas) const
  {
    for(std::list<Step>::const_iterator i = steps.begin(); i != steps.end(); ++i) {
      i->footStep.draw(canvas);
    }
  }

  virtual void print(std::ostream& /*stream*/) const
  {
  }
};

//namespace naoth
//{
//  template<>
//  class Serializer<StepBuffer>
//  {
//  public:
//    static void serialize(const StepBuffer& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, StepBuffer& representation);
//  };
//}

#endif // __StepBuffer_h_
