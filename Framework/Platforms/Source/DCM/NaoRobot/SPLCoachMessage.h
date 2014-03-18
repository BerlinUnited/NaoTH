#ifndef SPLCOACHMESSAGE_H
#define SPLCOACHMESSAGE_H

#include <stdint.h>

#define SPL_COACH_MESSAGE_PORT           3839

#define SPL_COACH_MESSAGE_STRUCT_HEADER  "SPLC"
#define SPL_COACH_MESSAGE_STRUCT_VERSION 2
#define SPL_COACH_MESSAGE_SIZE           40

#ifdef __cplusplus
  #include <string>
#endif

struct SPLCoachMessage 
{
  char header[4];        // "SPLC"
  uint8_t version;       // SPL_COACH_MESSAGE_STRUCT_VERSION
  uint8_t team;          // team number

  // buffer for message
  uint8_t message[SPL_COACH_MESSAGE_SIZE];

#ifdef __cplusplus
  // constructor
  SPLCoachMessage()
  {
    std::string headerAsString = SPL_COACH_MESSAGE_STRUCT_HEADER;
    headerAsString.copy(header, 4, 0);
    version = SPL_COACH_MESSAGE_STRUCT_VERSION;
  }
#endif
};

#endif // SPLCOACHMESSAGE_H
