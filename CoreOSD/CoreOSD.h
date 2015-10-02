#ifndef CoreOSD_H_
#define CoreOSD_H_

#include "types.h"

	extern uint8_t Settings[];
	extern char screen[];
	extern char screenBuffer[];
	extern MW_ATTITUDE_t MW_ATT;
	extern MW_imu_t MW_IMU;
	extern uint16_t MwRcData[];
	extern MW_status_t MW_STATUS;
	extern MW_ANALOG_t MW_ANALOG;
	extern uint16_t flyingTime;
	
	//General use variables
	extern uint8_t TempBlinkAlarm;                       // Temporary for blink alarm
	extern uint8_t BlinkAlarm;                           // This is turning on and off at selected freq. (alarm)
	extern uint8_t Blink10hz;                            // This is turning on and off at 10hz
	extern uint8_t rssiTimer;
	extern uint16_t allSec;

	// For Time
	extern uint16_t onTime;
	extern uint16_t flyTime;
	
	// For Amperage
	extern float amperageADC;
	extern int16_t amperage_Int;
	extern float amperage;                // its the real value x10
	extern float amperagesum;

	// Rssi
	extern int16_t rssi;
	extern int16_t rssiADC;
	extern int16_t rssiMin;
	extern int16_t rssiMax;
	extern int16_t rssi_Int;

	// For Voltage
	extern uint16_t voltage;                      // its the value x10

	void resetFunc();
			
#endif
