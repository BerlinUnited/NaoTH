//	Serial.cpp - Implementation of the CSerial class
//
//	Copyright (C) 1999-2003 Ramon de Klein (Ramon.de.Klein@ict.nl)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


// this marker checs if the serial definitions 
// are avaliable on this platform
// (there is no serial on mac)
#ifdef SERIAL_IS_DEFINED

//////////////////////////////////////////////////////////////////////
// Include the standard header files

#define STRICT



//////////////////////////////////////////////////////////////////////
// Include module headerfile
#include "Serial.h"

#ifdef WIN32

//#define _RPTF0( level, message) LOG_ERROR(message);
//#define _RPTF2( level, message, param1, param2) _RPTF0(level,message)

//////////////////////////////////////////////////////////////////////
  // Disable warning C4127: conditional expression is constant, which
  // is generated when using the _RPTF and _ASSERTE macros.

  #pragma warning(disable: 4127)


  //////////////////////////////////////////////////////////////////////
  // Enable debug memory manager

  #ifdef _DEBUG

  #ifdef THIS_FILE
  #undef THIS_FILE
  #endif

  static const char THIS_FILE[] = __FILE__;
  #define new DEBUG_NEW

  #endif

//////////////////////////////////////////////////////////////////////
//Windows Helper methods

inline void CSerial::CheckRequirements (LPOVERLAPPED lpOverlapped, DWORD dwTimeout) const
{
    #ifdef SERIAL_NO_OVERLAPPED

      // Check if an overlapped structure has been specified
      if (lpOverlapped || (dwTimeout != INFINITE))
      {
        // Quit application
        // ::MessageBox(0,_T("Overlapped I/O and time-outs are not supported, when overlapped I/O is disabled."),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
        ::DebugBreak();
        ::ExitProcess(0xFFFFFFF);
      }

    #endif

    #ifdef SERIAL_NO_CANCELIO

      // Check if 0 or INFINITE time-out has been specified, because
      // the communication I/O cannot be cancelled.
      if ((dwTimeout != 0) && (dwTimeout != INFINITE))
      {
        // Quit application
        // ::MessageBox(0,_T("Timeouts are not supported, when SERIAL_NO_CANCELIO is defined"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
        ::DebugBreak();
        ::ExitProcess(0xFFFFFFF);
      }

    #endif	// SERIAL_NO_CANCELIO
  // Avoid warnings
  (void) dwTimeout;
  (void) lpOverlapped;
}

inline BOOL CSerial::CancelCommIo (void)
{
  #ifdef SERIAL_NO_CANCELIO
    // CancelIo shouldn't have been called at this point
    ::DebugBreak();
    return FALSE;
  #else

    // Cancel the I/O request
    return ::CancelIo(m_hFile);

  #endif	// SERIAL_NO_CANCELIO
}
#endif

//////////////////////////////////////////////////////////////////////
// Code

CSerial::CSerial ()
:
m_lLastError(ERROR_SUCCESS)
,
m_hFile(NO_FILE)
,
m_eEvent(EEventNone)
,
m_dwEventMask(0)
#ifndef SERIAL_NO_OVERLAPPED
,
m_hevtOverlapped(0)
#endif
{}

CSerial::~CSerial ()
{
	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile != NO_FILE)
	{
      // Display a warning
    errMsg(_CRT_WARN,"CSerial::~CSerial - Serial port not closed\n");

		// Close implicitly
		Close();
	}
}

void CSerial::errMsg(int type, std::string msg)
{
  #ifdef WIN32
    _RPTF0(type, msg);
  #elif __linux__
    std::cout << msg << std::endl;
  #endif
}

