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

#endif	/* COGNITIONDEBUGSERVER_H */

