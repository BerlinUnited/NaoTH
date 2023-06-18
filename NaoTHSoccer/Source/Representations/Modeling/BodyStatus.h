/**
* @file BodyStatus.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter Stella Alice</a>
* Definition of class BodyStatus
*/

#ifndef __BodyStatus_h_
#define __BodyStatus_h_

#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/JointData.h"

class BodyStatus : public naoth::Printable
{
public:
  BodyStatus():
    currentSum(),
    timestamp(0)
  {}

  ~BodyStatus(){}

  double currentSum[naoth::JointData::numOfJoint];
  unsigned int timestamp;

  virtual void print(std::ostream& stream) const
  {
    for(int i=0; i < naoth::JointData::numOfJoint; i++)
    {
      stream << naoth::JointData::getJointName((naoth::JointData::JointID) i) << " " << currentSum[i] << std::endl;
    }
  }
};

  namespace naoth
  {
    template<>
    class Serializer<BodyStatus>
    {
    public:
      static void serialize(const BodyStatus& representation, std::ostream& stream);
      static void deserialize(std::istream& stream, BodyStatus& representation);
    };
  }
#endif// __BodyStatus_h_
