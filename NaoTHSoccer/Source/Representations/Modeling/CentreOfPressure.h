/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef _CENTRE_OF_PRESSURE_H_
#define _CENTRE_OF_PRESSURE_H_

#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include <iomanip>

class CentreOfPressure : public naoth::Printable
{
public:
    CentreOfPressure(){}

    struct {
        bool valid;
        double magnitude;
        Vector3d CoP;
    } in_and_only_left_foot, in_and_only_right_foot, in_kinematic_chain_origin;

    virtual void print(std::ostream& stream) const
    {
        stream << std::fixed << std::setprecision(3);
        stream << "--- left foot ---" << std::endl;
        stream << "x: " << std::setw(8) << in_and_only_left_foot.CoP.x << std::endl;
        stream << "y: " << std::setw(8) << in_and_only_left_foot.CoP.y << std::endl;
        stream << "z: " << std::setw(8) << in_and_only_left_foot.CoP.z << std::endl;
        stream << "magnitude: " << std::setw(8) << in_and_only_left_foot.magnitude << std::endl;
        stream << "--- right foot ---" << std::endl;
        stream << "x: " << std::setw(8) << in_and_only_right_foot.CoP.x << std::endl;
        stream << "y: " << std::setw(8) << in_and_only_right_foot.CoP.y << std::endl;
        stream << "z: " << std::setw(8) << in_and_only_right_foot.CoP.z << std::endl;
        stream << "magnitude: " << std::setw(8) << in_and_only_right_foot.magnitude << std::endl;
        stream << "--- in kinematic chain origin (combined) --- " << std::endl;
        stream << "x: " << std::setw(8) << in_kinematic_chain_origin.CoP.x << std::endl;
        stream << "y: " << std::setw(8) << in_kinematic_chain_origin.CoP.y << std::endl;
        stream << "z: " << std::setw(8) << in_kinematic_chain_origin.CoP.z << std::endl;
        stream << "magnitude: " << std::setw(8) << in_kinematic_chain_origin.magnitude << std::endl;
    }
};

namespace naoth
{
  template<>
  class Serializer<CentreOfPressure>
  {
  public:
    static void serialize(const CentreOfPressure& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, CentreOfPressure& representation);
  };
}

#endif // _CENTRE_OF_PRESSURE_H_
