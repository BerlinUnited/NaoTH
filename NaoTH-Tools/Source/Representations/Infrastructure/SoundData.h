/* 
 * File:   SoundData.h
 * Author: Oliver Welter
 *
 * Created on 16. Juni 2009, 21:29
 */

#ifndef _SOUNDDATA_H
#define	_SOUNDDATA_H

#include <string>
#include "PlatformInterface/PlatformInterchangeable.h"

using namespace std;

namespace naoth
{

  class SoundData : public PlatformInterchangeable
  {
  public:
    enum SpeakerID
    {
      LeftSpeaker,
      RightSpeaker,
      numOfSpeaker
    };

    bool mute;
    int volume[numOfSpeaker];
    string soundFile;

    SoundData();
    ~SoundData();
  };
}

#endif	/* _SOUNDDATA_H */

