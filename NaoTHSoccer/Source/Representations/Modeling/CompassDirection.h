/**
* @file CompassDirection.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class CompassDirection
*/

#ifndef _CompassDirection_h_
#define _CompassDirection_h_

#include <Tools/DataStructures/Printable.h>

class CompassDirection : public naoth::Printable
{

public:
    CompassDirection() : angle(0.0) {}
    virtual ~CompassDirection(){}

    double angle;

    virtual void print(std::ostream& stream) const
    {
      stream << "angle = " << angle << std::endl;
    }//end print

};//end class CompassDirection

#endif // __CompassDirection_h_
