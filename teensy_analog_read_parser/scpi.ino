#include "scpi.h"

#include "commands.h"

void initSCPI(){

};

// Parse SCPI command
void parseSCPI(String& command) {
  command.toLowerCase();  // Convert to lowercase for case-insensitive matching

  for (const struct SCPICommandMapping& scpi : SCPI_commands) {
    String pattern = scpi.pattern;
    pattern.toLowerCase();  // Convert command pattern to lowercase

    unsigned int i_p = 0;  // index for SCPI command
    unsigned int i_c = 0;  // index for incoming command
    bool match = true;
    String args[SCPI_MAX_ARGS];  // Array to hold extracted arguments

    int argIndex = 0;  // Index for args array

    while (i_p < pattern.length() && i_c < command.length()) {
      if (pattern.charAt(i_p) == command.charAt(i_c)) {
        i_p++;
        i_c++;
      } else if (pattern.charAt(i_p) == ' ') {
        i_p++;
      } else if (pattern.charAt(i_c) == ' ') {
        i_c++;
      } else if (pattern.charAt(i_p) == '<') {

        // Find the end of the placeholder
        i_p = pattern.indexOf('>', i_p) + 1;  // Find closing '>' and move past it

        // Find the next fixed character in pattern
        int start_i_c = i_c;
        if (i_p < pattern.length()) {
          char nextChar = pattern.charAt(i_p);
          i_c = command.indexOf(nextChar, i_c);
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
    if (i_c != command.length() || i_p != pattern.length()) {
      match = false;
    }

    // Check if the command matched and the correct number of arguments were provided
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


int chanListToArray(const String& chanList, int channels[], ) {
  String tempChanList = chanList;
  int index = 0;  // Index for the output array
  String number;   // Temporary string for each number
  bool has_comma = (tempChanList.indexOf(',') != -1);
  bool has_colon = (tempChanList.indexOf(':') != -1);

  if (has_comma && has_colon) { // Can't have both
    ErrorQueue.enqueue("Bad Channel List");
    return 0;
  } else if (!has_comma && !has_colon) {  // No valid delimiter found, return the one value
    channels[0] = tempChanList.toInt();
    return 1;
  }


  if (has_colon) {

    // Find the colon
    int colonIndex = tempChanList.indexOf(':');
    if (colonIndex != tempChanList.lastIndexOf(':')) { // Only one colon allowed
      ErrorQueue.enqueue("Bad Channel List");
      return 0;
    }

    // Extract values
    number = tempChanList.substring(0, colonIndex);      // Extract number
    tempChanList = tempChanList.substring(colonIndex + 1);  // Remove the number from the tempChanList
    int start = number.toInt();                      // Convert to start integer
    int end = tempChanList.toInt();                  // Get the remaining part as end integer

    // Validate start and end
    if (start > end || end > numChannels) {
      ErrorQueue.enqueue("Bad Channel List");
      return 0;
    }

    // Fill the output array
    for (int i = start; i <= end && i <= numChannels; i++) {
      channels[index++] = i;  
    }

  } else { // If comma seperated
    // Loop through the tempChanList string
    while (tempChanList.length() > 0 && index < numChannels) {
      // Find the position of the comman
      int commaIndex = tempChanList.indexOf(',');

      if (commaIndex == -1) {
        number = tempChanList;  // Take the rest of the string
      } else {
        number = tempChanList.substring(0, commaIndex);      // Extract number
        tempChanList = tempChanList.substring(commaIndex + 1);  // Remove the number from the tempChanList
      }

      // Convert the number to an integer and store it in the array
      int channel = number.toInt();
      if (channel < 0 || channel > numChannels) { // If the channel is bad ignore it
        channel = 0;
      }
      channels[index++] = channel;
    }
  }
  return index;
}