CSerial::EPort CSerial::CheckPort (LPCTSTR lpszDevice)
{
	#ifdef WIN32
    // Try to open the device
    HANDLE hFile = ::CreateFile(lpszDevice, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
  #elif __linux__
   std::string port = "/dev/tty" + lpszDevice;
   HANDLE hFile = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  #endif
	// Check if we could open the device
	if (hFile == INVALID_HANDLE_VALUE)
	{
    #ifdef WIN32
      // Display error
      switch (::GetLastError())
      {
        case ERROR_FILE_NOT_FOUND:
          // The specified COM-port does not exist
          return EPortNotAvailable;

        case ERROR_ACCESS_DENIED:
          // The specified COM-port is in use
          return EPortInUse;

        default:
          // Something else is wrong
          return EPortUnknownError;
      }
    #elif __linux__
      return EPortNotAvailable;
    #endif
	}

  // Close handle
  #ifdef WIN32
    ::CloseHandle(hFile);
  #elif __linux__
    close(hFile);
  #endif
	// Port is available
	return EPortAvailable;
}

LONG CSerial::Open (LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue, bool fOverlapped)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the port isn't already opened
	if (m_hFile != NO_FILE)
	{
		m_lLastError = ERROR_ALREADY_INITIALIZED;
    errMsg(_CRT_WARN,"CSerial::Open - Port already opened\n");
		return m_lLastError;
	}
  #ifdef WIN32
    // Open the device
    m_hFile = ::CreateFile(lpszDevice, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, fOverlapped ? FILE_FLAG_OVERLAPPED : 0, 0);
  #elif __linux__
    std::string port = "/dev/tty" + lpszDevice;
    std::cout << port << std::endl;
    m_hFile = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  #endif

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		// Reset file handle
		m_hFile = NO_FILE;

		// Display error
    #ifdef WIN32
      m_lLastError = ::GetLastError();
    #endif
      errMsg(_CRT_WARN, "CSerial::Open - Unable to open port\n");
		return m_lLastError;
  }
  else
  {
    errMsg(_CRT_WARN,"CSerial::Open - Port opened\n");
  }

  #ifndef SERIAL_NO_OVERLAPPED
    // We cannot have an event handle yet
    _ASSERTE(m_hevtOverlapped == 0);

    // Create the event handle for internal overlapped operations (manual reset)
    if (fOverlapped)
    {
      m_hevtOverlapped = ::CreateEvent(0,true,false,0);
      if (m_hevtOverlapped == 0)
      {
        // Obtain the error information
        m_lLastError = ::GetLastError();
        errMsg(_CRT_WARN,"CSerial::Open - Unable to create event\n");

        // Close the port
        ::CloseHandle(m_hFile);
        m_hFile = NO_FILE;

        // Return the error
        return m_lLastError;
      }
    }
  #else
    // Overlapped flag shouldn't be specified
    _ASSERTE(!fOverlapped);
  #endif

	// Setup the COM-port
	if (dwInQueue || dwOutQueue)
	{
		// Make sure the queue-sizes are reasonable sized. Win9X systems crash
		// if the input queue-size is zero. Both queues need to be at least
		// 16 bytes large.
		_ASSERTE(dwInQueue >= 16);
		_ASSERTE(dwOutQueue >= 16);

    #ifdef WIN32
      if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
      {
        // Display a warning
        long lLastError = ::GetLastError();
  			errMsg(_CRT_WARN,"CSerial::Open - Unable to setup the COM-port\n");

        // Close the port
        Close();

        // Save last error from SetupComm
        m_lLastError = lLastError;
        return m_lLastError;
      }
    #endif
	}

	// Setup the default communication mask
	SetMask();

	// Non-blocking reads is default
	SetupReadTimeouts(EReadTimeoutNonblocking);

  #ifdef WIN32
    // Setup the device for default settings
    COMMCONFIG commConfig = {0};
    DWORD dwSize = sizeof(commConfig);
    commConfig.dwSize = dwSize;
    if (::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize))
    {
      // Set the default communication configuration
      if (!::SetCommConfig(m_hFile,&commConfig,dwSize))
      {
        // Display a warning
        errMsg(_CRT_WARN,"CSerial::Open - Unable to set default communication configuration.\n");
      }
    }
    else
    {
      // Display a warning
      errMsg(_CRT_WARN,"CSerial::Open - Unable to obtain default communication configuration.\n");
    }
  #endif
	// Return successful
	return m_lLastError;
}

LONG CSerial::Close (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile == NO_FILE)
	{
		// Display a warning
		errMsg(_CRT_WARN,"CSerial::Close - Method called when device is not open\n");
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED
	// Free event handle
	if (m_hevtOverlapped)
	{
		::CloseHandle(m_hevtOverlapped);
		m_hevtOverlapped = 0;
	}
#endif

	// Close COM port
  #ifdef WIN32
    ::CloseHandle(m_hFile);
  #elif __linux__
    close(m_hFile);
  #endif
	m_hFile = NO_FILE;

	// Return successful
	return m_lLastError;
}

