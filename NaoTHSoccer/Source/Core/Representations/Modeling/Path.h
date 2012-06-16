/**
* @file Path.h
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Kirill Yasinovskiy</a>
* Declaration of class Path
*/

#ifndef PATH_H_
#define PATH_H_

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"
#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/FrameInfo.h"

class Path: public naoth::Printable
{
private:
  unsigned int _timeNoNodeExpandable;
  naoth::FrameInfo _frameInfoWhenNoNodeExpanded;

public:
  // default constructor
  Path():
    _timeNoNodeExpandable(0),

    // accessors
    timeNoNodeExpandable(_timeNoNodeExpandable),
    frameInfoWhenNoNodeExpanded(_frameInfoWhenNoNodeExpanded)
  {}
  // default destructor
  ~Path(){};

  // target point and next point to go
  Vector2d nextPointToGo;
  Vector2d targetPoint;

  // Tells when the path was found the last time
  const naoth::FrameInfo& frameInfoWhenNoNodeExpanded;
  // time how long we can't expand any nodes
  const unsigned int& timeNoNodeExpandable;

  void setFrameInfoWhenNodeWasNotExpanded(const naoth::FrameInfo& frameInfo)
  {
    if(frameInfo.getFrameNumber() != frameInfoWhenNoNodeExpanded.getFrameNumber()+1)
      _timeNoNodeExpandable = 0;
    else
    {
      ASSERT(frameInfoWhenNoNodeExpanded.getTime() < frameInfo.getTime());
      _timeNoNodeExpandable += frameInfo.getTimeSince(frameInfoWhenNoNodeExpanded.getTime());
    }
    _frameInfoWhenNoNodeExpanded = frameInfo;
  }//end setFrameInfoWhenBallWasSeen

  virtual void print(std::ostream& stream) const
  {
  }//end print

};

#endif // end PATH_H_