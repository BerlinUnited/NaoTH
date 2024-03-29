
#include "SPLGameController.h"

#include <PlatformInterface/Platform.h>
#include "Tools/Communication/NetAddr.h"
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

    // init return data
    // 
    // NOTE: subtle harmless "bug": copy zero terminated c-string (4+1 chars) 
    //       but header is only 4 chars long. 
    //       The terminal '\0' is copied into 'version' and is then overwritten
    // 
    // Is there a more elegant+safer way of doing this?
    strcpy(dataOut.header, GAMECONTROLLER_RETURN_STRUCT_HEADER);
    dataOut.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
    dataOut.playerNum = 0;
    dataOut.teamNum = 0;
    dataOut.fallen = 0;
    
    // new in 2022
    dataOut.pose[0] = 0;
    dataOut.pose[1] = 0;
    dataOut.pose[2] = 0;
    dataOut.ballAge = -1;
    dataOut.ball[0] = 0;
    dataOut.ball[1] = 0;
    

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

  // NOTE:
  // Set the broadcast option directly. GLib spoorts it starting version 2.36.
  // Linux and Windows let you set a single-byte value from an int,
  // but most other platforms don't.
  // https://github.com/GNOME/glib/blob/main/gio/gsocket.c#L6340
  // TODO: the following might not work on MACOS

#ifdef WIN32
  // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
  // https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
  BOOL broadcastFlag = TRUE;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), sizeof(broadcastFlag));
#else // Linux/MACOS
  // https://linux.die.net/man/3/setsockopt
  int broadcastFlag = 1;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), static_cast<socklen_t> (sizeof(int)));
#endif

  // NOTE: needs newer glib 2.36
  //g_socket_set_broadcast(socket, true);
  // or ...
  //g_socket_set_option (...);

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
  if ( lock.owns_lock() )
  {
    dataOut.playerNum = (uint8_t)data.playerNum;
    dataOut.teamNum   = (uint8_t)data.teamNum;
    dataOut.fallen    = data.fallen;

    dataOut.pose[0]   = (float) data.pose.translation.x;
    dataOut.pose[1]   = (float) data.pose.translation.y;
    dataOut.pose[2]   = (float) data.pose.rotation;

    // in seconds (only if positive)!
    dataOut.ballAge   = (float) ((data.ballAge < 0)? data.ballAge : data.ballAge / 1000.0);
    dataOut.ball[0]   = (float) data.ballPosition.x;
    dataOut.ball[1]   = (float) data.ballPosition.y;
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
    gssize size = g_socket_receive_from(socket, &senderAddress,
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

