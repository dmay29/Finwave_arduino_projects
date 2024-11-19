#ifndef SCPI_H
#define SCPI_H

#include <Queue.h>

#ifndef SCPI_MAX_ARGS
  #define SCPI_MAX_ARGS 5
#endif

#ifndef IDN_RESPONSE
  #define IDN_RESPONSE "Finwave, Teensy, 1, 0"
#endif

// Structure definition for SCPI Command Mapping
struct SCPICommandMapping {
  String pattern;                // Command pattern
  void (*handler)(String*);      // Pointer to the function associated with the command
};

//Declare Error Queue
Queue<String, 10> ErrorQueue;

// Parse SCPI command
void init_SCPI();
void parseSCPI(const String& command);
int chanListToArray(const String chanList, int* channels);
#endif


