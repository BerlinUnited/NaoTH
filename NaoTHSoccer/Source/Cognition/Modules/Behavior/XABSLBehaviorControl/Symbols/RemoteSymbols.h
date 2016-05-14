/* 
 * File:   RemoteSymbols.h
 */

#ifndef _RemoteSymbols_H
#define _RemoteSymbols_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Representations/Infrastructure/RemoteControlCommand.h"

// tools
#include "Tools/Math/Common.h"

BEGIN_DECLARE_MODULE(RemoteSymbols)
  REQUIRE(RemoteControlCommand)
END_DECLARE_MODULE(RemoteSymbols)

class RemoteSymbols: public RemoteSymbolsBase
{
public:

  RemoteSymbols()
  {
    theInstance = this;
  }

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** update internal symbol state */
  virtual void execute();

  virtual ~RemoteSymbols();

private:
  static RemoteSymbols* theInstance;
};

#endif  /* _RemoteSymbols_H */

