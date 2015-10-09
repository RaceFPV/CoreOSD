#ifndef CoreOSD_H_
#define CoreOSD_H_

#include "types.h"

	extern uint8_t Settings[];
	extern char screen[];
	extern char screenBuffer[];
	extern uint16_t flyingTime;
	
	//General use variables
	extern uint8_t TempBlinkAlarm;                       // Temporary for blink alarm
	extern uint8_t BlinkAlarm;                           // This is turning on and off at selected freq. (alarm)
	extern uint8_t Blink10hz;                            // This is turning on and off at 10hz
	extern uint16_t allSec;

	// For Time
	extern uint16_t onTime;
	extern uint16_t flyTime;

  // For Callsign
  //extern char callSign[];
  
	// For Amperage
	extern int16_t amperage_Int;
	extern float amperage;                // its the real value x10
	extern float amperagesum;

	// For Voltage
	extern uint16_t voltage;                      // its the value x10

	void resetFunc();
			
#endif
