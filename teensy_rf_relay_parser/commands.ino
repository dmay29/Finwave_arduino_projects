#include "commands.h"
#include "scpi.h"

// Define the actual default list of commands
const struct SCPICommandMapping SCPI_commands[] = {
  /* PATTERN, HANDLER */
  /* Keep these three */
    { "*IDN?", identify}, 
    { "*RST", reset},
    { "SYSTem:ERRor?", error_query},
  /* */
    { "SWITCH<n>, <state>", scpi_setSwitch},
    { "SWITCH<n>?", scpi_getSwitch},
};

const int numCommands = sizeof(SCPI_commands) / sizeof(SCPI_commands[0]);

// Default SCPI Command Handlers
void identify(String* args){
  Serial.println(IDN_RESPONSE);
};

void reset(String* args){
  resetSwitches();

  while(!ErrorQueue.isEmpty()){
    ErrorQueue.dequeue();
  }
};

void error_query(String* args){
  if (ErrorQueue.isEmpty()){
    Serial.println("No Errors");
  } else {
    Serial.println("error");//ErrorQueue.front());
    ErrorQueue.dequeue();
  }
};

// Implementation Specific Commands Handlers
void scpi_setSwitch(String* args){
  int switch_num = args[0].toInt();
  int throw_num = args[1].toInt();
  setSwitch(switch_num, throw_num);
};

void scpi_getSwitch(String* args){
  int switch_num = args[0].toInt();
  int throw_num = getSwitch(switch_num);
  Serial.println(throw_num);
};
