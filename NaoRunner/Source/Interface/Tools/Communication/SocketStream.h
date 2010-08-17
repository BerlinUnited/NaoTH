/**
 * @file SocketStream.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief encapsulate the socket I/O as a stream
 * 
 */

#ifndef SOCKET_STREAM_H
#define SOCKET_STREAM_H

#include "PracticalSocket.h"
#include <errno.h>
#include <iostream>
#include <sstream>
#include <cstring>
#ifndef WIN32
#include <netinet/in.h>
#else
#include "winsock2.h"
#endif

const int default_recv_buffer_size = 1024;

template <class SOCKET>
class SocketStream : public SOCKET
{
public:

    SocketStream():
    mRecvdLen(0)
    {
        mRecvBuf = new char[default_recv_buffer_size + 1];
        mRecvBufSize = default_recv_buffer_size;
    }

    SocketStream(const std::string& host, int port) :
    SOCKET(host, port),
    mRecvdLen(0)
    {
        mRecvBuf = new char[default_recv_buffer_size + 1];
        mRecvBufSize = default_recv_buffer_size;
    }

    ~SocketStream()
    {
        delete [] mRecvBuf;
    }

    void send(const std::string& msg)
    {
        SOCKET::send(msg.c_str(), msg.size());
    }

    SocketStream& send()
    {
        SOCKET::send(mSendStr.str().c_str(), mSendStr.str().size());
        mSendStr.str("");
        return *this;
    }

    int recv(string& msg)
    {
        memset(mRecvBuf, 0, mRecvBufSize + 1);
        int status = SOCKET::recv(mRecvBuf, mRecvBufSize);
        if (status == -1)
        {
            std::cerr << "status == -1   errno == " << errno << endl;
        } else if (status > 0)
        {
            msg = mRecvBuf;
        } else
        {
            msg = "";
        }
        return status;
    }

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

    void prefixedSend()
    {
        if (!mSendStr.str().empty())
        {
            // prefix the message with it's payload length
            unsigned int len = static_cast<unsigned int> (htonl(mSendStr.str().size()));
            static char preChar[sizeof (unsigned int) ];
            memcpy(preChar, &len, sizeof (unsigned int));
            string msg = mSendStr.str();
            msg.insert(0, preChar, sizeof (unsigned int));
            mSendStr.str() = string(preChar) + mSendStr.str();
            send(msg);
            mSendStr.str("");
        }
    }

    bool isFixedLengthDataAvailable(unsigned int len)
    {
        if (len == 0) return true;

        /* check whether the read_buffer is large enough to handle this request
        if not, reallocate the array */
        if (mRecvBufSize < len)
        {
            reallocRecvBuffer(len);
        }

        for (;;)
        {
            /* See if we have enough data to satisfy request */
            if (mRecvdLen >= len) return true;
            /* there was not enough data in the read buffer, so let's try to get some more */
            int res = SOCKET::recv(mRecvBuf + mRecvdLen, mRecvBufSize - mRecvdLen);
            if (res <= 0) return false;
            /* res is > 0 */
            mRecvdLen += res;
        }
    }

    int prefixedRecv(string& msg)
    {
        msg = "";
        unsigned int messLen;

        bool res = isFixedLengthDataAvailable(sizeof (unsigned int));
        if (!res) return 0;
        //get the message length
        messLen = ntohl((*(unsigned int*) mRecvBuf));

        //try and get the data
        res = isFixedLengthDataAvailable(messLen + sizeof (unsigned int));
        if (!res) return 0;

        //we have to copy, skipping the sizeof(int) bytes at the beginning
        msg.insert(0, mRecvBuf + sizeof (unsigned int), messLen);
        mRecvdLen -= (sizeof (unsigned int) + messLen);
        memmove(mRecvBuf, mRecvBuf + sizeof (unsigned int) + messLen, mRecvdLen);
        return messLen;
    }

    void reallocRecvBuffer(unsigned int size)
    {
        char* oldBuffer = mRecvBuf;
        mRecvBuf = new char[size + 1];
        memcpy(mRecvBuf, oldBuffer, mRecvdLen);
        mRecvBufSize = size;
        delete [] oldBuffer;
    }

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
};

template<class T>
T& send(T& o)
{
    return o.send();
}

template <class SOCKET>
class PrefixedSocketStream : public SocketStream<SOCKET>
{
public:

    PrefixedSocketStream()
    {
    }

    PrefixedSocketStream(const std::string& host, int port) :
    SocketStream<SOCKET>(host, port)
    {
    }

    ~PrefixedSocketStream()
    {
    }

    PrefixedSocketStream& send()
    {
        SocketStream<SOCKET>::prefixedSend();
        return *this;
    }

    template <class T>
    PrefixedSocketStream & operator <<(const T& msg)
    {
        SocketStream<SOCKET>::addSendMsg(msg);
        return *this;
    }

    PrefixedSocketStream & operator <<(PrefixedSocketStream& (*pf) (PrefixedSocketStream&))
    {
        return pf(*this);
    }

    PrefixedSocketStream & operator >>(std::string& msg)
    {
        SocketStream<SOCKET>::prefixedRecv(msg);
        return *this;
    }
};

#endif /* SOCKET_STREAM_HPP */