LONG CSerial::Setup (EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::Setup - Device is not opened\n");
		return m_lLastError;
	}

  // Obtain the DCB structure for the device
  CDCB dcb;
	#ifdef WIN32
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::	GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::Setup - Unable to obtain DCB information\n");
      return m_lLastError;
    }

    // Set the new data
    dcb.BaudRate = DWORD(eBaudrate);
    dcb.ByteSize = BYTE(eDataBits);
    dcb.Parity   = BYTE(eParity);
    dcb.StopBits = BYTE(eStopBits);

    // Determine if parity is used
    dcb.fParity  = (eParity != EParNone);

    // Set the new DCB structure
    if (!::SetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::Setup - Unable to set DCB information\n");
      return m_lLastError;
    }
  #elif __linux__
    fcntl(m_hFile, F_SETFL, 0);
    /*
     * Get the current options for the port...
     */
    tcgetattr(m_hFile, &dcb);

    /*
     * Set the baud rates to
     */
    cfsetispeed(&dcb, eBaudrate);
    cfsetospeed(&dcb, eBaudrate);

    /*
     * Enable the receiver and set local mode...
     */
    dcb.c_cflag |= (CLOCAL | CREAD);
//    options.c_cflag &= ~CREAD;

    switch(eDataBits)
    {
      case EData5:
        dcb.c_cflag &= ~CSIZE;
        dcb.c_cflag |= CS5;
        break;

      case EData6:
        dcb.c_cflag &= ~CSIZE;
        dcb.c_cflag |= CS6;
        break;

      case EData7:
        dcb.c_cflag &= ~CSIZE;
        dcb.c_cflag |= CS7;
        break;

      case EData8:
      default:
        dcb.c_cflag &= ~CSIZE;
        dcb.c_cflag |= CS8;
        break;
    }

    switch(eStopBits)
    {
      case EStop1:
      case EStop1_5:
      default:
        dcb.c_cflag &= ~CSTOPB;
        break;

      case EStop2:
        dcb.c_cflag |= CSTOPB;
        break;
    }

    switch(eParity)
    {
      default:
      case EParNone:
        dcb.c_cflag &= ~PARENB;
        break;

      case EParOdd :
        dcb.c_cflag |= PARENB;
        dcb.c_cflag &= ~PARODD;
        break;

      case EParEven:
        dcb.c_cflag |= PARENB | PARODD;
        break;

      case EParMark:
        dcb.c_cflag |= PARENB | CMSPAR | PARODD;
        break;

      case EParSpace:
        dcb.c_cflag |= PARENB | CMSPAR;
        dcb.c_cflag &= ~PARODD;
        break;
    }



//    dcb.c_cflag &= ~CRTSCTS;               /* Disable hardware flow control */



//
////    //RAW input
//    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
////    //RAW output
//    options.c_oflag &= ~OPOST;
////
////    //Software Flow Control
//////    options.c_iflag |= (IXON | IXOFF | IXANY);
//    options.c_iflag &= ~(IXON | IXOFF | IXANY);
//
//    options.c_cc[VINTR]    = 0;     /* Ctrl-c */
//    options.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
//    options.c_cc[VERASE]   = 0;     /* del */
//    options.c_cc[VKILL]    = 0;     /* @ */
//    options.c_cc[VEOF]     = 0;     /* Ctrl-d */
//    options.c_cc[VTIME]    = 0;     /* inter-character timer unused */
//    options.c_cc[VMIN]     = 0;     /* blocking read until 1 character arrives */
//    options.c_cc[VSWTC]    = 0;     /* '\0' */
//    options.c_cc[VSTART]   = 0;     /* Ctrl-q */
//    options.c_cc[VSTOP]    = 0;     /* Ctrl-s */
//    options.c_cc[VSUSP]    = 0;     /* Ctrl-z */
//    options.c_cc[VEOL]     = 0;     /* '\0' */
//    options.c_cc[VREPRINT] = 0;     /* Ctrl-r */
//    options.c_cc[VDISCARD] = 0;     /* Ctrl-u */
//    options.c_cc[VWERASE]  = 0;     /* Ctrl-w */
//    options.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
//    options.c_cc[VEOL2]    = 0;     /* '\0' */
////
                                       /* Get the current options for the port */

                                       /* Enable the receiver and set local mode */
                                     /* Enable data to be processed as raw input */
