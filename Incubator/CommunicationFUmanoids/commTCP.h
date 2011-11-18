/** @file
 **
 ** Handles TCP connection to remote application.
**/

#ifndef __COMM_TCP_H__
#define __COMM_TCP_H__

#include "system/timer.h"
#include "system/thread.h"
#include "platform/generic/singleton.h"

#include "transport/transport_tcp.h"
#include "transport/transport_tcp_server.h"

#include "communication/protobuf/msg_fumanoid.pb.h"


/*------------------------------------------------------------------------------------------------*/
/**
 * @addtogroup comm
 * @{
 */


/*------------------------------------------------------------------------------------------------*/

/// default  port number that the server should listen to and send packets to other players/PC
#define DEFAULTCOMMPORT 11011


/*------------------------------------------------------------------------------------------------*/
/// CommTCPHeader struct
typedef struct {
	uint8_t   version;
	uint8_t   command;
	uint8_t   parameter1;
	uint8_t   parameter2;
} CommTCPHeader;

/// Commands for CommTCP
typedef enum {
	TCPCMD_GETIMAGE = 'I',
	TCPCMD_MESSAGE  = 'M',
	TCPCMD_GETLUT   = 'G',
	TCPCMD_SETLUT   = 'S'
} COMMTCPCMD;


/*------------------------------------------------------------------------------------------------*/
/**
 * CommTCP handles TCP connection to remote application.
 *
  * It is similar to Comm.
 */
class CommTCP : public Singleton<CommTCP>, public Thread {
private:
	/// constructor (protected, this is a singleton)
	CommTCP();

	friend class Singleton<CommTCP>;

protected:
	///
	TransportTCPServer *server;
	TransportTCP       *transport;

	/// critical section
	CriticalSection cs;

	/// thread main function, waits for incoming packets
	void threadMain();

	void handle(const CommTCPHeader &request, Transport *transport);
	void sendImage(Transport *transport, uint8_t version, uint8_t param1, uint8_t param2);

public:
	virtual ~CommTCP();

	/// get a descriptive name of the thread
	virtual const char* getName() {
		return "CommTCP";
	}

	void init();

	void sendMessage(de::fumanoids::message::Message &message, const char *ip);
};

/**
 * @}
 */

#endif
