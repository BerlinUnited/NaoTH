/* 
 * File:   FSRData.h
 * Author: Oliver Welter
 * eMail: welter@informatik.hu-berlin.de
 *
 * Created on 31. Dezember 2008, 18:20
 */

#ifndef _FSRDATA_H
#define _FSRDATA_H

#include <string>

#include "Tools/Math/Vector3.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

using namespace std;

namespace naoth
{

  class FSRData : public Printable, public PlatformInterchangeable, public Streamable
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
    static string getFSRName(FSRID fsr);

    virtual void print(ostream& stream) const;
  };
  
  template<>
  class Serializer<FSRData>
  {
    public:
    static void serialize(const FSRData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, FSRData& representation);
  };
  
  

}//end namespace naoth

#endif  /* _FSRDATA_H */

