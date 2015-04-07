// Define NEC so that only the NEC sections of the IRRemote library are included
#define NEC

// Include needed header files
#include <avr/pgmspace.h>
#include <IRremote.h>

// Define used pins
#define RED_INPUT_PIN 0
#define GREEN_INPUT_PIN 1
#define BLUE_INPUT_PIN 2
#define WHITE_INPUT_PIN 3

// Define remote control codes
#define POWER 0x20DF02FD
#define RED_UP 0x20DF0AF5
#define RED_DOWN 0x20DF38C7
#define GREEN_UP 0x20DF8A75
#define GREEN_DOWN 0x20DFB847
#define BLUE_UP 0x20DFB24D
#define BLUE_DOWN 0x20DF7887
#define WHITE_UP 0x20DF32CD
#define WHITE_DOWN 0x20DFF807
#define DAYLIGHT 0x20DF58A7
#define M2 0x20DF9867
#define M1 0x20DF18E7
#define MOON 0x20DFD827

// Define settings
#define RGBW_STEPS = 100
#define UP_DOWN_DELAY = 1000

// Data stored in ROM
const PROGMEM byte gVoltagePins[] = {RED_INPUT_PIN, GREEN_INPUT_PIN, BLUE_INPUT_PIN, WHITE_INPUT_PIN};
const PROGMEM unsigned long gRGBWCodes[][] = {{RED_UP, RED_DOWN}, {GREEN_UP, GREEN_DOWN},
    {BLUE_UP, BLUE_DOWN}, {WHITE_UP, WHITE_DOWN}};

// Global variables
unsigned long gLastAdjustmentTime = 0;
byte[] gCurrentValues = {0, 0, 0, 0};
IRsend irsend;

// Setup code
void setup()
{
  // Setup the serial communication
  Serial.being(9600);
  Serial.println("Starting Volt to IR sketch ...");
}

// Main code
void loop()
{
  // Check if it's time to make adjustments again
  if ((long)(millis() - (gLastAdjustment + UP_DOWN_DELAY)) >= 0)
  {
    // Loop through the colors
    bool adjustmentMade = false;
    for (byte i = 0; i < 4; ++i)
    {
      short voltage = analogRead(gVoltagePins[i])
      if (gCurrentValues[i] / RGBW_STEPS < voltage / 1023)
      {
        // Send the IR signal
        irsend.sendNEC(gRGBWCodes[i][0], 32);

        // Adjust the current value
        gCurrentColorValues[i] += gCurrentValues[i] / RGBW_STEPS;

        // Set the adjustment flag
        adjustmentMade = true;
      }
      else if (gCurrentValues[i] / RGBW_STEPS > voltage / 1023)
      {      
        // Send the IR signal
        irsend.sendNEC(gRGBWCodes[i][1], 32)

        // Adjust the current value
        gCurrentColorValues[i] -= gCurrentValues[i] / RGBW_STEPS;

        // Set the adjustment flag
        adjustmentMade = true;
      }        
    }

    // Change the last adjustment time if we made adjustments
    if (adjustmentMade == true)
      gLastAdjustmentTime = millis()
  }
}