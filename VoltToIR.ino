// Uncomment to enable debug messages
#define DEBUG

// Define NEC so that only the NEC sections of the IRRemote library are included
#define NEC

// Include header files
#include <Flash.h>
#include <IRremote.h>

// Pins
static const unsigned long RED_INPUT_PIN 0
static const unsigned long GREEN_INPUT_PIN 1
static const unsigned long BLUE_INPUT_PIN 2
static const unsigned long WHITE_INPUT_PIN 3

// Remote control codes
static const unsigned long POWER_CODE = 0x20DF02FD;
static const unsigned long DAYLIGHT_CODE = 0x20DF58A7;
static const unsigned long M1_CODE = 0x20DF18E7;
static const unsigned long M2_CODE = 0x20DF9867;s
static const unsigned long MOON_CODE = 0x20DFD827;
static const unsigned long RED_UP_CODE = 0x20DF0AF5;
static const unsigned long RED_DOWN_CODE = 0x20DF38C7;
static const unsigned long GREEN_UP_CODE = 0x20DF8A75;
static const unsigned long GREEN_DOWN_CODE = 0x20DFB847;
static const unsigned long BLUE_UP_CODE = 0x20DFB24D;
static const unsigned long BLUE_DOWN_CODE = 0x20DF7887;
static const unsigned long WHITE_UP_CODE = 0x20DF32CD;
static const unsigned long WHITE_DOWN_CODE = 0x20DFF807;

// Settings
static const byte RGBW_STEPS = 100;
static const short IR_DELAY = 5000;

// Data stored in ROM
FLASH_ARRAY(byte, fVoltageInputPins, RED_INPUT_PIN, GREEN_INPUT_PIN, BLUE_INPUT_PIN, WHITE_INPUT_PIN);
FLASH_TABLE(unsigned long, fRGBWUpDownCodes, 4, {RED_UP_CODE, RED_DOWN_CODE}, 
    {GREEN_UP_CODE, GREEN_DOWN_CODE}, {BLUE_UP_CODE, BLUE_DOWN_CODE}, {WHITE_UP_CODE, WHITE_DOWN_CODE});
FLASH_ARRAY(unsigned long, fPreprogrammedCodes, DAYLIGHT_CODE, M1_CODE, M2_CODE, MOON_CODE);
#ifdef DEBUG
FLASH_STRING_ARRAY(fColors, PSTR("red"), PSTR("green"), PSTR("blue"), PSTR("white"));
#endif

// Global variables
static byte[] gCurrentColorValues = {0, 0, 0, 0};
static byte[][] gPreprogrammedColorValues = {{100, 100, 100, 100}, {80, 80, 80, 80}, {60, 60, 60, 60}, 
    {40, 40, 40, 40}, {20, 20, 20, 20}, {0, 0, 0, 0}};
static IRsend gIRSend;
static unsigned long gLastIRTime = 0;

// Setup code
void setup()
{
#ifdef DEBUG
  // Setup the serial communication
  Serial.being(9600);
  Serial.println("Starting Volt to IR sketch ...");
#endif
}

// Main code
void loop()
{
  // Check if it's time to check if changes need to be made
  if ((long)(millis() - (gLastChangeTime + CHANGE_DELAY)) >= 0)
  {
    // Loop through the colors
    byte[4] newColorValues;
    for (byte i = 0; i < 4; ++i)
    {
      // Calculate the new color value based on the voltage
      newColorValues[i] = (byte)((float)analogRead(fVoltageInputPins[i]) / (float)1023 * 
                          (float)RGBW_VALUE_STEPS + (float)0.5)
#ifdef DEBUG
      // Log details
      Serial << "New calculated " << fColors[i] << " color value based on voltage is " << newColorValues[i];
#endif                          
    }
    
    // Check if there is a change in color values
    if (gCurrentColorValues[0] != newColorValues[0] || gCurrentColorValues[1] != newColorValues[1] ||
        gCurrentColorValues[2] != newColorValues[2] || gCurrentColorValues[3] != newColorValues[3])
    {
      // Loop through the preprogrammed color values
      bool irSent = false;
      for (byte i = 0; i < 4; ++i)
      {
        // Check if the new values match a set of preprogrammed color values
        if (newColorValues[0] == gPreprogrammedColorValues[i][0] &&
            newColorValues[1] == gPreprogrammedColorValues[i][1] &&
            newColorValues[2] == gPreprogrammedColorValues[i][2] &&
            newColorValues[3] == gPreprogrammedColorValues[i][3])
        {
          // Send the preprogrammed IR signal
          gIRSend.sendNEC(fPreprogrammedCodes[i], 32);
          
          // Set the flag
          irSent = true;
        }
      }
      
      // Check if we still need to send the IR codes to change the color values
      if (irSent == false)
      {
        // Loop through the colors
        for (byte i = 0; i < 4; ++i)
        {
          // Check if this color value changed
          if (newColorValues[i] > gCurrentColorValues[i])
          {
            // Check if we should wait so the IR signals aren't too close together
            if (irSent == true)
              delay(100);

            // Send the up IR signal
            gIRSend.sendNEC(fRGBWUpDownCodes[i][0], 32);
            
            // Adjust the current value
            gCurrentColorValues[i]++;
            
            // Set the flag
            irSent = true;
          }
          else if (newColorValues[i] < gCurrentColorValues[i])
          {
            // Check if we should wait so the IR signals aren't too close together
            if (irSent == true)
              delay(100);

            // Send the down IR signal
            gIRSend.sendNEC(fRGBWUpDownCodes[i][1], 32);
            
            // Adjust the current value
            gCurrentColorValues[i]--;
            
            // Set the flag
            irSent = true;
          }
        }
      }
      
      // Check if we sent an IR signal and set the last IR time
      if (irSent == true)
        gLastIRTime = millis();
    }
  }
}
