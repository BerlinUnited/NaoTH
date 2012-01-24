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
        step(0),
        frameNumber(0)
    {}

    FrameInfo(unsigned int time, unsigned int frameNumber) 
      :
        time(time),
        frameNumber(frameNumber)
    {}

    /**
    * The method returns the time difference between a given time stamp and the
    * current frame time.
    * @param timeStamp A time stamp, usually in the past.
    * @return The number of ms passed since the given time stamp.
    */
    inline int getTimeSince(unsigned timeStamp) const {return int(time - timeStamp);}

    /**
    * The method returns the corrent time in seconds current frame time.
    */
    inline double getTimeInSeconds() const {return static_cast<double>(time)/1000.0; }

    inline unsigned int getTime() const { return time; }

    inline unsigned int getFrameNumber() const { return frameNumber; }

    inline unsigned int getStep() const { return step; }

    // TODO: there should be functionality in representations!!!
    void setTime(unsigned int t)
    {
      step = t - time;
      time = t;
      frameNumber++;
    }

    virtual void print(std::ostream& stream) const
    {
      stream << "frameNumber = " << frameNumber << endl;
      stream << "time(ms) = " << time << endl;
      stream << "fps(avg) = " << (((double)frameNumber) / getTimeInSeconds()) << endl;
    }

    friend class Serializer<FrameInfo>;

  private:

    unsigned int time; /**< The time stamp of the data processed in the current frame in miliseconds. */
    unsigned int step; /**< the time of last step */
    unsigned int frameNumber; /**< The number of the frame. */
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
