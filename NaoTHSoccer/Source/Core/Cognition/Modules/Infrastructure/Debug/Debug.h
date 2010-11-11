/* 
 * File:   Debug.h
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#ifndef DEBUG_H
#define	DEBUG_H

#include <ModuleFramework/Module.h>
#include <DebugCommandExecutor.h>

#include <DebugServer.h>

#include <Representations/Infrastructure/Image.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(Debug)
  REQUIRE(Image)
END_DECLARE_MODULE(Debug)

class Debug : public DebugBase, public DebugCommandExecutor
{
public:
  Debug();
  virtual ~Debug();

  virtual void execute();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream);

private:

  DebugServer debugServer;

};

#endif	/* DEBUG_H */

