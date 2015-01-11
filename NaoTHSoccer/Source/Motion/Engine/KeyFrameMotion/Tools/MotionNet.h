/**
* @file MotionNet.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class MotionNet
*/

#ifndef __MotionNet_h_
#define __MotionNet_h_

#include <vector>
#include <Representations/Infrastructure/JointData.h>

class MotionNet
{
public:

  MotionNet();
  ~MotionNet();

  class KeyFrame
  {
    public:
      KeyFrame(): 
        id(-1),
        jointData() // initialize the array to 0.0 by default
      {}
      ~KeyFrame(){}

      int id;
      double jointData[naoth::JointData::numOfJoint];
  };

  class Transition
  {
    public:
      Transition(): duration(0.0), fromKeyFrame(-1), toKeyFrame(-1){}
      ~Transition(){}

      double duration;
      std::string condition;
      int fromKeyFrame;
      int toKeyFrame;
  };

protected:
  naoth::JointData::JointID jointIndex[naoth::JointData::numOfJoint];
  int numberOfJoints; // number of used Joints

  std::vector<KeyFrame> keyFrameArray;

  std::vector<Transition> transitionArray;

public:
  const KeyFrame& getKeyFrame(int id) const;
  KeyFrame& getMutableKeyFrame(int id);
  const Transition& getTransition(int i) const { return transitionArray[i]; }

  void add(const naoth::JointData::JointID& id);
  void add(const KeyFrame& keyFrame);
  void add(const Transition& transition);

  naoth::JointData::JointID getJointID(int i) const { return jointIndex[i]; }
  int getNumOfJoints() const { return numberOfJoints; }
  size_t getNumOfKeyFrame() const { return keyFrameArray.size(); }
  size_t getNumOfTransition() const { return transitionArray.size(); }
  bool isEmpty() const { return keyFrameArray.empty(); }

  friend std::ostream& operator<<(std::ostream& stream, const MotionNet& motionNet);
};

/**
 * Streaming operator that writes a MotionNet to a stream.
 * @param stream The stream to write on.
 * @param image The MotionNet object.
 * @return The stream.
 */ 
std::ostream& operator<<(std::ostream& stream, const MotionNet& motionNet);
std::ostream& operator<<(std::ostream& stream, const MotionNet::KeyFrame& keyFrame);
std::ostream& operator<<(std::ostream& stream, const MotionNet::Transition& transition);

#endif //__MotionNet_h_