//    options.c_lflag &= ~(ICANON | ECHO | ISIG);


    /*
     * Set the new options for the port...
     */
    tcsetattr(m_hFile, TCSANOW, &dcb);
  #endif

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetEventChar (BYTE bEventChar, bool fAdjustMask)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::SetEventChar - Device is not opened\n");
		return m_lLastError;
	}
  #ifdef WIN32
    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::SetEventChar - Unable to obtain DCB information\n");
      return m_lLastError;
    }

    // Set the new event character
    dcb.EvtChar = char(bEventChar);

    // Adjust the event mask, to make sure the event will be received
    if (fAdjustMask)
    {
      // Enable 'receive event character' event.  Note that this
      // will generate an EEventNone if there is an asynchronous
      // WaitCommEvent pending.
      SetMask(GetEventMask() | EEventRcvEv);
    }

    // Set the new DCB structure
    if (!::SetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::SetEventChar - Unable to set DCB information\n");
      return m_lLastError;
    }
  #endif
  // Return successful
  return m_lLastError;
}

LONG CSerial::SetMask (DWORD dwEventMask)
{
  // Reset error state
  m_lLastError = ERROR_SUCCESS;

  // Check if the device is open
  if (m_hFile == NO_FILE)
  {
    // Set the internal error code
    m_lLastError = ERROR_INVALID_HANDLE;

    // Issue an error and quit
    errMsg(_CRT_WARN,"CSerial::SetMask - Device is not opened\n");
    return m_lLastError;
  }
  #ifdef WIN32
    // Set the new mask. Note that this will generate an EEventNone
    // if there is an asynchronous WaitCommEvent pending.
    if (!::SetCommMask(m_hFile,dwEventMask))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::SetMask - Unable to set event mask\n");
      return m_lLastError;
    }
  #endif
	// Save event mask and return successful
	m_dwEventMask = dwEventMask;
	return m_lLastError;
}

LONG CSerial::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
  #ifdef WIN32
    // Check if time-outs are supported
    CheckRequirements(lpOverlapped,dwTimeout);
  #endif
    
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::WaitEvent - Device is not opened\n");
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::WaitEvent - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (!lpOverlapped && m_hevtOverlapped)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));

	// Wait for the COM event
	if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			errMsg(_CRT_WARN,"CSerial::WaitEvent - Unable to wait for COM event\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				errMsg(_CRT_WARN,"CSerial::WaitEvent - Unable to wait until COM event has arrived\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}
#else
  #ifdef WIN32
    // Wait for the COM event
    if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),0))
    {
      // Set the internal error code
      m_lLastError = ::GetLastError();

      // Issue an error and quit
      errMsg(_CRT_WARN,"CSerial::WaitEvent - Unable to wait for COM event\n");
      return m_lLastError;
    }
  #endif
#endif

	// Return successfully
	return m_lLastError;
}


