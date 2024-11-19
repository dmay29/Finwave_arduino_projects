TEENSY RF RELAY w/ SCPI PARSER

Files
- scpi.h/.ino - Contains parsing functions for scpi commands. Shouldn't need to be edited
- commands.h/.ino - Contains a list of commands that can be sent to the device

Comand Patterns
- Generally follows the SCPI protocol
- Case insensitive, spaces ignored
- '< >' used to show where variables should go
- Ex.  
    pattern = "CH<channel>:VOLT <voltage>"
    command = "CH1:VOLT 5"

Command patterns are linked to handler functions in the commandMapping struct in commands.ino
Handler fucntions recieve an array of strings in the order they appear in the command
Ex.  
    pattern = "CH<channel>:VOLT <voltage>"
    command = "CH1:VOLT 5"
    The handler would recieve {"1","5"}

For queries, the handler should send the results back over serial.
