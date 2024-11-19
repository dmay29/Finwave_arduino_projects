/*
Contains the function to parse SCPI and call the appropriate functions

*/

#include "scpi.h"

#include "commands.h"

void initSCPI(){
  
};

// Parse SCPI command
void parseSCPI(String& command) {
  command.toLowerCase();  // Convert to lowercase for case-insensitive matching

  // Step through every SCPI command, this is fine for simple instruments with few commands
  for (const struct SCPICommandMapping& scpi : SCPI_commands) {
    
    String pattern = scpi.pattern;
    pattern.toLowerCase();  // Convert command pattern to lowercase

    unsigned int i_p = 0;  // index for SCPI command
    unsigned int i_c = 0;  // index for incoming command
    bool match = true;
    String args[SCPI_MAX_ARGS];  // Array to hold extracted arguments
    
    int argIndex = 0;            // Index for args array

    // Step through the pattern and command character by character
    while (i_p < pattern.length() && i_c < command.length()) {
      
      if (pattern.charAt(i_p) == command.charAt(i_c)) { // If the same, advance both cursors
        i_p++;
        i_c++;
      } else if (pattern.charAt(i_p) == ' ') { // Ignore spaces
        i_p++;
      } else if (pattern.charAt(i_c) == ' ') { // Ignore spaces
        i_c++;
      } else if (pattern.charAt(i_p) == '<') { // Indicates a variable, need to save this part of the command

        // Find the end of the placeholder
        i_p = pattern.indexOf('>', i_p) + 1; // Find closing '>' and move past it

        // i_p now points to the character after the variable
        // Find the corresponding character in the command so we can tell when the variable is over
        // Not a perfect system but works well for numbers
        int start_i_c;
        if (i_p < pattern.length()){
          char nextChar = pattern.charAt(i_p);
          i_c = command.indexOf(nextChar,i_c);
        } else {
          i_c = command.length();
        }
        
        // Extract and store the placeholder value from command
        args[argIndex++] = command.substring(start_i_c, i_c).trim();
      } else {

        match = false;
        break;
      }
    }
    if (i_c != command.length() || i_p != pattern.length()){
      match = false;
    }

    // if it matched call the handler and exit
    if (match) {
      // Call the associated function with the extracted arguments
      // Serial.print(pattern);
      // for (int i = 0; i < argIndex; i++) {
      //   Serial.print(args[i]);
      //   Serial.print(',');
      // }
      scpi.handler(args);
      return;  // Command matched and executed, exit the function
    }
  }

  ErrorQueue.enqueue("Unknown SCPI command.");
}
