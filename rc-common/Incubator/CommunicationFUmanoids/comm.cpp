#include "comm.h"

#include "motion/motion.h"
#include "motion/motion_static.h"
#include "system/timer.h"
#include "robot.h"
#include "config/configRegistry.h"
#include "transport/transport_udp.h"
#include "behaviorLayer/xabsl/Symbols/StrategySymbols.h"
#include "behaviorLayer/roleDecider.h"
#include "behaviorLayer/behaviorControl.h"
#include "simulation/sim_worldModel.h"
#include "debug.h"
#include "messageRegistry.h"
#include "worldModel/worldModel.h"

#include <string>
#include <sstream>


/*------------------------------------------------------------------------------------------------*/

REGISTER_OPTION("comm.port",        DEFAULTCOMMPORT, "Port for communication (default 11011)");
REGISTER_OPTION("comm.remote.ip",   "",              "Remote IP to be used instead of broadcast packages");
REGISTER_OPTION("comm.remote.port", DEFAULTCOMMPORT, "Remote port for broadcast");


/*------------------------------------------------------------------------------------------------*/

/**
 ** Constructor
**/
Comm::Comm()
	: transport(0)
	, defaultRecipient(0)
{
	cs.setName("Comm::cs");

	lastTimeCheck = 0;

	#define OP(id,log,op) operationNames[(OPERATION)id] = #op;
	OPERATIONS
	#undef OP

	#define OP(id,log,op) logOperation[(OPERATION)id] = log;
	OPERATIONS
	#undef OP
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** initialize the Comm object.
**/
bool Comm::init() {
	CriticalSectionLock lock(cs);

	int localPort  = robot.getConfig().getIntValue("comm.port",       DEFAULTCOMMPORT);
	int remotePort = robot.getConfig().getIntValue("comm.remote.port", localPort);

	transport = new TransportUDP( localPort, remotePort, true );
	if (transport->open() == false)
		return false;

	// assemble default recipient
	std::string remoteIP = robot.getConfig().getStrValue("comm.remote.ip", "");
	if (remoteIP != "") {
		defaultRecipient = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
		defaultRecipient->sin_family      = AF_INET;
		defaultRecipient->sin_addr.s_addr = inet_addr(remoteIP.c_str());
		defaultRecipient->sin_port        = htons(remotePort);
	}

	// start thread
	run();
	return true;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Destructor
**/
Comm::~Comm() {
	kill();

	if (transport) {
		transport->close();
		delete transport;
		transport = 0;
	}
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Register a callback for an operation
 **
 ** @param callback        Object to call back when operation is received
 ** @param operation       Operation to register
 ** @param minPayloadSize  Min size in bytes of payload to require
 ** @param maxPayloadSize  Max size in bytes of payload to accept
 **
 ** @return true iff registration succeeded
 */


/*------------------------------------------------------------------------------------------------*/

bool Comm::registerOperationCallback(OperationCallback* callback, OPERATION operation, int16_t minPayloadSize, int16_t maxPayloadSize) {
	OperationRegistration opReg(callback, operation, minPayloadSize, maxPayloadSize);
	operationRegistrations.push_back(opReg);
	return true;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Unregister a callback for an operation
 **
 ** @param callback    Object that was registered to be called back
 ** @param operation   Operation that the callback was registered for
 **
 ** @return true iff unregistration succeeded
**/

bool Comm::unregisterOperationCallback(OperationCallback* callback, OPERATION operation) {
	CriticalSectionLock lock(cs);

	for (uint16_t i=0; i < operationRegistrations.size(); i++) {
		if (operationRegistrations[i].cb == callback && operationRegistrations[i].operation == operation) {
			operationRegistrations.erase(operationRegistrations.begin() + i);
			return true;
		}
	}

	return false;
}


/*------------------------------------------------------------------------------------------------*/

#define BUFFSIZE 65536

/**
 ** Handles incoming data.
 **
 ** The thread code is a simple loop receiving messages and dispatching them to handleOperation().
 ** Note that messages are processed in the order they are received, and one after the other.
**/
void Comm::threadMain() {
	robottime_t lastStatsTime = getCurrentTime();
	totalBytesSent = 0;
	totalBytesRead = 0;

	while (isRunning()) {
		if (transport->waitForData(1, 500*1000 /* microseconds */)) {
			struct sockaddr_in remoteAddress;
			uint8_t buffer[BUFFSIZE];
			int received = ((TransportUDP*)transport)->read(buffer, BUFFSIZE, &remoteAddress);
			if (received < 0) {
				// TODO: handle error in receiving
				continue;
			} else {
				totalBytesRead += received;
			}

			// packages must at least be as big as the header, ignore everything else
			if (received >= COMM_HEADER_SIZE) {
				OPERATION operation = (OPERATION)ntohs(*(uint16_t*)(void*)buffer);
				handleOperation(operation, buffer[2], &remoteAddress, buffer + COMM_HEADER_SIZE, received - COMM_HEADER_SIZE);
			}
		}

		if (getCurrentTime() - lastStatsTime > 10*1000) {
			cs.enter();
			int outAvg = (int)(totalBytesSent/ ( (getCurrentTime() - lastStatsTime)/1000.0));
			int inAvg  = (int)(totalBytesRead/ ( (getCurrentTime() - lastStatsTime)/1000.0));
			lastStatsTime  = getCurrentTime();
			totalBytesSent = 0;
			totalBytesRead = 0;
			cs.leave();

			INFO("Average communication (only Comm): outgoing %d bytes/s, incoming %d bytes/s", outAvg, inAvg);
		}
	}
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Sends a message
 **
 ** @param  operation    ID of operation
 ** @param  flags        Flags
 ** @param  data         Payload
 ** @param  dataLen      Number of bytes in payload
 ** @param  recipient    Address of recipient; set to 0 for broadcast
**/
void Comm::sendMessage(OPERATION operation, uint8_t flags, uint8_t *data, uint16_t dataLen, struct sockaddr_in *recipient) {
	if (transport == 0)
		return;

	// assemble message
	uint16_t  msgLen = COMM_HEADER_SIZE + dataLen;
	uint8_t  *msg    = (uint8_t*)malloc(msgLen);

	if (msg == 0) {
		ERROR("ERROR allocating memory for message structure");
		return;
	}

	uint16_t operationBE = htons(operation);
	memset(msg, 0, COMM_HEADER_SIZE);
	memcpy(msg,   &operationBE, 2);
	memcpy(msg+2, &flags, 1);

	if (data != NULL && dataLen > 0)
		memcpy(msg+COMM_HEADER_SIZE, data, dataLen);

	// if we have a default recipient, use it instead of a broadcast
	if (recipient == 0)
		recipient = defaultRecipient;

	// actually send data
	cs.enter();
	int bytesWritten = ((TransportUDP*)transport)->write(msg, msgLen, recipient);
	if (bytesWritten < 0)
		ERROR("error sending message: %s", strerror(errno));
	else
		totalBytesSent += bytesWritten;
	cs.leave();

	free(msg);
	msg = 0;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Sends a protobuf message
 **
 ** @param  message      Protobuf message
 ** @param  flags        Flags
 ** @param  recipient    Address of recipient; set to 0 for broadcast
**/
void Comm::sendMessage(de::fumanoids::message::Message &message, uint8_t flags, struct sockaddr_in *recipient) {
	if (transport == 0)
		return;

	// include our id
	message.set_robotid( robot.getID() );

	if (message.IsInitialized() == false) {
		WARNING("Message sending failed due to wrong initialization: %s", message.InitializationErrorString().c_str());
		return;
	}

	int dataLen = message.ByteSize();
	if (dataLen > 65000) {
		ERROR("Message is too large for UDP");
		return;
	} else if (dataLen > 16394) {
		WARNING("Message is a bit large (%d bytes) for efficient transport", dataLen);
	}

	// assemble message
	uint16_t  msgLen = COMM_HEADER_SIZE + 8 + dataLen;
	uint8_t  *msg    = (uint8_t*)malloc(msgLen);

	if (msg == 0) {
		ERROR("ERROR allocating memory for message structure");
		return;
	}

	uint16_t operationBE = htons(OP_MESSAGE);
	memset(msg, 0, COMM_HEADER_SIZE);
	memcpy(msg,   &operationBE, 2);
	memcpy(msg+2, &flags, 1);

	int dataLenBE = htonl(dataLen);
	memset(msg+4, 0,          4);
	memcpy(msg+8, &dataLenBE, 4);

	if (!message.SerializeToArray(msg+COMM_HEADER_SIZE+8, dataLen)) {
		ERROR("Could not serialize message");
		return;
	}

	// if we have a default recipient, use it instead of a broadcast
	if (recipient == 0)
		recipient = defaultRecipient;

	// actually send data
	cs.enter();
	int bytesWritten = ((TransportUDP*)transport)->write(msg, msgLen, recipient);
	if (bytesWritten < 0)
		ERROR("error sending message: %s\n", strerror(errno));
	else
		totalBytesSent += bytesWritten;
	cs.leave();

	free(msg);
}


/*------------------------------------------------------------------------------------------------*/


typedef struct {
	OPERATION    operation;
	const char*  operationName;
	int32_t      size;
	bool         answer;
	bool         log;
} OperationCheck;

OperationCheck opCheck[] = {
	// operation ID       operation name       size  answer log
	{ OP_PLAYMOTION,      "OP_PLAYMOTION",      -1,  false, true  },
	{ OP_TIMESYNC,        "OP_TIMESYNC",         4,  true,  true  },
	{ OP_NO_OP,           "",                   -1,  false, false }
};


/*------------------------------------------------------------------------------------------------*/

/**
 ** Processes an operation.
 **
 ** @param operation       received operation
 ** @param flags           message flags
 ** @param remoteAddress   Address to send response to (NULL for broadcast)
 ** @param data            Pointer to data block
 ** @param dataLen         Size of data block
**/
void Comm::handleOperation(OPERATION operation, uint8_t flags, struct sockaddr_in *remoteAddress, uint8_t *data, uint16_t dataLen) {
	if (operation < OP_LAST_OPERATION && logOperation[operation] && !(flags & FLAG_IS_ANSWER)) {
		logCommand(operationNames[operation].c_str(), data, dataLen, remoteAddress);
	}

	for (uint16_t i=0; i < operationRegistrations.size(); i++) {
		if (operationRegistrations[i].operation == operation) {
			int16_t minSize = operationRegistrations[i].minPayloadSize;
			int16_t maxSize = operationRegistrations[i].maxPayloadSize;
			if (    (minSize >= 0 && minSize > dataLen)
			     || (maxSize >= 0 && maxSize < dataLen))
			{
				ERROR("Operation %s has incorrect data length (expected %d-%d, but got %d bytes)", operationNames[operation].c_str(), minSize, maxSize, dataLen);
			} else {
				operationRegistrations[i].cb->operationCallback(operation, flags, data, dataLen, remoteAddress);
			}
		}
	}

	for (int i=0; opCheck[i].operation != OP_NO_OP; i++) {
		if (opCheck[i].operation == operation) {
			bool isAnswer = (flags & FLAG_IS_ANSWER) != 0;
			if (isAnswer ^ opCheck[i].answer)
				return;

			if (opCheck[i].size != -1 && opCheck[i].size != dataLen) {
				ERROR("Operation %s has incorrect data length (expected %d, but got %d)", opCheck[i].operationName, opCheck[i].size, dataLen);
				return;
			}
			break;
		}
	}

	// handle operation
	switch (operation)  {
	case OP_MESSAGE:
		{
			if (dataLen < 8)
				break;

			int32_t msgType, msgSize;
			memcpy(&msgType, data, 4);
			msgType = ntohl(msgType);

			if (msgType != 0)
				WARNING("Received unknown message type %d.", msgType);

			memcpy(&msgSize, data + 4, 4);
			msgSize = ntohl(msgSize);

			if (msgSize > dataLen - 8) {
				WARNING("Received message with incorrect size (got %d, expected %d)", dataLen-8, msgSize);
				break;
			}

			de::fumanoids::message::Message msg;
			msg.ParseFromArray(data + 8, msgSize);

			if (msg.IsInitialized() == false)
				WARNING("Received incorrect message: %s", msg.InitializationErrorString().c_str());
			else
				handleMessage(msg);
		}
		break;

	case OP_PLAYMOTION:
		processPlayMotionCommand(data, dataLen, remoteAddress);
		break;

	case OP_TIMESYNC:
		if (flags & FLAG_IS_ANSWER)
			processTimeSyncCommand(data, dataLen, remoteAddress);
		break;

	default:
		break;
	}

	return;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Evaluate incoming proto messages.
 **
 ** @param msg Message to handle
 */

void Comm::handleMessage(const de::fumanoids::message::Message &msg) {
	// execute callbacks
	MessageRegistry::getInstance().handleMessage(msg, msg.robotid());

	// handle world model infos from other robots or the simulator
	// as sim comes up with the same robot id, this code MUST be first
	if (msg.has_worldmodel()) {
		wm.updateNetwork(msg.worldmodel());
	}

	// from now on, ignore messages that came from us
	if (msg.robotid() == robot.getID())
		return;

	// handle captain commands from other robots
	if (msg.has_captaincommands()) {
		if (robot.isBehaviorControlRunning()) {
			robot.getBehaviorControl()->getRoleDecider()->handleCaptainCommands(msg.captaincommands(), msg.robotid());
		}
	}

}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Outputs a log message about a received command, including sender IP and some excerpt
 ** from the data block.
 **
 ** @param commandName    Name of command
 ** @param data           Pointer to data block (may be NULL)
 ** @param dataLen        Length of data block
 ** @param remoteAddress  Address this command was sent from
 */
void Comm::logCommand(const char* commandName, uint8_t* data, uint16_t dataLen, struct sockaddr_in *remoteAddress) {
	std::string formattedData, sender;
	if (dataLen > 0) {
		formattedData = ": ";

		const int maxValues = 15;
		for (int i = 0; i < dataLen && i < maxValues; i++) {
			char tmp[16];

			if (isgraph(data[i]))
				sprintf(tmp, "%c ", data[i]);
			else
				sprintf(tmp, "%02x ", data[i]);

			formattedData += tmp;
		}

		if (dataLen > maxValues)
			formattedData += "...";
	}

	if (remoteAddress) {
		char tmp[32];
		uint32_t address = (uint32_t)remoteAddress->sin_addr.s_addr;
		uint8_t *addressP = (uint8_t*)&address;
		sprintf(tmp, "%d.%d.%d.%d", addressP[0], addressP[1], addressP[2], addressP[3]);
		sender = tmp;
	} else
		sender = "Unknown";

	INFO("Received command %s from %s with %d bytes of data %s", commandName, sender.c_str(), dataLen, formattedData.c_str());
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Process Play Motion command
 **
 ** @param data           Pointer to data block (may be NULL)
 ** @param dataLen        Length of data block
 ** @param remoteAddress  Address this command was sent from
 **
 ** @return true if command was successful
 */
bool Comm::processPlayMotionCommand(uint8_t *data, uint16_t dataLen, struct sockaddr_in *remoteAddress) {
	int16_t motorCount = data[0];
	int16_t rowCount   = data[1];

	int16_t expectedDataLen = 2 + sizeof(int16_t) * (motorCount + 1) * rowCount;
	if (expectedDataLen != dataLen) {
		ERROR("Play motion failed because data array (%d bytes) was not of expected size (%d bytes)", dataLen, expectedDataLen);
		return false;
	}

	if (motorCount != MOTOR_LAST_MOTOR_IN_STATIC - MOTOR_FIRST_MOTOR_IN_STATIC + 1) {
		ERROR("Play motion failed because the data sent is for %d motors but this robot handles %d motors.", motorCount, MOTOR_LAST_MOTOR_IN_STATIC - MOTOR_FIRST_MOTOR_IN_STATIC + 1);
		return false;
	}

	// convert from network to host byteorder
	int16_t cellCount = (motorCount+1)*rowCount;
	int16_t *data16 = new int16_t[cellCount];

	for (int i=0; i < cellCount; i++) {
		int16_t value;
		memcpy(&value, data + 2 + i*2, 2);
		data16[i] = ntohs(value);
	}

	// play custom motion and wait for it to finish
	INFO("Playing motion (%d rows)", rowCount);
	robot.switchMotion(new MotionStatic(M_Custom, data16, rowCount), true);

	delete[] data16;
	INFO("Custom motion finished playing");
	return true;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Process Time Sync command
 **
 ** @param data           Pointer to data block (may be NULL)
 ** @param dataLen        Length of data block
 ** @param remoteAddress  Address this command was sent from
 **
 ** @return true if command was successful
 */
bool Comm::processTimeSyncCommand(uint8_t *data, uint16_t dataLen, struct sockaddr_in *remoteAddress) {
	uint32_t time;
	memcpy(&time, data, 4);
	time = ntohl(time);
	printf("Got timesync command, time %d (0x%x) seconds \n", time, time);

	struct timeval tv = { time, 0 };
	settimeofday(&tv, 0);
	return true;
}
