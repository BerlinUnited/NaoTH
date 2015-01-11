/* 
 * File:   DefinitionsLinux.h
 * Author: claas
 *
 * Created on 15. Juli 2010, 10:17
 */
#ifdef __linux__
  #ifndef _DEFINITIONSLINUX_H
  #define  _DEFINITIONSLINUX_H

  // this marker indicates that the serial definitions 
  // are avaliable on this platform
  #define SERIAL_IS_DEFINED

  #define SERIAL_NO_OVERLAPPED

  #include <stdio.h>   /* Standard input/output definitions */
  #include <string.h>  /* String function definitions */
  #include <unistd.h>  /* UNIX standard function definitions */
  #include <fcntl.h>   /* File control definitions */
  #include <errno.h>   /* Error number definitions */
  #include <termios.h> /* POSIX terminal control definitions */

  #include <limits>
  #include <iostream>
  #include <assert.h>

  typedef int HANDLE;
  typedef long LONG;
  typedef bool BOOL;
  typedef unsigned long DWORD;
  typedef unsigned short WORD;
  typedef unsigned char BYTE;

  #define SERIAL_DEFAULT_OVERLAPPED false

  //typedef char* LPCTSTR;
  typedef std::string LPCTSTR;
  typedef char* LPCSTR;
  typedef bool LPOVERLAPPED;


  #define NO_FILE -1

  #define INFINITE -1
  #define ERROR_SUCCESS -1
  #define ERROR_ALREADY_INITIALIZED 1
  #define ERROR_INVALID_HANDLE 2
  #define E_INVALIDARG 3
  #define INVALID_HANDLE_VALUE -1
  #define ERROR_FILE_NOT_FOUND -1
  #define ERROR_ACCESS_DENIED -1
  #define _CRT_WARN 0

  #define _ASSERTE assert

  // Communication event
  typedef enum
  {
    EEventUnknown       = -1,      // Unknown event
    EEventNone       = 0,        // Event trigged without cause
    EEventBreak      = 1,//EV_BREAK,    // A break was detected on input
    EEventCTS        = 2,//EV_CTS,    // The CTS signal changed state
    EEventDSR        = 3,//EV_DSR,    // The DSR signal changed state
    EEventError      = 4,//EV_ERR,    // A line-status error occurred
    EEventRing       = 5,//EV_RING,    // A ring indicator was detected
    EEventRLSD       = 6,//EV_RLSD,    // The RLSD signal changed state
    EEventRecv       = 7,//EV_RXCHAR,    // Data is received on input
    EEventRcvEv      = 8,//EV_RXFLAG,    // Event character was received on input
    EEventSend       = 9,//EV_TXEMPTY,  // Last character on output was sent
    EEventPrinterError = 10,//EV_PERR,    // Printer error occured
    EEventRx80Full     = 11,//EV_RX80FULL,  // Receive buffer is 80 percent full
    EEventProviderEvt1 = 12,//EV_EVENT1,    // Provider specific event 1
    EEventProviderEvt2 = 13,//EV_EVENT2,    // Provider specific event 2
  }
  CSerialEvent;

  // Baudrate
  typedef enum
  {
    EBaudUnknown = -1,      // Unknown
    EBaud110     = B110,    // 110 bits/sec
    EBaud300     = B300,    // 300 bits/sec
    EBaud600     = B600,    // 600 bits/sec
    EBaud1200    = B1200,  // 1200 bits/sec
    EBaud2400    = B2400,  // 2400 bits/sec
    EBaud4800    = B4800,  // 4800 bits/sec
    EBaud9600    = B9600,  // 9600 bits/sec
    EBaud14400   = 14400,  // 14400 bits/sec
    EBaud19200   = B19200,  // 19200 bits/sec (default)
    EBaud38400   = B38400,  // 38400 bits/sec
    EBaud56000   = 56000,  // 56000 bits/sec
    EBaud57600   = B57600,  // 57600 bits/sec
    EBaud115200  = B115200,  // 115200 bits/sec
    EBaud128000  = 128000,  // 128000 bits/sec
    EBaud230400  = B230400,  // 230400 bits/sec
    EBaud256000  = 256000,  // 256000 bits/sec
    EBaud460800  = B460800,  // 460800 bits/sec
    EBaud500000  = B500000,  // 500000 bits/sec
    EBaud576000  = B576000,  // 576000 bits/sec
    EBaud921600  = B921600,  // 921600 bits/sec
    EBaud1000000 = B1000000, // 1000000 bits/sec
    EBaud1152000 = B1152000, // 1000000 bits/sec
    EBaud2000000 = B2000000, // 1000000 bits/sec
    EBaud2500000 = B2500000, // 1000000 bits/sec
    EBaud3000000 = B3000000, // 1000000 bits/sec
    EBaud3500000 = B3500000, // 1000000 bits/sec
    EBaud4000000 = B4000000, // 1000000 bits/sec
  }
  CSerialBaudrate;

  // Data bits (5-8)
  typedef enum
  {
    EDataUnknown = -1,  // Unknown
    EData5       =  5,  // 5 bits per byte
    EData6       =  6,  // 6 bits per byte
    EData7       =  7,  // 7 bits per byte
    EData8       =  8    // 8 bits per byte (default)
  }
  CSerialDataBits;

  // Parity scheme
  typedef enum
  {
    EParUnknown = -1,  // Unknown
    EParNone    = 0,  // No parity (default)
    EParOdd     = 1,  // Odd parity
    EParEven    = 2,  // Even parity
    EParMark    = 3,  // Mark parity
    EParSpace   = 4   // Space parity
  }
  CSerialParity;

  // Stop bits
  typedef enum
  {
    EStopUnknown = -1,  // Unknown
    EStop1       = 0,   // 1 stopbit (default)
    EStop1_5     = 1,   // 1.5 stopbit
    EStop2       = 2    // 2 stopbits
  }
  CSerialStopBits;

  // Handshaking
  typedef enum
  {
    EHandshakeUnknown   = -1,  // Unknown
    EHandshakeOff       =  0,  // No handshaking
    EHandshakeHardware  =  1,  // Hardware handshaking (RTS/CTS)
    EHandshakeSoftware  =  2  // Software handshaking (XON/XOFF)
  }
  CSerialHandshake;

  // Timeout settings
  typedef enum
  {
    EReadTimeoutUnknown     = -1,  // Unknown
    EReadTimeoutNonblocking  =  0,  // Always return immediately
    EReadTimeoutBlocking    =  1  // Block until everything is retrieved
  }
  CSerialReadTimeout;

  // Communication errors
  typedef enum
  {
    EErrorUnknown = 0,            // Unknown
    EErrorBreak   = 1,//CE_BREAK,  // Break condition detected
    EErrorFrame   = 2,//CE_FRAME,  // Framing error
    EErrorIOE     = 3,//CE_IOE,    // I/O device error
    EErrorMode    = 4,//CE_MODE,  // Unsupported mode
    EErrorOverrun = 5,//CE_OVERRUN,  // Character buffer overrun, next byte is lost
    EErrorRxOver  = 6,//CE_RXOVER,  // Input buffer overflow, byte lost
    EErrorParity  = 7,//CE_RXPARITY,// Input parity error
    EErrorTxFull  = 8,//CE_TXFULL  // Output buffer full
  }
  CSerialError;

  // Port availability
  typedef enum
  {
    EPortUnknownError = -1,    // Unknown error occurred
    EPortAvailable    =  0,    // Port is available
    EPortNotAvailable =  1,    // Port is not present
    EPortInUse        =  2    // Port is in use
  }
  CSerialPort;


  #endif  /* _DEFINITIONSLINUX_H */
#endif
