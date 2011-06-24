/**
* @file FrameInfo.h
* The file declares a class that contains information on the current frame.
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/ 

#ifndef __FrameInfo_h_
#define __FrameInfo_h_

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Streamable.h"
#include "PlatformInterface/PlatformInterchangeable.h"

namespace naoth
{
  /**
  * @class FrameInfo
  * A class that contains information on the current frame.
  */
  class FrameInfo : public Printable, public PlatformInterchangeable, public Streamable
  {
  public:
    /**
    * Default constructor.
    */
    FrameInfo() :
        time(0),
        frameNumber(0)
    {}

    /**
    * The method returns the time difference between a given time stamp and the
    * current frame time.
    * @param timeStamp A time stamp, usually in the past.
    * @return The number of ms passed since the given time stamp.
    */
    int getTimeSince(unsigned timeStamp) const {return int(time - timeStamp);}

    /**
    * The method returns the corrent time in seconds current frame time.
    */
    double getTimeInSeconds() const {return static_cast<double>(time)/1000.0; }
    
    /**
     * The method returns the basic time step in seconds
     */
    double getBasicTimeStepInSecond() const { return static_cast<double>(basicTimeStep)/1000.0; }

    unsigned int time; /**< The time stamp of the data processed in the current frame in miliseconds. */
    unsigned int frameNumber; /**< The number of the frame. */
    
    unsigned int basicTimeStep; /**< The time of each step which depends on platforms */

    virtual void print(std::ostream& stream) const
    {
      stream << "frameNumber = " << frameNumber << endl;
      stream << "time(ms) = " << time << endl;
      stream << "time(s) = " << getTimeInSeconds() << endl;
      stream << "fps(avg) = " << (((double)frameNumber) / getTimeInSeconds()) << endl;
      stream << "basic time step = "<< basicTimeStep << endl;
    }


  };
  
  template<>
  class Serializer<FrameInfo>
  {
    public:
      static void serialize(const FrameInfo& representation, std::ostream& stream);
      static void deserialize(std::istream& stream, FrameInfo& representation);
  };
  
}
#endif //__FrameInfo_h_