LONG CSerial::SetupHandshaking (EHandshake eHandshake)
{
  // Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::SetupHandshaking - Device is not opened\n");
		return m_lLastError;
	}
  CDCB dcb;
  #ifdef WIN32
    // Obtain the DCB structure for the device
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      _RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Unable to obtain DCB information\n");
      return m_lLastError;
    }

    // Set the handshaking flags
    switch (eHandshake)
    {
    case EHandshakeOff:
      dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
      dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
      dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
      dcb.fOutX = false;							// Disable XON/XOFF for transmission
      dcb.fInX = false;							// Disable XON/XOFF for receiving
      dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
      break;

    case EHandshakeHardware:
      dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
      dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
      dcb.fOutX = false;							// Disable XON/XOFF for transmission
      dcb.fInX = false;							// Disable XON/XOFF for receiving
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
      break;

    case EHandshakeSoftware:
      dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
      dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
      dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
      dcb.fOutX = true;							// Enable XON/XOFF for transmission
      dcb.fInX = true;							// Enable XON/XOFF for receiving
      dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
      break;

    default:
      // This shouldn't be possible
      _ASSERTE(false);
      m_lLastError = E_INVALIDARG;
      return m_lLastError;
    }

    // Set the new DCB structure
    if (!::SetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      _RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Unable to set DCB information\n");
      return m_lLastError;
    }
  #elif __linux__
    fcntl(m_hFile, F_SETFL, 0);
    /*
     * Get the current options for the port...
     */
    tcgetattr(m_hFile, &dcb);

    switch (eHandshake)
    {
      case EHandshakeOff:
        dcb.c_cflag &= ~CRTSCTS;          // Disable hardware flow control
        dcb.c_iflag &= ~(IXON | IXOFF | IXANY);   // Disable XON/XOFF
        break;

      case EHandshakeHardware:
        dcb.c_cflag |= CRTSCTS;          // Enable hardware flow control
        dcb.c_iflag &= ~(IXON | IXOFF | IXANY);   // Disable XON/XOFF
        break;

      case EHandshakeSoftware:
        dcb.c_cflag &= ~CRTSCTS;          // Disable hardware flow control
        dcb.c_iflag |= (IXON | IXOFF | IXANY);   // Enable XON/XOFF
        break;

      default:
        // This shouldn't be possible
        _ASSERTE(false);
        m_lLastError = E_INVALIDARG;
        return m_lLastError;
    }

    /*
     * Set the new options for the port...
     */
    tcsetattr(m_hFile, TCSANOW, &dcb);
  #endif
	// Return successful
	return m_lLastError;
}

LONG CSerial::SetupReadTimeouts (EReadTimeout eReadTimeout)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
    errMsg(_CRT_WARN,"CSerial::SetupReadTimeouts - Device is not opened\n");
		return m_lLastError;
	}

  #ifdef WIN32
    // Determine the time-outs
    COMMTIMEOUTS cto;
    if (!::GetCommTimeouts(m_hFile,&cto))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::SetupReadTimeouts - Unable to obtain timeout information\n");
      return m_lLastError;
    }

    // Set the new timeouts
    switch (eReadTimeout)
    {
    case EReadTimeoutBlocking:
      cto.ReadIntervalTimeout = 0;
      cto.ReadTotalTimeoutConstant = 0;
      cto.ReadTotalTimeoutMultiplier = 0;
      break;
    case EReadTimeoutNonblocking:
      cto.ReadIntervalTimeout = MAXDWORD;
      cto.ReadTotalTimeoutConstant = 0;
      cto.ReadTotalTimeoutMultiplier = 0;
      break;
    default:
      // This shouldn't be possible
      _ASSERTE(false);
      m_lLastError = E_INVALIDARG;
      return m_lLastError;
    }

    // Set the new DCB structure
    if (!::SetCommTimeouts(m_hFile,&cto))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::SetupReadTimeouts - Unable to set timeout information\n");
      return m_lLastError;
    }
  #endif
	// Return successful
	return m_lLastError;
}

CSerial::EBaudrate CSerial::GetBaudrate (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetBaudrate - Device is not opened\n");
		return EBaudUnknown;
	}

  #ifdef WIN32
    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetBaudrate - Unable to obtain DCB information\n");
      return EBaudUnknown;
    }
	// Return the appropriate baudrate
	return EBaudrate(dcb.BaudRate);
  #endif
  return EBaudUnknown;
}

CSerial::EDataBits CSerial::GetDataBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetDataBits - Device is not opened\n");
		return EDataUnknown;
	}

  #ifdef WIN32
    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetDataBits - Unable to obtain DCB information\n");
      return EDataUnknown;
    }
	// Return the appropriate bytesize
	return EDataBits(dcb.ByteSize);
  #endif
  return EDataUnknown;
}

CSerial::EParity CSerial::GetParity (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetParity - Device is not opened\n");
		return EParUnknown;
	}

  #ifdef WIN32
    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetParity - Unable to obtain DCB information\n");
      return EParUnknown;
    }

    // Check if parity is used
    if (!dcb.fParity)
    {
      // No parity
      return EParNone;
    }

    // Return the appropriate parity setting
    return EParity(dcb.Parity);
  #endif
    return EParUnknown;
}

