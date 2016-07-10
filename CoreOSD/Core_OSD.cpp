#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "Config.h"
#include "types.h"
#include "EEPROM.h" //Needed to access eeprom read/write functions for screen size
#include "Max7456.h"
#include "Screen.h"
#include "CoreOSD.h"

//General use variables

struct  __timer {
  uint8_t  tenthSec;
  uint8_t  halfSec;
  uint8_t  Blink2hz;                          // This is turing on and off at 2hz
  uint8_t  Blink10hz;                         // This is turing on and off at 10hz
  uint32_t seconds;
}
timer;
uint8_t fontMode = 0;
uint16_t allSec=0;

// Mode bits
uint8_t Settings[EEPROM_ITEM_LOCATION];

// For Settings Defaults

// Screen is the Screen buffer between program an MAX7456 that will be writen to the screen at 10hz
char screen[480];
// ScreenBuffer is an intermietary buffer to created Strings to send to Screen buffer
char screenBuffer[20];

// For Time
uint16_t onTime=0;

// For Callsign
char callSign[] = CALLSIGN;

// For Voltage
uint16_t voltage=0;                      // its the value x10

//-------------- Timed Service Routine vars (No need Metro.h library)
unsigned long previous_millis_low=0;
unsigned long previous_millis_high =0;
unsigned long previous_millis_sync =0;
unsigned long sync_speed_cycle =33;
unsigned long hi_speed_cycle = 50;
unsigned long lo_speed_cycle = 100;
//----------------


void setup()
{ 
  //Led output
  pinMode(7,OUTPUT);  // PD7
 
  checkEEPROM();
  readEEPROM();
  MAX7456_Setup();
  
  analogReference(DEFAULT);
}


void loop()
{
       // Process AI   
      static uint16_t ind = 0;
      static uint32_t voltageRawArray[8];
      voltageRawArray[(ind++)%8] = analogRead(voltagePin);                  
      uint16_t voltageRaw = 0;
      for (uint16_t i=0;i<8;i++)
      voltageRaw += voltageRawArray[i];
      voltage = float(voltageRaw) * 100 / 1020;

  //---------------  Start Timed Service Routines  ---------------------------------------
  unsigned long currentMillis = millis();

  //enable high speed
  if((currentMillis - previous_millis_sync) >= sync_speed_cycle)  // (Executed > NTSC/PAL hz 33ms)
  {
    previous_millis_sync = previous_millis_sync+sync_speed_cycle;
  }

  if((currentMillis - previous_millis_low) >= lo_speed_cycle)  // 10 Hz (Executed every 100ms)
  {
    previous_millis_low = previous_millis_low+lo_speed_cycle;    
    timer.tenthSec++;
    timer.halfSec++;
    timer.Blink10hz=!timer.Blink10hz;    
  }  // End of slow Timed Service Routine (100ms loop)

  if((currentMillis - previous_millis_high) >= hi_speed_cycle)  // 20 Hz (Executed every 50ms)
  {
    previous_millis_high = previous_millis_high+hi_speed_cycle;
       
        //if statements to define what to show on screen.
        //this should be moved to proper c++ code, not if statements
        if(showVoltage == 1){
          displayVoltage();
        }
        if(showTimer == 1){
          displayTime();
        }
        if(showCallsign == 1){
          displayCallsign();
        }
        if(showCrosshair == 1){
          displayCrosshair();
        }
    
	 MAX7456_DrawScreen();
  }  // End of fast Timed Service Routine (50ms loop)
//---------------------  End of Timed Service Routine ---------------------------------------
 
  if(millis() > timer.seconds+1000)     // this execute 1 time a second
  {
    timer.seconds+=1000;
    timer.tenthSec=0;
    onTime++;
    //check to see if the MAX chip crashed
    MAX7456Stalldetect();
    allSec++;
  }

}  // End of main loop



