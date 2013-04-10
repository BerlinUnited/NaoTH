/* 
 * File:   FSRData.h
 * Author: Oliver Welter
 * eMail: welter@informatik.hu-berlin.de
 *
 * Created on 31. Dezember 2008, 18:20
 */

#ifndef _FSRData_H_
#define _FSRData_H_

#include <string>

#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{

  class FSRData : public Printable
  {
  public:
    enum FSRID
    {
      LFsrFL,
      LFsrFR,
      LFsrBL,
      LFsrBR,
      RFsrFL,
      RFsrFR,
      RFsrBL,
      RFsrBR,
      numOfFSR
    };

    double force[numOfFSR]; // the force that applied to the sensor
    double data[numOfFSR]; // the data read from the platform directly
    bool valid[numOfFSR]; // if the sensor data is valid
    static const Vector3<double> offset[numOfFSR];

    /** total force on left foot */
    double forceLeft() const;

    /** total force on right foot */
    double forceRight() const;
    
    /** Returns the force value of the given sensor or -1 if its value is invalid. */
    double forceOf(FSRID fsrId) const;

    FSRData();
    ~FSRData();
    static std::string getFSRName(FSRID fsr);

    virtual void print(std::ostream& stream) const;
  };
  
  template<>
  class Serializer<FSRData>
  {
    public:
    static void serialize(const FSRData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, FSRData& representation);
  };
  
  

}//end namespace naoth

#endif  /* _FSRData_H_ */