CSerial::EStopBits CSerial::GetStopBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetStopBits - Device is not opened\n");
		return EStopUnknown;
	}
  #ifdef WIN32
    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetStopBits - Unable to obtain DCB information\n");
      return EStopUnknown;
    }
    // Return the appropriate stopbits
    return EStopBits(dcb.StopBits);
  #endif
  return EStopUnknown;
}

DWORD CSerial::GetEventMask (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetEventMask - Device is not opened\n");
		return 0;
	}

	// Return the event mask
	return m_dwEventMask;
}

BYTE CSerial::GetEventChar (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetEventChar - Device is not opened\n");
		return 0;
	}
  #ifdef WIN32
    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetEventChar - Unable to obtain DCB information\n");
      return 0;
    }

    // Set the new event character
    return BYTE(dcb.EvtChar);
  #endif
  return EEventUnknown;
}

CSerial::EHandshake CSerial::GetHandshaking (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetHandshaking - Device is not opened\n");
		return EHandshakeUnknown;
	}

  #ifdef WIN32
    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetHandshaking - Unable to obtain DCB information\n");
      return EHandshakeUnknown;
    }

    // Check if hardware handshaking is being used
    if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
      return EHandshakeHardware;

    // Check if software handshaking is being used
    if (dcb.fOutX && dcb.fInX)
      return EHandshakeSoftware;
  #endif
	// No handshaking is being used
	return EHandshakeOff;
}

LONG CSerial::Write (const void* pData, size_t iLen, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
  #ifdef WIN32
    // Check if time-outs are supported
    CheckRequirements(lpOverlapped,dwTimeout);
  #endif

	// Overlapped operation should specify the pdwWritten variable
	_ASSERTE(!lpOverlapped || pdwWritten);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwWritten;
	if (pdwWritten == 0)
	{
		pdwWritten = &dwWritten;
	}

	// Reset the number of bytes written
	*pdwWritten = 0;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::Write - Device is not opened\n");
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::Write - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (!lpOverlapped && m_hevtOverlapped)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));

	// Write the data
	if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			errMsg(_CRT_WARN,"CSerial::Write - Unable to write the data\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					errMsg(_CRT_WARN,"CSerial::Write - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				errMsg(_CRT_WARN,"CSerial::Write - Unable to wait until data has been sent\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}

#else
  #ifdef WIN32
    // Write the data
    if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,0))
    {
      // Set the internal error code
      m_lLastError = ::GetLastError();

      // Issue an error and quit
      errMsg(_CRT_WARN,"CSerial::Write - Unable to write the data\n");
      return m_lLastError;
    }
  #elif __linux__
    *pdwWritten = write(m_hFile, pData, iLen);
//    std::cout << *pdwWritten << " Bytes written" << std::endl;
  #endif
#endif

	// Return successfully
	return m_lLastError;
}

LONG CSerial::Write (LPCSTR pString, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
  #ifdef WIN32
    // Check if time-outs are supported
    CheckRequirements(lpOverlapped,dwTimeout);
  #endif
	// Determine the length of the string
	return Write(pString,strlen(pString),pdwWritten,lpOverlapped,dwTimeout);
}

LONG CSerial::Read (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
  #ifdef WIN32
    // Check if time-outs are supported
    CheckRequirements(lpOverlapped,dwTimeout);
  #endif

	// Overlapped operation should specify the pdwRead variable
	_ASSERTE(!lpOverlapped || pdwRead);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwRead;
	if (pdwRead == 0)
	{
		pdwRead = &dwRead;
	}

	// Reset the number of bytes read
	*pdwRead = 0;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::Read - Device is not opened\n");
		return m_lLastError;
	}

#ifdef _DEBUG
	// The debug version fills the entire data structure with
	// 0xDC bytes, to catch buffer errors as soon as possible.
	memset(pData,0xDC,iLen);
