/* 
 * File:   BumperData.h
 * Author: Oliver Welter
 *
 * Created on 15. März 2009, 00:11
 */

#ifndef _BUMPERDATA_H
#define	_BUMPERDATA_H

#include "Tools/ModuleFramework/Representation.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"

using namespace std;

class BumperData : public PlatformInterchangeable, public Printable
{
public:
  enum BumperID
  {
    LeftBumperLeft,
    LeftBumperRight,
    RightBumperLeft,
    RightBumperRight,
    numOfBumper
  };

  bool data[numOfBumper];
  
  BumperData();
  virtual ~BumperData();
  static string getBumperName(BumperID id);
  
  virtual void print(ostream& stream) const;
private:

};

REPRESENTATION_INTERFACE(BumperData);

#endif	/* _BUMPERDATA_H */
