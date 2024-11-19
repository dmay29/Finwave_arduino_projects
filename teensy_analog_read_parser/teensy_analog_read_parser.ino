#include <Adafruit_MCP23X17.h>

#define IDN_RESPONSE "Finwave, 14-Channel Power Meter, 1, 1.0"

#include "scpi.h"
const int pin_map[] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 9, 10, 11, 12};
const int numChannels = sizeof(pin_map) / sizeof(pin_map[0]);

float getPower(int channel){
  return averagedAnalogRead(channel, 1000);
}

float averagedAnalogRead(int channel, uint16_t samples){
  float sum = 0;
  for(int i = 0; i < samples; i++){
    sum += analogRead(pin_map[channel-1]);
  }
  return sum/samples;
}

void setup() {
  Serial.begin(115200);
  for(int channel = 0; channel < numChannels; channel++){
    pinMode(pin_map[channel], INPUT);
  }

}

void loop() {
  // Check if data is available on Serial
  if (Serial.available() > 0) {
    // digitalWrite(BUILTIN_LED)
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove any trailing whitespace
    parseSCPI(input);
  }
}
