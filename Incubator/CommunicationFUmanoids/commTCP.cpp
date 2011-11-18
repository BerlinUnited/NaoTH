/** @file
 **
 ** Handles TCP connection to remote application.
 **
 **
 */

#include "comm.h"
#include "commTCP.h"
#include "system/timer.h"
#include "robot.h"
#include "debug.h"

#include "vision/vision.h"
#include "vision/imagePresenter.h"

#include <string>


/*------------------------------------------------------------------------------------------------*/

/**
 ** Constructor
**/

CommTCP::CommTCP()
	: server(0)
	, transport(0)
{
	cs.setName("CommTCP::cs");
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Destructor
**/

CommTCP::~CommTCP() {
	kill();

	// close transport
	if (server) {
		server->close();
		delete server;
	}

	server = 0;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Constructor
**/

void CommTCP::init() {
	CriticalSectionLock lock(cs);

	server = new TransportTCPServer(robot.getConfig().getIntValue("comm.port", DEFAULTCOMMPORT));
	if (server->init() == false)
		return;

	// start thread
	run();
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Thread main function, waits for incoming connections.
 **
**/

void CommTCP::threadMain() {
	while (isRunning()) {
		Transport *transport = server->waitForIncoming(500*1000 /* µs */);
		if (transport) {
			// read request and handle it
			CommTCPHeader header = { 0 };
			int count = transport->read((uint8_t*)&header, sizeof(header));
			if (count == sizeof(header))
				handle(header, transport);

			transport->close();
			delete transport;
			transport = 0;
		}
	}
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Sends a protobuf message
 **
 ** @param  message      Protobuf message
 ** @param  recipient    Address of recipient
**/
void CommTCP::sendMessage(de::fumanoids::message::Message &message, const char *recipient) {
	if (transport == 0) {
		transport = new TransportTCP(DEFAULTCOMMPORT+1, recipient);
		transport->open();
	} else if (!transport->isConnected())
		transport->open();

	if (!transport->isConnected()) {
		WARNING("[CommTCP] Cannot connect to %s - aborting", recipient);
		return;
	}

	///  just to be sure
	message.set_robotid(robot.getID());

	if (message.IsInitialized() == false) {
		WARNING("[CommTCP] Message sending failed due to wrong initialization: %s",
				message.InitializationErrorString().c_str());
		return;
	}

	if (recipient == 0) {
		WARNING("[CommTCP] No recipient given - aborting");
		return;
	}

	int     msg_len = message.ByteSize();
	uint8_t *msg = (uint8_t*) malloc(msg_len);
	if (msg == NULL) {
		WARNING("[CommTCP] Out of memory - aborting");
		return;
	}

	if (!message.SerializeToArray(msg, msg_len)) {
		WARNING("[CommTCP] Serialization of message failed - aborting");
		free(msg);
		return;
	}

	///  actually send data
	robottime_t startTime = getCurrentTime();
	int bytesWritten = transport->write(msg, msg_len);
	if (bytesWritten < 0)
		ERROR("[CommTCP] Error sending message: %s\n", strerror(errno));
	else {
		double duration = (getCurrentTime() - startTime) / 1000;
		printf("TCP: sent %d bytes in %.1f seconds (%.1f kbps)\n", bytesWritten, duration, bytesWritten/duration/1024);
	}

	free(msg);

	transport->close();
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Request dispatcher
 **
 ** @param request    Received request
 ** @param transport  Transport used for communication
**/

void CommTCP::handle(const CommTCPHeader &request, Transport *transport) {
	if (request.command == TCPCMD_GETIMAGE) {
		INFO("Received TCPCMD_GETIMAGE, param1 = 0x%x, param2 = 0x%x", request.parameter1, request.parameter2);
		sendImage(transport, request.version, request.parameter1, request.parameter2);
		return;

	} else if (request.command == TCPCMD_MESSAGE) {
		INFO("Received TCPCMD_MESSAGE, param1 = 0x%x, param2 = 0x%x", request.parameter1, request.parameter2);

		if (request.parameter1 == 0 /* receive data */) {
			if (request.parameter2 == 1 /* calibration */) {
				INFO("MSG: incoming calibration");
				std::string data;
				int32_t size = 0;
				if (transport->read((uint8_t*)&size, 4)) {
					data.resize(size);
					int dataRead = transport->read((uint8_t*)data.c_str(), size);
					if (dataRead == size) {
						INFO("MSG: calibration data received");
						ASSERT(0); // Vision::getInstance().calibration.load((uint8_t*)data.c_str(), size);
						INFO("MSG: calibration data set");
					} else
						INFO("MSG: failure receiving calibration data (%d read, %d expected)", dataRead, size);
				}

				return;
			}
		} else if (request.parameter1 == 1 /* data requested, send it out */) {
			if (request.parameter2 == 1 /* calibration */) {
				INFO("MSG: calibration data requested");

				std::string data;
				ASSERT(0); // Vision::getInstance().calibration.getData(&data);

				robottime_t startTime = getCurrentTime();

				int32_t size = data.size();
				transport->write((uint8_t*)&size, 4);
				int bytesWritten = transport->write((uint8_t*)data.c_str(), size);

				double duration = (getCurrentTime() - startTime) / 1000;
				printf("TCP: sent %d bytes in %.1f seconds (%.1f kbps)\n", bytesWritten, duration, bytesWritten/duration/1024);

				if (bytesWritten < (int)data.size())
					ERROR("Image sending incomplete, only sent %d bytes (%d missing)", bytesWritten, data.size() - bytesWritten);
				else
					INFO("Image sent");

				INFO("MSG: calibration data sent out");
				return;
			}
		} else if (request.parameter1 == 2 /* image requested */) {
//			CameraSensor::getInstance().sendImageData(transport, request.parameter2 == 0x01);
			return;
		}
	}

	WARNING("Unhandled message over TCP (cmd 0x%x (%d), param1 0x%x, param2 0x%x)", request.command, request.command, request.parameter1, request.parameter2);
}


/*------------------------------------------------------------------------------------------------*/


#define SENDIMAGE_OVERLAY_MASK         0xFC
#define SENDIMAGE_OVERLAY_OBJECTS      0x04
#define SENDIMAGE_OVERLAY_PARTICLES    0x08
#define SENDIMAGE_OVERLAY_PROJECTION   0x0c
#define SENDIMAGE_OVERLAY_GRADIENTS    0x10
#define SENDIMAGE_OVERLAY_LINEPOINTS   0x14
#define SENDIMAGE_OVERLAY_PROJECTED    0x18
#define SENDIMAGE_OVERLAY_UNDISTORTED  0x1c
#define SENDIMAGE_OVERLAY_LOCALISATION 0x20
#define SENDIMAGE_OVERLAY_UNUSED1      0x24
#define SENDIMAGE_OVERLAY_UNUSED2      0x28
#define SENDIMAGE_OVERLAY_UNUSED3      0x2c
#define SENDIMAGE_OVERLAY_UNUSED4      0x40
#define SENDIMAGE_OVERLAY_UNUSED5      0x44
#define SENDIMAGE_OVERLAY_UNUSED6      0x48
#define SENDIMAGE_OVERLAY_UNUSED7      0x4c

/** Send image.
 **
 ** @param transport  transport used for communication
 ** @param param1     Parameter 1, 'C' for compressed
 ** @param param2     ignored
**/
void CommTCP::sendImage(Transport *transport, uint8_t version, uint8_t param1, uint8_t param2) {
	if (version != '1')
		return;

	/*
	 \0  -> unkomprimiert YUV422 (raw)
	 'C' -> komprimiert YUV422 (raw)
	 'I' -> IplImage
	*/
	switch (param1) {
	case 'I': // IplImage
		{
			int scale = (param2 & 0x03) + 1;
			ImageOverlayModus overlay = IMAGEOVERLAY_NORMAL;
			ImagePresentationMode mode = IP_MODE_NORMAL;

			switch (param2 & SENDIMAGE_OVERLAY_MASK) {
			case SENDIMAGE_OVERLAY_OBJECTS:    overlay = IMAGEOVERLAY_OBJECTS;    break;
			case SENDIMAGE_OVERLAY_PARTICLES:  overlay = IMAGEOVERLAY_EDGES;      break;
			case SENDIMAGE_OVERLAY_PROJECTION: overlay = IMAGEOVERLAY_PROJECTION; break;
			case SENDIMAGE_OVERLAY_GRADIENTS:  overlay = IMAGEOVERLAY_GRADIENT;   break;
			case SENDIMAGE_OVERLAY_LINEPOINTS: overlay = IMAGEOVERLAY_LINEPOINTS; break;
			case SENDIMAGE_OVERLAY_UNDISTORTED: mode = IP_MODE_UNDISTORTED; break;
			case SENDIMAGE_OVERLAY_PROJECTED:   mode = IP_MODE_PROJECTED; break;
			case SENDIMAGE_OVERLAY_LOCALISATION: mode = IP_MODE_LOCALIZATION; break;
			default: break;
			}

			ImagePresenter *imageStreamer = new ImagePresenter(scale, overlay, mode);

			imageStreamer->startStreaming(1, 1, transport);
			imageStreamer->waitForStreamingDone();
			delete imageStreamer;
			imageStreamer = 0;
			break;
		}

		break;
	}
}
