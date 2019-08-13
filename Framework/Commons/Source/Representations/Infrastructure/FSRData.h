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
#include <vector>

#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{

  class FSRData : public Printable
  {
  public:
    /*
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
    };*/

    enum SensorID {
      FrontLeft,
      FrontRight,
      RearLeft,
      RearRight,
      numOfFSR
    };

    //double force[numOfFSR]; // the force that applied to the sensor
    //double data[numOfFSR]; // the data read from the platform directly

    std::vector<double> dataLeft;
    std::vector<double> dataRight;

    FSRData();
    ~FSRData();
    static const std::string getFSRName(SensorID fsr);

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

