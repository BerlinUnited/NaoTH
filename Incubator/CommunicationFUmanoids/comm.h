#ifndef __COMM_H__
#define __COMM_H__

#include "system/thread.h"
#include "platform/generic/singleton.h"
#include "transport/transport.h"

#include "protobuf/msg_fumanoid.pb.h"

#include <stdarg.h>
#include <arpa/inet.h>

#include <vector>
#include <map>

/*------------------------------------------------------------------------------------------------*/
/**
 * @defgroup comm The Communication
 * @ingroup infrastructure
 *
 * The Communication handles all incoming and outgoing communication.
 *
 * The main classes are Comm and CommTCP.
 *
 * @{
 */

/*------------------------------------------------------------------------------------------------*/

/// default port number that the server should listen to and send packets to other players/PC
#define DEFAULTCOMMPORT 11011

/// default log level (valid values: NONE (no logs), ERROR, WARNING, INFO, DEBUG
#define DEFAULTLOGLEVEL "DEBUG"

// available operations
#define OPERATIONS \
	/* ID  Log    Name                         */\
	OP(-1, false, OP_NO_OP)                      \
	OP( 0, false, OP_LOG)                        \
	OP( 1, false, OP_STATUS)                     \
	OP( 4, true,  OP_DISABLEMOTORS)              \
	OP( 5, true,  OP_ENABLEMOTORS)               \
	OP( 6, true,  OP_PLAYMOTION)                 \
	OP( 7, true,  OP_GETMOTORSTATUS)             \
	OP( 8, true,  OP_GETSTRATEGIES)              \
	OP( 9, true,  OP_GETROLES)                   \
	OP(15, true,  OP_SETMOTOR)                   \
	OP(17, true,  OP_IMAGECAPTURE)               \
	OP(20, true,  OP_LIMITTEAM)                  \
	OP(21, true,  OP_TIMESYNC)                   \
	OP(23, true,  OP_GETMOTOROFFSETS)            \
	OP(24, true,  OP_SETMOTOROFFSETS)            \
	OP(25, true,  OP_GETLISTOFCAMERASETTINGS)    \
	OP(26, true,  OP_GETCAMERASETTING)           \
	OP(27, true,  OP_SETCAMERASETTING)           \
	OP(28, true,  OP_SAVEVISIONSETTINGS)         \
	OP(29, true,  OP_SETFEETSPACE)               \
	OP(30, true,  OP_GETFEETSPACE)               \
	OP(34, true,  OP_READMOTORREGISTERS)         \
	OP(35, true,  OP_SETIMAGECENTER)             \
	OP(36, true,  OP_GETIMAGECENTER)             \
	OP(39, false, OP_WHITECALIBRATION)           \
	OP(42, false, OP_MESSAGE)                    \


#define OP(id,log,op) op = id,
/// all operations that are available over the UDP interface
typedef enum {
	OPERATIONS
	OP_LAST_OPERATION
} OPERATION;
#undef OP


/*------------------------------------------------------------------------------------------------*/

/// operationflags for the msg
typedef enum {
	FLAG_IS_ANSWER       = 1
} OPERATIONFLAGS;

/// loglevel for the logging facility
typedef enum {
	LOG_DEBUG = 0,
	LOG_INFO  = 1,
	LOG_WARN  = 2,
	LOG_ERROR = 3
} LOGLEVEL;

/// logsubsystem indicate the source of the log msg
typedef enum {
	LOG_GENERAL  = 0,
	LOG_BEHAVIOR = 1,
	LOG_COMM     = 2,
	LOG_MOTION   = 3,
	LOG_MOTORBUS = 4,
	LOG_ROLE     = 5,
	LOG_STRATEGY = 6,
	LOG_VISION   = 7
} LOGSUBSYSTEM;

#define COMM_HEADER_SIZE 4


/*------------------------------------------------------------------------------------------------*/

class OperationCallback {
public:
	virtual ~OperationCallback() {}

	virtual bool operationCallback(
			OPERATION operation,
			uint8_t   flags,
			uint8_t  *data,
			uint16_t  dataLen,
			struct sockaddr_in *remoteAddress) = 0;
};

class OperationRegistration {
public:
	OperationRegistration(
			OperationCallback *_cb,
			OPERATION          _operation,
			int16_t            _minPayloadSize=-1,
			int16_t            _maxPayloadSize=-1
	)
	: operation(_operation)
	, cb(_cb)
	, minPayloadSize(_minPayloadSize)
	, maxPayloadSize(_maxPayloadSize)
	{
	}

	friend class Comm;

private:
	OPERATION          operation;       /// the operation being registered
	OperationCallback *cb;              /// who to call back when such an operation is received

	int16_t            minPayloadSize;  /// min size of payload in bytes
	int16_t            maxPayloadSize;  /// max size of payload in bytes
};


/*------------------------------------------------------------------------------------------------*/

/**
 ** Comm maintains the connection to server and acts as the remote control interface.
 **
 ** The server object is the heart of all that involves parties outside this particular robot,
 ** i.e. other robots, the server and any human trying to interact with the robot.
 **
 ** All messages (in- and outgoing) are constructed in the exact same way to allow for a transparent
 ** processing. As we use UDP, the message size is known and doesn't need to be encoded specially.
 **
 ** See doc/comm.txt for details on message format and parameters.
 **
 */
class Comm : public Singleton<Comm>, public Thread {
private:
	/// constructor (protected, this is a singleton)
	Comm();

	friend class Singleton<Comm>;

protected:
	/// transport channel
	Transport *transport;

	// default recipient
	struct sockaddr_in *defaultRecipient;

	/// names of the operations we support
	std::map<OPERATION, std::string> operationNames;

	/// whether to log an operation
	std::map<OPERATION, bool> logOperation;

	/// operation callbacks
	std::vector<OperationRegistration> operationRegistrations;

	/// critical section
	CriticalSection cs;

	/// time we last asked for the correct time
	robottime_t lastTimeCheck;

	/// amount of data send and read in current round
	uint32_t     totalBytesSent;
	uint32_t     totalBytesRead;

	/// thread main function, waits for incoming packets
	void threadMain();

	void handleOperation(OPERATION operation, uint8_t flags, struct sockaddr_in *remoteAddress, uint8_t *data, uint16_t dataLen);
	void handleMessage(const de::fumanoids::message::Message &msg);

	void logCommand(const char* commandName, uint8_t* data, uint16_t dataLen, struct sockaddr_in *remoteAddress);

	bool processInitCommand(const uint8_t *data, uint16_t dataLen, struct sockaddr_in *remoteAddress);
	bool processPlayMotionCommand(uint8_t *data, uint16_t dataLen, struct sockaddr_in *remoteAddress);
	bool processSetLogLevelCommand(uint8_t *data, uint16_t dataLen, struct sockaddr_in *remoteAddress);
	bool processTimeSyncCommand(uint8_t *data, uint16_t dataLen, struct sockaddr_in *remoteAddress);

  public:
	virtual ~Comm();

	/// get a descriptive name of the thread
	virtual const char* getName() {
		return "Comm";
	}

	bool init();

	bool registerOperationCallback(OperationCallback* callback, OPERATION operation, int16_t minPayloadSize=-1, int16_t maxPayloadSize=-1);
	bool unregisterOperationCallback(OperationCallback* callback, OPERATION operation);

	void sendMessage(OPERATION operation, uint8_t flags, uint8_t *data, uint16_t dataLen, struct sockaddr_in *recipient=0);
	void sendMessage(de::fumanoids::message::Message &message, uint8_t flags=0, struct sockaddr_in *recipient=0);
};


/** @} */ // end of group comm

#endif
