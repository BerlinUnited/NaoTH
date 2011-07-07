/**
 * @file MessageQueue4Process.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating between processes
 */

#include "MessageQueue4Process.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Debug/Trace.h"
#include <cstring>

// only avaiable on Nao
#ifdef NAO
#include <gio/gunixsocketaddress.h>

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
    g_socket_set_blocking(serverSocket, false); // just to make sure...
    writeSocket = g_socket_accept(serverSocket, NULL, NULL);

    if ( writeSocket!=NULL )
    {
      g_socket_set_blocking(writeSocket, false);
      theWriteStream.init(writeSocket);
      cout<<"MessageQueue " << theName << " writing opened"<<endl;
    }

    if ( writeSocket==NULL || !g_socket_is_connected(writeSocket) )
    {
      return;
    }
  }

  try
  {
    theWriteStream<<msg<<send;
  }
  catch (runtime_error& e)
  {
    if ( g_socket_close(writeSocket, NULL) )
    {
      cerr<<"MessageQueue " << theName << " writing closed, because "<<e.what()<<endl;
      g_object_unref(writeSocket);
      writeSocket = NULL;
      theWriteStream.init(NULL);
    }
    else
    {
      cerr<<"MessageQueue " << theName << " writing can not close"<<endl;
    }
  }
}

bool MessageQueue4Process::empty()
{
  if ( !g_socket_is_connected(readSocket) )
  {
    connect();
    if ( !g_socket_is_connected(readSocket) )
      return true;
  }

  string msg;
  try{
    theReadStream>>msg;
  }
  catch (runtime_error& e)
  {
    if ( g_socket_close(readSocket, NULL) )
    {
      cerr<<"MessageQueue " << theName << " reading closed, because "<<e.what()<<endl;
      g_object_unref(readSocket);
      readSocket = g_socket_new(G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, NULL);
      g_socket_set_blocking(readSocket, false);
    }
    else
    {
      cerr<<"MessageQueue " << theName << " reading close failed"<<endl;
    }
  }

  if ( msg.size() > 0 )
  {
    MessageQueue::write(msg);
  }

  return MessageQueue::empty();
}

void MessageQueue4Process::connect()
{
  if ( g_socket_connect(readSocket, addr, NULL, NULL) )
  {
    theReadStream.init(readSocket);
    cout<<"MessageQueue " << theName << " reading opened"<<endl;
  }
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
    g_error_free(error);
  }

  MessageQueue::setWriter(writer);
  cout<<"MessageQueue open "<<theName<<endl;
}

#endif

