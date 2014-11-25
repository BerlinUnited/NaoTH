/* 
 * File:   DefinitionsWindows.h
 * Author: claas
 *
 * Created on 15. Juli 2010, 10:16
 */
#ifdef WIN32
  #ifndef _DEFINITIONSWINDOWS_H
  #define  _DEFINITIONSWINDOWS_H

  // this marker indicates that the serial definitions 
  // are avaliable on this platform
  #define SERIAL_IS_DEFINED

  #define SERIAL_NO_CANCELIO
  #define SERIAL_NO_OVERLAPPED

  // Windows Header Files:
  #include <windows.h>
  #include <stdio.h>
  #include <math.h>
  #include <malloc.h>
  #include <crtdbg.h>

  //////////////////////////////////////////////////////////////////////
  // The SERIAL_DEFAULT_OVERLAPPED defines if the default open mode uses
  // overlapped I/O. When overlapped I/O is available (normal Win32
  // platforms) it uses overlapped I/O. Windows CE doesn't allow the use
  // of overlapped I/O, so it is disabled there by default.

  #ifndef SERIAL_DEFAULT_OVERLAPPED
  #ifndef SERIAL_NO_OVERLAPPED
  #define SERIAL_DEFAULT_OVERLAPPED  true
  #else
  #define SERIAL_DEFAULT_OVERLAPPED  false
  #endif
  #endif

  #define NO_FILE 0

  // Communication event
  typedef enum
  {
    EEventUnknown       = -1,      // Unknown event
    EEventNone       = 0,        // Event trigged without cause
    EEventBreak      = EV_BREAK,    // A break was detected on input
    EEventCTS        = EV_CTS,    // The CTS signal changed state
    EEventDSR        = EV_DSR,    // The DSR signal changed state
    EEventError      = EV_ERR,    // A line-status error occurred
    EEventRing       = EV_RING,    // A ring indicator was detected
    EEventRLSD       = EV_RLSD,    // The RLSD signal changed state
    EEventRecv       = EV_RXCHAR,    // Data is received on input
    EEventRcvEv      = EV_RXFLAG,    // Event character was received on input
    EEventSend       = EV_TXEMPTY,  // Last character on output was sent
    EEventPrinterError = EV_PERR,    // Printer error occured
    EEventRx80Full     = EV_RX80FULL,  // Receive buffer is 80 percent full
    EEventProviderEvt1 = EV_EVENT1,    // Provider specific event 1
    EEventProviderEvt2 = EV_EVENT2,    // Provider specific event 2
  }
  CSerialEvent;

  // Baudrate
  typedef enum
  {
    EBaudUnknown = -1,        // Unknown
    EBaud110     = CBR_110,    // 110 bits/sec
    EBaud300     = CBR_300,    // 300 bits/sec
    EBaud600     = CBR_600,    // 600 bits/sec
    EBaud1200    = CBR_1200,  // 1200 bits/sec
    EBaud2400    = CBR_2400,  // 2400 bits/sec
    EBaud4800    = CBR_4800,  // 4800 bits/sec
    EBaud9600    = CBR_9600,  // 9600 bits/sec
    EBaud14400   = CBR_14400,  // 14400 bits/sec
    EBaud19200   = CBR_19200,  // 19200 bits/sec (default)
    EBaud38400   = CBR_38400,  // 38400 bits/sec
    EBaud56000   = CBR_56000,  // 56000 bits/sec
    EBaud57600   = CBR_57600,  // 57600 bits/sec
    EBaud115200  = CBR_115200,// 115200 bits/sec
    EBaud128000  = CBR_128000,// 128000 bits/sec
    EBaud230400  = 230400,    // 230400 bits/sec
    EBaud256000  = CBR_256000,// 256000 bits/sec
    EBaud460800  = 460800,  // 460800 bits/sec
    EBaud500000  = 500000,  // 500000 bits/sec
    EBaud576000  = 576000,  // 576000 bits/sec
    EBaud921600  = 921600,  // 921600 bits/sec
    EBaud1000000 = 1000000,   // 1000000 bits/sec
    EBaud1152000 = 1152000,   // 1152000 bits/sec
    EBaud2000000 = 2000000,   // 2000000 bits/sec
    EBaud2500000 = 2500000,   // 2500000 bits/sec
    EBaud3000000 = 3000000,   // 3000000 bits/sec
    EBaud3500000 = 3500000,   // 3500000 bits/sec
    EBaud4000000 = 4000000,   // 4000000 bits/sec
  }
  CSerialBaudrate;

  // Data bits (5-8)
  typedef enum
  {
    EDataUnknown = -1,      // Unknown
    EData5       =  5,      // 5 bits per byte
    EData6       =  6,      // 6 bits per byte
    EData7       =  7,      // 7 bits per byte
    EData8       =  8      // 8 bits per byte (default)
  }
  CSerialDataBits;

  // Parity scheme
  typedef enum
  {
    EParUnknown = -1,      // Unknown
    EParNone    = NOPARITY,    // No parity (default)
    EParOdd     = ODDPARITY,  // Odd parity
    EParEven    = EVENPARITY,  // Even parity
    EParMark    = MARKPARITY,  // Mark parity
    EParSpace   = SPACEPARITY  // Space parity
  }
  CSerialParity;

  // Stop bits
  typedef enum
  {
    EStopUnknown = -1,      // Unknown
    EStop1       = ONESTOPBIT,  // 1 stopbit (default)
    EStop1_5     = ONE5STOPBITS,// 1.5 stopbit
    EStop2       = TWOSTOPBITS  // 2 stopbits
  }
  CSerialStopBits;

  // Handshaking
  typedef enum
  {
    EHandshakeUnknown    = -1,  // Unknown
    EHandshakeOff      =  0,  // No handshaking
    EHandshakeHardware    =  1,  // Hardware handshaking (RTS/CTS)
    EHandshakeSoftware    =  2  // Software handshaking (XON/XOFF)
  }
  CSerialHandshake;

  // Timeout settings
  typedef enum
  {
    EReadTimeoutUnknown    = -1,  // Unknown
    EReadTimeoutNonblocking  =  0,  // Always return immediately
    EReadTimeoutBlocking  =  1  // Block until everything is retrieved
  }
  CSerialReadTimeout;

  // Communication errors
  typedef enum
  {
    EErrorUnknown = 0,      // Unknown
    EErrorBreak   = CE_BREAK,  // Break condition detected
    EErrorFrame   = CE_FRAME,  // Framing error
    EErrorIOE     = CE_IOE,    // I/O device error
    EErrorMode    = CE_MODE,  // Unsupported mode
    EErrorOverrun = CE_OVERRUN,  // Character buffer overrun, next byte is lost
    EErrorRxOver  = CE_RXOVER,  // Input buffer overflow, byte lost
    EErrorParity  = CE_RXPARITY,// Input parity error
    EErrorTxFull  = CE_TXFULL  // Output buffer full
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



  #endif  /* _DEFINITIONSWINDOWS_H */
#endif
