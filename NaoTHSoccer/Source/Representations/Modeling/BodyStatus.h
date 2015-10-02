/**
* @file BodyStatus.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter Benjamin</a>
* Definition of class BodyStatus
*/

#ifndef __BodyStatus_h_
#define __BodyStatus_h_

#include "Tools/DataStructures/Printable.h"

class BodyStatus : public naoth::Printable
{
public:
  BodyStatus():
  currentSum(),
  currentSumFiltered()
  {}

  ~BodyStatus(){}

  double currentSum[JointData::numOfJoint];
  double currentSumFiltered[JointData::numOfJoint];

  virtual void print(std::ostream& stream) const
  {
    for(int i=0; i < JointData::numOfJoint; i++)
    {
      stream << JointData::getJointName((JointData::JointID) i) << " " << currentSum[i] << std::endl;
      stream << JointData::getJointName((JointData::JointID) i) << " Filtered" << currentSumFiltered[i] << std::endl;
    }
   
  }//end print
};

#endif// __BodyStatus_h_
