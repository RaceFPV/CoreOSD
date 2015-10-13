#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "Config.h"
#include "types.h"
#include "EEPROM.h" //Needed to access eeprom read/write functions
#include "Max7456.h"
#include "symbols.h"
#include "Screen.h"
#include "CoreOSD.h"

//General use variables
uint8_t tenthSec=0;
uint8_t TempBlinkAlarm=0;                       // Temporary for blink alarm
uint8_t BlinkAlarm=0;                           // This is turning on and off at selected freq. (alarm)
uint8_t Blink10hz=0;                            // This is turning on and off at 10hz

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
uint16_t flyTime=0;

// For Callsign
char callSign[] = CALLSIGN;

// For Amperage
float amperageADC =0;
int16_t amperage_Int=0;
float amperage = 0;                // its the real value x10
float amperagesum = 0;

// For Voltage
uint16_t voltage=0;                      // its the value x10

// For Statistics
uint16_t flyingTime=0;

//-------------- Timed Service Routine vars (No need Metro.h library)
uint16_t previous_millis_low=0;
uint16_t previous_millis_high =0;
uint8_t hi_speed_cycle = 50;
uint8_t lo_speed_cycle = 100;
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


void calculateAmperage(void)
{
	float aa=0;
	// calculation of amperage [A*10]
	aa = amperageADC*10;   // *10 We want one decimal digit
	amperage_Int += ( ( (signed int)((aa* 10) - amperage_Int )) / 10 );  // Smoothing the displayed value with average of 10 samples
	amperage = (amperage_Int / 10);
	if (amperage >=999) amperage=999;  // [A*10]
	
	// Calculation of amperagesum = amperage integration on 50msec (without reading average)
	// 720= *100/72000 --> where:
	// *100=mA (instead of *1000 as the value is already multiplied by 10)
	// 72000=n. of 50ms in 1 hour
	amperagesum += aa /720; // [mAh]    // NOTE: if want add 3%, change this "amperagesum += aa /690;"
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
      voltage = float(voltageRaw) * 100 / 1023;

  //---------------  Start Timed Service Routines  ---------------------------------------
  uint16_t currentMillis = millis();

  if((currentMillis - previous_millis_low) >= lo_speed_cycle)  // 10 Hz (Executed every 100ms)
  {
    previous_millis_low = currentMillis;

	tenthSec++;
	TempBlinkAlarm++;
	Blink10hz=!Blink10hz;    
  }  // End of slow Timed Service Routine (100ms loop)

  if((currentMillis - previous_millis_high) >= hi_speed_cycle)  // 20 Hz (Executed every 50ms)
  {
    previous_millis_high = currentMillis;   
       
        //if statements to define what to show on screen.
        //this should be moved to proper c++ code, not if statements
        if(showVoltage == 1){
          displayVoltage();
        }
        if(showTimer == 1){
          displayTime();
        }
        if(showAmps == 1){
          displayAmperage();
        }
        if(showCallsign == 1){
          displayCallsign();
        }
        if(showMah == 1){
          displaypMeterSum();
        }
    
	 MAX7456_DrawScreen();
  }  // End of fast Timed Service Routine (50ms loop)
//---------------------  End of Timed Service Routine ---------------------------------------


  if(TempBlinkAlarm >= Settings[S_BLINKINGHZ]) {    // selectable alarm blink freq
    TempBlinkAlarm = 0;
    BlinkAlarm =!BlinkAlarm;     // 10=1Hz, 9=1.1Hz, 8=1.25Hz, 7=1.4Hz, 6=1.6Hz, 5=2Hz, 4=2.5Hz, 3=3.3Hz, 2=5Hz, 1=10Hz
  }

 
  if(tenthSec >= 10)     // this execute 1 time a second
  {
    onTime++;
    
    tenthSec=0;

      flyTime++;
      flyingTime++;
    allSec++;
  }

}  // End of main loop



