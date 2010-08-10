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

#include "Tools/ModuleFramework/Representation.h"
#include "Tools/Math/Vector3.h"
#include "Messages/naothMessages.pb.h"
#include "Tools/DataStructures/Streamable.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"

using namespace std;

class FSRData : public Streamable, public Printable, public PlatformInterchangeable
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

  virtual void toDataStream(ostream& stream) const;
  virtual void fromDataStream(istream& stream);
  virtual void print(ostream& stream) const;
};

REPRESENTATION_INTERFACE(FSRData);

#endif	/* _FSRDATA_H */

