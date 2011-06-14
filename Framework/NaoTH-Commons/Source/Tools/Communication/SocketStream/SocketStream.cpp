#include "SocketStream.h"
#include <cassert>
#include <glib.h>

SocketStream::SocketStream()
: mRecvdLen(0),socket(NULL)
{
    mRecvBuf = new char[default_recv_buffer_size + 1];
    mRecvBufSize = default_recv_buffer_size;
}

SocketStream::~SocketStream()
{
  init(NULL);
  delete [] mRecvBuf;
}

void SocketStream::init(GSocket* s)
{
  if ( socket != NULL)
  {
    g_object_unref(socket);
    socket = NULL;
  }

  if ( s != NULL)
  {
    g_object_ref(s);
    socket = s;
  }
  mRecvdLen = 0;
}

void SocketStream::send(const std::string& msg) throw(std::runtime_error)
{
  if(socket == NULL)
  {
    return;
  }
  
  if(g_socket_is_connected(socket))
  {
    GError* err = NULL;
    g_socket_send(socket, msg.c_str(), msg.size(), NULL, &err);
    if(err)
    {
      std::string errMsg = err->message;
      g_error_free(err);
      throw std::runtime_error(errMsg);
    }
  }
  else
  {
    throw std::runtime_error("Can't send, not connected");
  }
}

SocketStream& SocketStream::send()
{
  send(mSendStr.str());
  mSendStr.str("");
  return *this;
}

int SocketStream::recv(std::string& msg) throw(std::runtime_error)
{
  if(socket == NULL)
  {
    return -1;
  }
  else if(!g_socket_is_connected(socket))
  {
    throw std::runtime_error("Can't receive, not connected");
    return -1;
  }

  memset(mRecvBuf, 0, mRecvBufSize + 1);
  GError* err = NULL;
  int status = g_socket_receive(socket, mRecvBuf, mRecvBufSize, NULL, &err);
  if (status < 0)
  {
    std::string errMsg = err->message;
    g_error_free(err);
    throw std::runtime_error(errMsg);
  }
  else if (status > 0) {
    msg = mRecvBuf;
  } else
  {
    msg = "";
  }
  return status;
}

void SocketStream::prefixedSend()
{
  if (!mSendStr.str().empty()) {
    // prefix the message with it's payload length
    unsigned int len = static_cast<unsigned int> (g_htonl(mSendStr.str().size()));
    char preChar[sizeof (unsigned int) ];
    memcpy(preChar, &len, sizeof (unsigned int));
    std::string msg = mSendStr.str();
    msg.insert(0, preChar, sizeof (unsigned int));
    mSendStr.str() = std::string(preChar) + mSendStr.str();
    send(msg);
    mSendStr.str("");
  }
}

bool SocketStream::isFixedLengthDataAvailable(unsigned int len) throw(std::runtime_error)
{
  if(socket == NULL || !g_socket_is_connected(socket))
  {
    return false;
  }

  if (len == 0) return true;

  /* check whether the read_buffer is large enough to handle this request
  if not, reallocate the array */
  if (mRecvBufSize < len) {
    reallocRecvBuffer(len);
  }

  for (;;) {
    /* See if we have enough data to satisfy request */
    if (mRecvdLen >= len) return true;

    /* there was not enough data in the read buffer, so let's try to get some more */
    int res = g_socket_receive(socket, mRecvBuf + mRecvdLen, mRecvBufSize - mRecvdLen, NULL, NULL);
    
    if (res <= 0)
    {
      bool lostConnection = g_socket_condition_check(socket, G_IO_IN) & (G_IO_HUP|G_IO_ERR);
      if ( lostConnection )
      {
        throw std::runtime_error("lost connection");
      }
      return false;
    }
    /* res is > 0 */
    mRecvdLen += res;
  }
}

int SocketStream::prefixedRecv(std::string& msg)
{
  msg = "";
  unsigned int messLen;

  bool res = isFixedLengthDataAvailable(sizeof (unsigned int));
  if (!res) return 0;
  //get the message length
  messLen = g_ntohl((*(unsigned int*) mRecvBuf));

  //try and get the data
  res = isFixedLengthDataAvailable(messLen + sizeof (unsigned int));
  if (!res) return 0;

  //we have to copy, skipping the sizeof(int) bytes at the beginning
  msg.insert(0, mRecvBuf + sizeof (unsigned int), messLen);
  mRecvdLen -= (sizeof (unsigned int) +messLen);
  memmove(mRecvBuf, mRecvBuf + sizeof (unsigned int) +messLen, mRecvdLen);
  return messLen;
}

void SocketStream::reallocRecvBuffer(unsigned int size)
{
  char* oldBuffer = mRecvBuf;
  mRecvBuf = new char[size + 1];
  memcpy(mRecvBuf, oldBuffer, mRecvdLen);
  mRecvBufSize = size;
  delete [] oldBuffer;
}

