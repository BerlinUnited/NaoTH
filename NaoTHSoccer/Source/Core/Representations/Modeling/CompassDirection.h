/**
* @file GoalModel.h
*
* @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GoalModel
*/

#ifndef __CompassDirection_h_
#define __CompassDirection_h_

#include <Tools/DataStructures/Printable.h>

class CompassDirection : public naoth::Printable
{
public:
    CompassDirection(){}
    ~CompassDirection(){}

    double angle;

    virtual void print(ostream& stream) const
    {
      stream << "angle = " << angle<< endl;
    }//end print

};//end class CompassDirection

#endif // __CompassDirection_h_
