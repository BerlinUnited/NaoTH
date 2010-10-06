/**
* @file FrameInfo.h
* The file declares a class that contains information on the current frame.
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/ 

#ifndef __FrameInfo_h_
#define __FrameInfo_h_

#include "Interface/Tools/DataStructures/Printable.h"
#include "Interface/PlatformInterface/PlatformInterchangeable.h"

namespace naorunner
{
  /**
  * @class FrameInfo
  * A class that contains information on the current frame.
  */
  class FrameInfo : public Printable, public PlatformInterchangeable
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
    * The method returns the corrent time in seconds
    * current frame time.
    * @param timeStamp A time stamp, usually in the past.
    * @return The number of ms passed since the given time stamp.
    */
    double getTimeInSeconds() const {return static_cast<double>(time)/1000.0 ;}

    unsigned int time; /**< The time stamp of the data processed in the current frame in us (miliseconds). */
    unsigned int frameNumber; /**< The number of the frame. */

  //  unsigned int motionFrameNumber; /**< The number of the frame. */
  //
  //  float cognition_fps; /** Frames per second of cognition */
  //  float motion_fps; /** Frames per second of motion */


    virtual void print(std::ostream& stream) const
    {
      stream << "frameNumber=" << frameNumber << endl;
      stream << "time=" << time << endl;
    }


  };
}
#endif //__FrameInfo_h_
