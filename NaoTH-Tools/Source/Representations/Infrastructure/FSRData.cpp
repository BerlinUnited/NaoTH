#include "Representations/Infrastructure/FSRData.h"

using namespace naoth;

// TODO: make it config
const Vector3<double> FSRData::offset[FSRData::numOfFSR] =
{Vector3<double>(70.1,30 ,-46),
 Vector3<double>(70.1,-23,-46),
 Vector3<double>(-30.4,30,-46),
 Vector3<double>(-29.8,-19,-46),
 Vector3<double>(70.1, 23,-46),
 Vector3<double>(70.1,-30,-46),
 Vector3<double>(-29.8,19,-46),
 Vector3<double>(-30.4,-30,-46)
};
   
FSRData::FSRData()
{
  for(int i=0;i<numOfFSR;i++)
  {
    force[i] = 0.0;
    data[i] = 0.0;
    valid[i] = true;
  }//end for
}

FSRData::~FSRData()
{
}

string FSRData::getFSRName(FSRID fsr)
{
  switch(fsr)
  {
    case LFsrFL: return string("LFsrFL");
    case LFsrFR: return string("LFsrFR");
    case LFsrBL: return string("LFsrBL");
    case LFsrBR: return string("LFsrBR");
    case RFsrFL: return string("RFsrFL");
    case RFsrFR: return string("RFsrFR");
    case RFsrBL: return string("RFsrBL");
    case RFsrBR: return string("RFsrBR");
    default: return string("Unknown FSR");
  }//end switch
}//end getName

void FSRData::print(ostream& stream) const
{
  stream << "Name : force(N) , data"<<endl;
  double sum = 0;
  for(int i = 0; i < numOfFSR; i++)
  {
    stream << getFSRName((FSRID)i) << " : " << force[i] << " , " << data[i];
    if ( !valid[i] ) stream << " INVALID!!";
    stream << "\n";
    sum += force[i];
  }//end for
  stream << "Sum: " << sum << endl;
}//end print

double FSRData::forceLeft() const
{
  double f = 0;
  for ( int i=LFsrFL; i<=LFsrBR; i++)
  {
    if ( valid[i] )// check if the sensor is valid
    {
      f += force[i];
    }
  }//end for
  return f;
}//end forceLeft

double FSRData::forceRight() const
{
  double f = 0;
  for (int i = RFsrFL; i <= RFsrBR; i++)
  {
    if ( valid[i] )// check if the sensor is valid
    {
      f += force[i];
    }
  }//end for
  return f;
}//end forceRight
