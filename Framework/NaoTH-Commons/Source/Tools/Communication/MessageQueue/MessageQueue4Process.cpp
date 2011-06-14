/**
 * @file MessageQueue4Process.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating between processes
 */

#include "MessageQueue4Process.h"
#include <gio/gunixsocketaddress.h>
#include "Tools/Debug/NaoTHAssert.h"

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
  //g_object_unref()
}
  
void MessageQueue4Process::write(const std::string& msg)
{
  if ( writeSocket==NULL || !g_socket_is_connected(writeSocket) )
    writeSocket = g_socket_accept(serverSocket, NULL, NULL);
  if ( writeSocket==NULL || !g_socket_is_connected(writeSocket) )
    return;

  GError* err = NULL;

  for(int i=0; i<msg.length(); i++)
  {
    ASSERT( *(msg.c_str()+i) != '\0' );
  }

  gssize bytesWritten = g_socket_send(writeSocket, msg.c_str(), (msg.length()+1),NULL, &err);
  if (err)
  {
    std::cerr << "send error: " << err->message << std::endl;
    g_error_free(err);
    err = NULL;
  }
  //cout<<"write = "<<bytesWritten<<" == "<<(msg.length()+1)*sizeof(char)<<endl;
}

bool MessageQueue4Process::empty()
{
  if ( !g_socket_is_connected(readSocket) ) connect();
  if ( !g_socket_is_connected(readSocket) ) return true;

  // receive all data
  gchar charBuffer;

  while ( g_socket_receive(readSocket, &charBuffer, sizeof(gchar), NULL, NULL) > 0 )
  {
    if ( charBuffer != '\0')
    {
      readBuffer += charBuffer;
    }
    else
    {
      cout<<"read one msg"<< readBuffer.length() <<endl;
      MessageQueue::write(readBuffer);
      readBuffer = "";
    }
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

