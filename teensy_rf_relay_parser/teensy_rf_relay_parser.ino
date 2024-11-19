#include <Adafruit_MCP23X17.h>

#define IDN_RESPONSE "Finwave, RF Relay Controller, 1, 1.0"

#include "scpi.h"

// Define the number of MCP23X17 chips
#define NUM_MCP 3

#define MAX_PINS 8

#define GET_SWITCH_MAP 

// Create an array of Adafruit_MCP23X17 objects
Adafruit_MCP23X17 mcp[NUM_MCP];

// Define I2C addresses for each MCP23X17
int mcp_addrs[NUM_MCP] = { 0x23, 0x27, 0x26 };

// Define a struct for the switch-to-I2C-pin mapping
struct SwitchI2CMapping {
  byte numThrows;
  byte MCP;
  byte pins[MAX_PINS];
};

// Create an array of mappings
const struct SwitchI2CMapping switchMappingTable[] = {
  /*{NUM_THROWs, GPIO EXPANDER NUMBER, {PIN FOR EACH THROW}} */
  /* Switch  1 */ { 8, 0, { 0, 1, 2, 3, 4, 5, 6, 7 } },
  /* Switch  2 */ { 4, 0, { 11, 10, 9, 8 } },
  /* Switch  3 */ { 4, 0, { 15, 14, 13, 12 } },
  /* Switch  4 */ { 4, 1, { 15, 14, 13, 12 } },
  /* Switch  5 */ { 1, 1, { 0 } },
  /* Switch  6 */ { 1, 1, { 1 } },
  /* Switch  7 */ { 1, 1, { 2 } },
  /* Switch  8 */ { 1, 1, { 3 } },
  /* Switch  9 */ { 1, 1, { 4 } },
  /* Switch 10 */ { 1, 1, { 5 } },
  /* Switch 11 */ { 1, 1, { 6 } },
  /* Switch 12 */ { 1, 1, { 7 } },
  /* Switch 13 */ { 4, 1, { 8, 9, 10, 11 } },
  /* Switch 14 */ { 4, 2, { 3, 2, 1, 0 } },
  /* Switch 15 */ { 4, 2, { 7, 6, 5, 4 } },
  /* Switch 16 */ { 4, 2, { 12, 13, 14, 15 } },
  /* Switch 17 */ { 4, 2, { 8, 9, 10, 11 } },

};

const int numSwitches = sizeof(switchMappingTable) / sizeof(switchMappingTable[0]);


void setPins(uint8_t chipNum, uint16_t pinMask, bool state) {
  uint8_t gpioA = pinMask & 0xFF;         // Lower 8 bits for GPIOA
  uint8_t gpioB = (pinMask >> 8) & 0xFF;  // Upper 8 bits for GPIOB

  Adafruit_MCP23X17& chip = mcp[chipNum];
  if (state) {
    // Set all pins to HIGH
    chip.writeGPIOA(gpioA | chip.readGPIOA());
    chip.writeGPIOB(gpioB | chip.readGPIOB());
  } else {
    // Set all pins to LOW
    chip.writeGPIOA(~gpioA & chip.readGPIOA());
    chip.writeGPIOB(~gpioB & chip.readGPIOB());
  }
}

void setPin(uint8_t chipNum, uint8_t pinNum, bool state) {
  Adafruit_MCP23X17& chip = mcp[chipNum];
  chip.digitalWrite(pinNum, state);
}

bool setSwitch(int switch_num, int throw_num) {
  if (switch_num <= 0 || switch_num > numSwitches) return false;
  const SwitchI2CMapping& sw = switchMappingTable[switch_num-1];  // Access correct mapping
  uint16_t pinMask = 0;
  for (int i = 0; i < sw.numThrows; i++) {
    bitSet(pinMask, sw.pins[i]);
  }

  // Set all pins to LOW
  setPins(sw.MCP, pinMask, LOW);

  // Set the specific pin for the throw to HIGH
  if (throw_num && throw_num <= sw.numThrows) {
    setPin(sw.MCP, sw.pins[throw_num - 1], HIGH);
  }

  return true;
}

int getSwitch(int switch_num) {
  // Access the correct mapping for the switch
  if (switch_num <= 0 || switch_num > numSwitches) return 0;
  const SwitchI2CMapping& sw = switchMappingTable[switch_num-1];

  // Read both GPIOA (lower 8 bits) and GPIOB (upper 8 bits)
  uint8_t gpioA = mcp[sw.MCP].readGPIOA();  // Reads pins 0-7
  uint8_t gpioB = mcp[sw.MCP].readGPIOB();  // Reads pins 8-15

  // Combine the results into one 16-bit value
  uint16_t gpioState = (gpioB << 8) | gpioA;

  // Loop through the throws (pins) and check their state in the 16-bit value
  for (int i = 0; i < sw.numThrows; i++) {
    // Check if the corresponding bit is set (HIGH)
    if (gpioState & (1 << sw.pins[i])) {
      return i + 1;  // Return the throw number (1-based index)
    }
  }

  // If no throw is set, return -1 (or any indicator for no throws active)
  return 0;
}



void resetSwitches(){
  for (int mcp_num = 0; mcp_num < NUM_MCP; mcp_num++) {
    setPins(mcp_num, 0xFFFF, 0);
  }
}


void setup() {
  Serial.begin(115200);

  // Initialize MCP23X17 chips
  for (int mcp_num = 0; mcp_num < NUM_MCP; mcp_num++) {
    mcp[mcp_num].begin_I2C(mcp_addrs[mcp_num]);
  }

  for (const struct SwitchI2CMapping& sw : switchMappingTable) {
    for(int t = 0; t < sw.numThrows; t++){
       mcp[sw.MCP].pinMode(sw.pins[t], OUTPUT);
    }
  }

}

void loop() {
  // Check if data is available on Serial
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove any trailing whitespace
    parseSCPI(input);
  }
}
