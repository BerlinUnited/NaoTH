/* 
 * File:   CognitionDebugServer.h
 * Author: thomas
 *
 * Created on 12. November 2010, 09:50
 */

#ifndef COGNITIONDEBUGSERVER_H
#define	COGNITIONDEBUGSERVER_H

#include <Tools/DataStructures/Singleton.h>
#include <DebugServer.h>

class CognitionDebugServer : public DebugServer, public naoth::Singleton<CognitionDebugServer>
{
public:
  CognitionDebugServer();
  
  virtual ~CognitionDebugServer();
private:

};

#undef REGISTER_DEBUG_COMMAND

#ifdef DEBUG
/** register a command only in DEBUG mode */
#define REGISTER_DEBUG_COMMAND(command, description, executor) \
CognitionDebugServer::getInstance().registerCommand(command, description, executor);
#else //DEBUG
#define REGISTER_DEBUG_COMMAND(command, description, executor) {}
#endif //DEBUG

#endif	/* COGNITIONDEBUGSERVER_H */

