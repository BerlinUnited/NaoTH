#ifndef _WifiMode_h_
#define _WifiMode_h_

#include <Tools/DataStructures/Printable.h>

class WifiMode : naoth::Printable {
public:
    WifiMode() : wifiEnabled(true) {

    }

    virtual ~WifiMode() {}

    virtual void print(std::ostream& stream) const {
        stream << "WiFi " << (wifiEnabled ? "enabled" : "disabled") << std::endl;
    }
public:
    bool wifiEnabled;
};

#endif // _WifiMode_h_