#endif

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::Read - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));

	// Read the data
	if (!::ReadFile(m_hFile,pData,iLen,pdwRead,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			errMsg(_CRT_WARN,"CSerial::Read - Unable to read the data\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					errMsg(_CRT_WARN,"CSerial::Read - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				errMsg(_CRT_WARN,"CSerial::Read - Unable to wait until data has been read\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}

#else
  #ifdef WIN32
    // Read the data
    if (!::ReadFile(m_hFile,pData,iLen,pdwRead,0))
    {
      // Set the internal error code
      m_lLastError = ::GetLastError();

      // Issue an error and quit
      errMsg(_CRT_WARN,"CSerial::Read - Unable to read the data\n");
      return m_lLastError;
    }
  #elif __linux__
  size_t bytes = 0;
  fcntl(m_hFile, F_SETFL, FNDELAY);
  bytes = read(m_hFile, pData, iLen);
  if (bytes > 0 && bytes < iLen)
  {
    *pdwRead = bytes;
//    std::cout << *pdwRead << " Bytes read" << std::endl;
  }
  fcntl(m_hFile, F_SETFL, 0);
  #endif
#endif

	// Return successfully
	return m_lLastError;
}

LONG CSerial::Purge()
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::Purge - Device is not opened\n");
		return m_lLastError;
	}

  #ifdef WIN32
    if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR))
    {
      // Set the internal error code
      m_lLastError = ::GetLastError();
      errMsg(_CRT_WARN,"CSerial::Purge - Overlapped completed without result\n");
    }
  #elif __linux__
    CDCB dcb;
    /*
     * Get the current options for the port...
     */
    tcgetattr(m_hFile, &dcb);
    tcsetattr(m_hFile, TCSAFLUSH, &dcb);
  #endif
	// Return successfully
	return m_lLastError;
}

LONG CSerial::Break (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::Break - Device is not opened\n");
		return m_lLastError;
	}
  #ifdef WIN32
    // Set the RS-232 port in break mode for a little while
    ::SetCommBreak(m_hFile);
    ::Sleep(100);
    ::ClearCommBreak(m_hFile);
  #endif
	// Return successfully
	return m_lLastError;
}

CSerial::EEvent CSerial::GetEventType (void)
{
#ifdef _DEBUG
	// Check if the event is within the mask
	if ((m_eEvent & m_dwEventMask) == 0)
		_RPTF2(_CRT_WARN,"CSerial::GetEventType - Event %08Xh not within mask %08Xh.\n", m_eEvent, m_dwEventMask);
#endif

	// Obtain the event (mask unwanted events out)
	EEvent eEvent = EEvent(m_eEvent & m_dwEventMask);

	// Reset internal event type
	m_eEvent = EEventNone;

	// Return the current cause
	return eEvent;
}

CSerial::EError CSerial::GetError (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == NO_FILE)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		errMsg(_CRT_WARN,"CSerial::GetError - Device is not opened\n");
		return EErrorUnknown;
	}

	// Obtain COM status
  #ifdef WIN32
    DWORD dwErrors = 0;
    if (!::ClearCommError(m_hFile,&dwErrors,0))
    {
      // Set the internal error code
      m_lLastError = ::GetLastError();

      // Issue an error and quit
      errMsg(_CRT_WARN,"CSerial::GetError - Unable to obtain COM status\n");
      return EErrorUnknown;
    }

    // Return the error
    return EError(dwErrors);
  #endif
  return EErrorUnknown;
}

bool CSerial::GetCTS (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
  #ifdef WIN32
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetCTS - Unable to obtain the modem status\n");
      return false;
    }

    // Determine if CTS is on
    return (dwModemStat & MS_CTS_ON) != 0;
  #endif
  return false;
}

bool CSerial::GetDSR (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
  #ifdef WIN32
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetDSR - Unable to obtain the modem status\n");
      return false;
    }

    // Determine if DSR is on
    return (dwModemStat & MS_DSR_ON) != 0;
  #endif
  return false;
}

bool CSerial::GetRing (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
  #ifdef WIN32
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetRing - Unable to obtain the modem status");
      return false;
    }

    // Determine if Ring is on
    return (dwModemStat & MS_RING_ON) != 0;
  #endif
    return false;
}

bool CSerial::GetRLSD (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
  #ifdef WIN32
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
      // Obtain the error code
      m_lLastError = ::GetLastError();

      // Display a warning
      errMsg(_CRT_WARN,"CSerial::GetRLSD - Unable to obtain the modem status");
      return false;
    }

    // Determine if RLSD is on
    return (dwModemStat & MS_RLSD_ON) != 0;
  #endif
  return false;
}


#endif // SERIAL_IS_DEFINED