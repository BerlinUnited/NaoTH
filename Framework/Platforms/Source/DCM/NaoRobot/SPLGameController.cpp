#include "SPLGameController.h"
#include <cstdlib>
#include <PlatformInterface/Platform.h>
#include <sys/socket.h>
#include "Tools/Communication/NetAddr.h"
#include <Tools/ThreadUtil.h>

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

    // init return data
    strcpy(dataOut.header, GAMECONTROLLER_RETURN_STRUCT_HEADER);
    dataOut.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
    dataOut.team = 0;
    dataOut.player = 0;
    dataOut.message = GAMECONTROLLER_RETURN_MSG_ALIVE;

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
  if(err) return err;
  g_socket_set_blocking(socket, true);

  int broadcast = 1;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

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

  // check the header
  std::string header;
  header.assign(dataIn.header, 4);
  if(header == GAMECONTROLLER_STRUCT_HEADER && dataIn.version == GAMECONTROLLER_STRUCT_VERSION)
  {
    // team number was set
    if( dataOut.team > 0 && 
       (dataIn.teams[0].teamNumber == dataOut.team || dataIn.teams[1].teamNumber == dataOut.team)
      ) 
    {
      data.parseFrom(dataIn, dataOut.team);
      data.valid = true;
    }
  } // end if header correct

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
  if ( lock.owns_lock() )
  {
    if(data.message == GameReturnData::dead) {
      // set to invalid data to avoid sending the message
      dataOut.player = 0;
      dataOut.team = 0;
      dataOut.message = data.message;
    } else {
      dataOut.player = (uint8_t)data.player;
      dataOut.team = (uint8_t)data.team;
      dataOut.message = data.message;
    }
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
  GError *error = NULL;
  if(gamecontrollerAddress != NULL)
  {
    gssize result = g_socket_send_to(socket, gamecontrollerAddress, (char*)(&data), sizeof(data), cancelable, &error);
    if ( result != sizeof(data) ) {
      std::cout << "[WARN] SPLGameController::returnData, sended size = " <<  result << std::endl;
    }
    if (error) {
      std::cout << "[WARN] g_socket_send_to error: " << error->message << std::endl;
      g_error_free(error);
    }
  }
}

void SPLGameController::socketLoop()
{
  while(!exiting && socket != NULL)
  {
    GSocketAddress* senderAddress = NULL;
    int size = g_socket_receive_from(socket, &senderAddress,
                                     (char*)(&dataIn),
                                     sizeof(RoboCupGameControlData),
                                     cancelable, NULL);

    if(senderAddress != NULL)
    {
      // construct a proper return address from the receiver
      GInetAddress* rawAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(senderAddress));
      if(gamecontrollerAddress != NULL)
      {
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

