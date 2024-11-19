#include "commands.h"
#include "scpi.h"

#include <math.h>

// Define the actual default list of commands
const struct SCPICommandMapping SCPI_commands[] = {
  { "*IDN?", identify },
  { "*RST", reset },
  { "SYSTem:ERRor?", error_query },
  { "MEAS:POWER? <channels>", scpi_getPower },
};

const int numCommands = sizeof(SCPI_commands) / sizeof(SCPI_commands[0]);

// Default SCPI Command Handlers
void identify(String* args) {
  Serial.println(IDN_RESPONSE);
};

void reset(String* args) {
  while (!ErrorQueue.isEmpty()) {
    ErrorQueue.dequeue();
  }
};

void error_query(String* args) {
  if (ErrorQueue.isEmpty()) {
    Serial.println("No Errors");
  } else {
    Serial.println("error");  //ErrorQueue.front());
    ErrorQueue.dequeue();
  }
};

// Implementation Specific Commands Handlers
void scpi_getPower(String* args) {
  String channels = args[0];
  String output = "";
  Serial.println(channels);
  int chanList[numChannels];
  int chanListLength = chanListToArray(channels, chanList);
  for (int i = 0; i < chanListLength; i++) {
    int channel = chanList[i];
    String power = (channel != 0) ? String(getPower(channel),6) : "NaN";
    output += power + ", ";
  }

  output.trim().remove(-1);
  Serial.println(output);
};

void scpi_getRaw(String* args) {
  String channels = args[0];
  String output = "";
  Serial.println(channels);
  int chanList[numChannels];
  int chanListLength = chanListToArray(channels, chanList);
  for (int i = 0; i < chanListLength; i++) {
    int channel = chanList[i];
    String power = (channel != 0) ? String(averagedAnalogRead(channel),6) : "NaN";
    output += power + ", ";
  }

  output.trim().remove(-1);
  Serial.println(output);
};
