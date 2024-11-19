#ifndef SCPI_COMMANDS_H
#define SCPI_COMMANDS_H

// SCPI Commands List
extern const struct SCPICommandMapping SCPI_commands[];

// Default SCPI Command Handlers
void identify(String* args);
void reset(String* args);
void error_query(String* args);

// Implementation Specific Commands Handlers 
void scpi_setSwitch(String* args);
void scpi_getSwitch(String* args);

#endif

