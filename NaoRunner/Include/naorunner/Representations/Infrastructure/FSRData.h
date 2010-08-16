/* 
 * File:   FSRData.h
 * Author: Oliver Welter
 * eMail: welter@informatik.hu-berlin.de
 *
 * Created on 31. Dezember 2008, 18:20
 */

#ifndef _FSRDATA_H
#define	_FSRDATA_H

#include <string>

#include "naorunner/Tools/Math/Vector3.h"
#include "naorunner/PlatformInterface/PlatformInterchangeable.h"
#include "naorunner/Tools/DataStructures/Printable.h"

using namespace std;

namespace naorunner
{

  class FSRData : public Printable, public PlatformInterchangeable
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

    /* total force on left foot */
    double forceLeft() const;

    /* total force on right foot */
    double forceRight() const;

    FSRData();
    ~FSRData();
    static string getFSRName(FSRID fsr);

    virtual void print(ostream& stream) const;
  };

}//end namespace naorunner

#endif	/* _FSRDATA_H */

