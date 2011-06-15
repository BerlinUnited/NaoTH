/**
 * @file SocketStream.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief encapsulate the socket I/O as a stream
 * 
 */

#ifndef SOCKET_STREAM_H
#define SOCKET_STREAM_H

#include <gio/gio.h>

#include <errno.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <stdexcept>

const int default_recv_buffer_size = 1024;

class SocketStream
{
public:

  SocketStream();

  ~SocketStream();

  void init(GSocket* s);

  void send(const std::string& msg) throw(std::runtime_error);

  SocketStream& send();

  int recv(std::string& msg) throw(std::runtime_error);

  template <class T>
  SocketStream & operator <<(const T& msg)
  {
    mSendStr << msg;
    return *this;
  }

  SocketStream & operator <<(SocketStream& (*pf) (SocketStream&))
  {
    return pf(*this);
  }

  SocketStream & operator >>(std::string& msg)
  {
    recv(msg);
    return *this;
  }

  /////////////////
protected:

  void prefixedSend();

  bool isFixedLengthDataAvailable(unsigned int len) throw(std::runtime_error);

  int prefixedRecv(std::string& msg);

  void reallocRecvBuffer(unsigned int size);

  template <class T>
  void addSendMsg(const T& msg)
  {
    mSendStr << msg;
  }



private:
  std::stringstream mSendStr;
  char* mRecvBuf;
  unsigned int mRecvBufSize;
  unsigned int mRecvdLen;

  GSocket* socket;
};

template<class T>
T& send(T& o)
{
  return o.send();
}

class PrefixedSocketStream : public SocketStream
{
public:

  PrefixedSocketStream()
  {
  }

  ~PrefixedSocketStream()
  {
  }

  PrefixedSocketStream& send()
  {
    prefixedSend();
    return *this;
  }

  template <class T>
  PrefixedSocketStream & operator <<(const T& msg)
  {
    addSendMsg(msg);
    return *this;
  }

  PrefixedSocketStream & operator <<(PrefixedSocketStream& (*pf) (PrefixedSocketStream&))
  {
    return pf(*this);
  }

  PrefixedSocketStream & operator >>(std::string& msg)
  {
    prefixedRecv(msg);
    return *this;
  }
};

#endif /* SOCKET_STREAM_HPP */
