
#ifndef _WHISTLE_CONTROL_H
#define _WHISTLE_CONTROL_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class WhistleControl: public Printable
  {
  public:
    int onOffSwitch;              // controls if detection and audio capture is active or not
    std::string whistleListFile;  // defines whistle list file to use
    std::string activeChannels;   // defines channels to be used in multi channel mode
    double threshold;             // threshold for cross correlation check, values bigger equal will be accepted as whistle
    bool checkAllWhistles;        // check not only first but all whistles in whistle list
    bool saveRawAudio;            // if true, samples will be recorded to /tmp/<capture file name> as long as onOffSwitch on plus some short extra time if switched off

    WhistleControl();
    virtual void print(std::ostream& stream) const;

    virtual ~WhistleControl();
  };
}

#endif  /* _WHISTLE_CONTROL_H */

