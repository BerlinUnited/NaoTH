/**
* @file SPLGameController.cpp
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "SPLGameController.h"
#include "NetUtils.h"

//#include <PlatformInterface/Platform.h>

#include <Tools/ThreadUtil.h>

#include <cstdlib>

#ifdef WIN32
#include <winsock.h>
#else // Linux/MACOS
#include <sys/socket.h>
#endif

using namespace naoth;
using namespace std;

SPLGameController::SPLGameController()
  : exiting(false),
    returnPort(GAMECONTROLLER_RETURN_PORT),
    socket(NULL),
    gamecontrollerAddress(NULL)
{
  GError* err = bindAndListen();
  if(err)
  {
    std::cout << "[WARN] could not listen for SPLGameController: " << err->message << std::endl;
    socket = NULL;
    g_error_free(err);
  }
  else
  {
    // init player number, team number and etc.
    //data.loadFromCfg( naoth::Platform::getInstance().theConfiguration );

    cancelable = g_cancellable_new();

    std::cout << "[INFO] SPLGameController start socket thread" << std::endl;
    socketThread = std::thread(&SPLGameController::socketLoop, this);
    ThreadUtil::setPriority(socketThread, ThreadUtil::Priority::lowest);
    ThreadUtil::setName(socketThread, "GameController");
  }
}

GError* SPLGameController::bindAndListen(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  
  if(err) { 
    return err;
  }

  g_socket_set_blocking(socket, true);

  // TODO: check and remove: broadcast option is only necessary for sending to broadcast.
  // SPLGameController sends messages directly to GameController.
  // NOTE: needs newer glib 2.36
  //  g_socket_set_broadcast(socket, true);
  //NetUtils::my_g_socket_set_broadcast(socket, true);

  // setup the socket for receiving messages at the port
  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, static_cast<guint16>(port));

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  return err;
}


bool SPLGameController::update()
{
  data.valid = false;

  // check the header and the version
  std::string header;
  header.assign(dataIn.header, 4);
  if(header == GAMECONTROLLER_STRUCT_HEADER && dataIn.version == GAMECONTROLLER_STRUCT_VERSION)
  {
    // dataOut.teamNum - is my team number
    // check if my team number was set and only accept the game data that was sent to my team
    if( dataOut.teamNum > 0 && 
       (dataIn.teams[0].teamNumber == dataOut.teamNum || dataIn.teams[1].teamNumber == dataOut.teamNum)
      ) 
    {
      data.parseFrom(dataIn, dataOut.teamNum);
      data.valid = true;
    }
  } // end if header correct
  else
  {
    std::cerr << "[SPLGameController] Invalid header and/or version ("
              << " header: " << header << " != " << GAMECONTROLLER_STRUCT_HEADER
              << " ; version: " << GAMECONTROLLER_STRUCT_VERSION << " != " << dataIn.version
              << ")!" << std::endl;
  }

  return data.valid;
}


void SPLGameController::get(GameData& gameData)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() )
  {
    if(data.valid) {
      gameData = data;
      data.valid = false; // invalidate after copy
    } else {
      // no new message received
      gameData.valid = false;
    }
  }
}

void SPLGameController::set(const naoth::GameReturnData& data)
{
  std::unique_lock<std::mutex> lock(returnDataMutex, std::try_to_lock);
  if ( lock.owns_lock() ) {
    data.writeTo(dataOut);
  }
}

SPLGameController::~SPLGameController()
{
  std::cout << "[SPLGameController] stop wait" << std::endl;
  // request the thread to stop
  exiting = true;

  // notify all waiting connections to cancel
  g_cancellable_cancel(cancelable);

  if(socketThread.joinable()) {
    socketThread.join();
  }

  if(socket != NULL) {
    g_object_unref(socket);
  }

  if(gamecontrollerAddress != NULL) {
    g_object_unref(gamecontrollerAddress);
  }
  g_object_unref(cancelable);
  std::cout << "[SPLGameController] stop done" << std::endl;
}

void SPLGameController::sendData(const RoboCupGameControlReturnData& data)
{
  if(gamecontrollerAddress != NULL)
  {
    return;
  }

  GError *error = NULL;
  gssize result = g_socket_send_to(socket, gamecontrollerAddress, (char*)(&data), sizeof(data), cancelable, &error);
  if (error) 
  {
    std::cout << "[WARN] g_socket_send_to error: " << error->message << std::endl;
    g_error_free(error);
  }
  else if ( result != sizeof(data) ) 
  {
    std::cout << "[WARN] SPLGameController::returnData, error wrong size sent: data size = " << sizeof(data) << ", sent size = " <<  result << std::endl;
  }
}

void SPLGameController::socketLoop()
{
  while(!exiting && socket != NULL)
  {
    GError* err = NULL;
    GSocketAddress* senderAddress = NULL;
    gssize size = g_socket_receive_from(socket, &senderAddress,
                                     (char*)(&dataIn),
                                     sizeof(RoboCupGameControlData),
                                     cancelable, &err);

    if (err) {
      std::cout << "[WARN] SPLGameController g_socket_receive_from error: " << err->message << std::endl;
      g_error_free(err);
    } 
    else {
      // construct a return address with the given returnPort
      if(senderAddress != NULL)
      {
        GInetAddress* rawAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(senderAddress));
        if(gamecontrollerAddress != NULL) {
          g_object_unref(gamecontrollerAddress);
        }
        gamecontrollerAddress = g_inet_socket_address_new(rawAddress, static_cast<guint16>(returnPort));
        g_object_unref(senderAddress);
      }

      if(size == sizeof(RoboCupGameControlData))
      {
        bool validPackage = false;
        {
          std::lock_guard<std::mutex> lock(dataMutex);
          validPackage = update();
        }

        // only send return package if we are sure the initial package was a proper game controller message
        if(validPackage)
        {
          std::lock_guard<std::mutex> lock(returnDataMutex);
          sendData(dataOut);
        }
      }
    }
  }
}

