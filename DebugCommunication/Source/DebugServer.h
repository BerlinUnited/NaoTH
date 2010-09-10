/* 
 * File:   DebugServer.h
 * Author: thomas
 *
 * Created on 10. September 2010, 15:38
 */

#ifndef DEBUGSERVER_H
#define	DEBUGSERVER_H

#include <gio/gio.h>

class DebugServer
{
public:
  DebugServer();
  virtual ~DebugServer();
private:

  GSocket* socket;

};

#endif	/* DEBUGSERVER_H */

