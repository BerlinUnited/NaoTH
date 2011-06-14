/**
 * @file MessageQueue4Process.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating between processes
 */

#include "MessageQueue4Process.h"
#include <gio/gunixsocketaddress.h>
#include "Tools/Debug/NaoTHAssert.h"
#include <cstring>

MessageQueue4Process::MessageQueue4Process(const std::string& name):
theName("/tmp/naoth.messagequeue."+name),
  serverSocket(NULL),
  readSocket(NULL),
  writeSocket(NULL)
{
  addr = g_unix_socket_address_new(theName.c_str());
}

MessageQueue4Process::~MessageQueue4Process()
{
  if ( serverSocket != NULL )
    g_socket_close(serverSocket, NULL);
  if ( readSocket != NULL )
    g_socket_close(readSocket, NULL);
  if ( writeSocket != NULL )
    g_socket_close(writeSocket, NULL);
}
  
void MessageQueue4Process::write(const std::string& msg)
{
  if ( writeSocket==NULL || !g_socket_is_connected(writeSocket) )
  {
    writeSocket = g_socket_accept(serverSocket, NULL, NULL);
    theWriteStream.init(writeSocket);
  }
  if ( writeSocket==NULL || !g_socket_is_connected(writeSocket) )
    return;

  theWriteStream<<msg<<send;
}

bool MessageQueue4Process::empty()
{
  if ( !g_socket_is_connected(readSocket) ) connect();
  if ( !g_socket_is_connected(readSocket) ) return true;

  string msg;
  theReadStream>>msg;
  if ( msg.size() > 0 )
  {
    MessageQueue::write(msg);
  }

  return MessageQueue::empty();
}

void MessageQueue4Process::clear()
{
}

void MessageQueue4Process::connect()
{
  g_socket_connect(readSocket, addr, NULL, NULL);

  GError* error = NULL;
  g_socket_check_connect_result(readSocket,&error);
  if ( error != NULL )
  {
    cerr<<"connect error: "<<error->message<<endl;
    return;
  }
  theReadStream.init(readSocket);
}

void MessageQueue4Process::setReader(MessageReader* reader)
{
  readSocket = g_socket_new(G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, NULL);
  g_socket_set_blocking(readSocket, false);
  connect();
  MessageQueue::setReader(reader);
}


void MessageQueue4Process::setWriter(MessageWriter* writer)
{
  serverSocket = g_socket_new(G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, NULL);
  g_socket_set_blocking(serverSocket, false);

  unlink(theName.c_str());

  GError* error = NULL;
  bool suc = g_socket_bind(serverSocket, addr, TRUE, &error);
  if ( suc )
  {
    suc = g_socket_listen(serverSocket, &error);
  }

  if ( !suc )
  {
    cerr<<"can not open "<<theName<<endl;
    cerr<<error->message<<endl;
  }

  MessageQueue::setWriter(writer);
}

