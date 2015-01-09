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

class Path
{
private:
  unsigned int _timeNoNodeExpandable;
  naoth::FrameInfo _frameInfoWhenNoNodeExpanded;

public:
  // default constructor
  Path() {}
  // default destructor
  ~Path(){};

  // target point and next point to go
  Vector2d nextPointToGo;
  Vector2d targetPoint;

  // Tells when the path was found the last time
  //const naoth::FrameInfo& frameInfoWhenNoNodeExpanded;
  const naoth::FrameInfo& getFrameInfoWhenNoNodeExpanded(){return _frameInfoWhenNoNodeExpanded;}
  // time how long we can't expand any nodes
  //const unsigned int& timeNoNodeExpandable;
  unsigned int getTimeNoNodeExpandable() const {return _timeNoNodeExpandable;}

  void setFrameInfoWhenNodeWasNotExpanded(const naoth::FrameInfo& frameInfo)
  {
    if(frameInfo.getFrameNumber() != _frameInfoWhenNoNodeExpanded.getFrameNumber()+1)
      _timeNoNodeExpandable = 0;
    else
    {
      ASSERT(_frameInfoWhenNoNodeExpanded.getTime() < frameInfo.getTime());
      _timeNoNodeExpandable += frameInfo.getTimeSince(_frameInfoWhenNoNodeExpanded.getTime());
    }
    _frameInfoWhenNoNodeExpanded = frameInfo;
  }//end setFrameInfoWhenBallWasSeen
};

#endif // end PATH_H_