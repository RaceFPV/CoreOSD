#ifndef CoreOSD_H_
#define CoreOSD_H_

#include "types.h"

	extern uint8_t Settings[];
	extern char screen[];
	extern char screenBuffer[];
	extern conf_t conf;
	extern MW_ATTITUDE_t MW_ATT;
	extern MW_imu_t MW_IMU;
	extern uint16_t MwRcData[];
	extern MW_status_t MW_STATUS;
	extern MW_ANALOG_t MW_ANALOG;
	extern uint32_t mode_horizon;
	extern uint32_t modeMSPRequests;
	extern uint32_t queuedMSPRequests;
	//extern int16_t temperature;
	extern uint16_t speedMAX;
	extern int8_t temperMAX;
	extern uint16_t flyingTime;
	extern const unsigned char temperatureUnitAdd[];
	
	//General use variables
	extern uint8_t TempBlinkAlarm;                       // Temporary for blink alarm
	extern uint8_t BlinkAlarm;                           // This is turning on and off at selected freq. (alarm)
	extern uint8_t Blink10hz;                            // This is turning on and off at 10hz
	extern uint8_t rssiTimer;
	extern uint16_t allSec;

	// Config status and cursor location
	extern uint8_t ROW;
	extern uint8_t COL;
	extern uint8_t configPage;
	extern uint8_t configMode;
	extern uint8_t fontMode;
	extern uint8_t fontData[];
	extern uint8_t nextCharToRequest;
	extern uint8_t lastCharToRequest;
	extern uint8_t retransmitQueue;
	
	//For Current Throttle
	extern uint16_t LowT;
	extern uint16_t HighT;

	// For Time
	extern uint16_t onTime;
	extern uint16_t flyTime;
	
	// For Heading
	extern const char headGraph[] PROGMEM; 
	
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
	extern uint16_t vidvoltage;                   // its the value x10
		
	// Variables for items pos change on screen
	//-----------------------------------------------------------
	extern int8_t screenitemselect; // pointer for item text strings
	extern int8_t screen_pos_item_pointer;  // 0; // pointer for first item display/row/col positions
        	
	extern PROGMEM const char * const item_table[];

	void resetFunc();
			
#endif
