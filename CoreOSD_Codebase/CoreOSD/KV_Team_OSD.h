#ifndef KV_TEAM_OSD_H_
#define KV_TEAM_OSD_H_

#include "types.h"

	extern uint8_t Settings[];
	extern char screen[];
	extern char screenBuffer[];
	extern conf_t conf;
	extern MW_ALTTITUDE_t MW_ALT;
	extern MW_ATTITUDE_t MW_ATT;
	extern MW_imu_t MW_IMU;
	extern uint16_t MwRcData[];
	extern MW_status_t MW_STATUS;
	extern MW_ANALOG_t MW_ANALOG;
	extern uint8_t armed;
	extern uint8_t previousarmedstatus;  // for statistics after disarming
	extern GPS_t GPS;
	extern uint32_t mode_armed;
	extern uint32_t mode_stable;
	extern uint32_t mode_horizon;
	extern uint32_t mode_baro;
	extern uint32_t mode_mag;
	extern uint32_t mode_gpshome;
	extern uint32_t mode_gpshold;
	extern uint32_t mode_osd_switch;
	extern uint32_t modeMSPRequests;
	extern uint32_t queuedMSPRequests;
	//extern int16_t temperature;
	extern uint16_t speedMAX;
	extern int8_t temperMAX;
	extern int16_t altitudeMAX;
	extern int16_t distanceMAX;
	extern float trip;
	extern uint16_t flyingTime;
	extern const unsigned char temperatureUnitAdd[];
	
	//General use variables
	extern uint8_t TempBlinkAlarm;                       // Temporary for blink alarm
	extern uint8_t BlinkAlarm;                           // This is turning on and off at selected freq. (alarm)
	extern uint8_t Blink10hz;                            // This is turning on and off at 10hz
	extern uint16_t lastCallSign;                         // callsign_timer
	extern uint8_t rssiTimer;
	extern uint8_t accCalibrationTimer;
	extern uint8_t magCalibrationTimer;
	extern uint8_t eepromWriteTimer;
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
		
	// Menu strings
	extern const char disarmed_text[] PROGMEM;
	extern const char armed_text[] PROGMEM;
	// For Intro
	extern const char message0[] PROGMEM;
	extern const char message5[] PROGMEM;
	extern const char message6[] PROGMEM;
	extern const char message7[] PROGMEM;
	extern const char message8[] PROGMEM;
	extern const char message9[] PROGMEM;         // Call Sign on the beginning of the transmission

	// For Config menu common
	extern const char configMsgON[] PROGMEM;
	extern const char configMsgOFF[] PROGMEM;
	extern const char configMsgNoAct[] PROGMEM;
	extern const char configMsgEXIT[] PROGMEM;
	extern const char configMsgSAVE[] PROGMEM;
	extern const char configMsgPGS[] PROGMEM;
	extern const char configMsgNTSC[] PROGMEM;
	extern const char configMsgPAL[] PROGMEM;

	// For Config pages
	//-----------------------------------------------------------Page1
	extern const char configMsg10[] PROGMEM;
	extern const char configMsg11[] PROGMEM;
	extern const char configMsg12[] PROGMEM;
	extern const char configMsg13[] PROGMEM;
	extern const char configMsg14[] PROGMEM;
	extern const char configMsg15[] PROGMEM;
	extern const char configMsg16[] PROGMEM;
	extern const char configMsg17[] PROGMEM;
	//-----------------------------------------------------------Page2
	extern const char configMsg20[] PROGMEM;
	extern const char configMsg21[] PROGMEM;
	extern const char configMsg22[] PROGMEM;
	extern const char configMsg23[] PROGMEM;
	extern const char configMsg24[] PROGMEM;
	extern const char configMsg25[] PROGMEM;
	extern const char configMsg26[] PROGMEM;
	extern const char configMsg27[] PROGMEM;
	//-----------------------------------------------------------Page3
	extern const char configMsg30[] PROGMEM;
	extern const char configMsg31[] PROGMEM;
	extern const char configMsg32[] PROGMEM;
	extern const char configMsg33[] PROGMEM;
	//-----------------------------------------------------------Page4
	extern const char configMsg40[] PROGMEM;
	extern const char configMsg41[] PROGMEM;
	extern const char configMsg42[] PROGMEM;
	extern const char configMsg43[] PROGMEM;
	extern const char configMsg44[] PROGMEM;
	//-----------------------------------------------------------Page5
	extern const char configMsg50[] PROGMEM;
	extern const char configMsg51[] PROGMEM;
	extern const char configMsg52[] PROGMEM;
	extern const char configMsg53[] PROGMEM;
	extern const char configMsg54[] PROGMEM;
	extern const char configMsg55[] PROGMEM;
	extern const char configMsg56[] PROGMEM;
	extern const char configMsg57[] PROGMEM;
	//-----------------------------------------------------------Page6
	extern const char configMsg60[] PROGMEM;
	extern const char configMsg61[] PROGMEM;
	extern const char configMsg62[] PROGMEM;
	extern const char configMsg63[] PROGMEM;
	//-----------------------------------------------------------Page7
	extern const char configMsg70[] PROGMEM;
	extern const char configMsg71[] PROGMEM;
	extern const char configMsg72[] PROGMEM;
	extern const char configMsg73[] PROGMEM;
	extern const char configMsg74[] PROGMEM;
	extern const char configMsg75[] PROGMEM;
	extern const char configMsg76[] PROGMEM;
	//-----------------------------------------------------------Page8
	extern const char configMsg80[] PROGMEM;
	extern const char configMsg81[] PROGMEM;
	extern const char configMsg82[] PROGMEM;
	//-----------------------------------------------------------Page9
	extern const char configMsg90[] PROGMEM;
	extern const char configMsg91[] PROGMEM;
	extern const char configMsg92[] PROGMEM;
	extern const char configMsg93[] PROGMEM;
	extern const char configMsg94[] PROGMEM;
	extern const char configMsg95[] PROGMEM;
	extern const char configMsg96[] PROGMEM;
	extern const char configMsg97[] PROGMEM;
	
	// Variables for items pos change on screen
	//-----------------------------------------------------------
	extern int8_t screenitemselect; // pointer for item text strings
	extern int8_t screen_pos_item_pointer;  // 0; // pointer for first item display/row/col positions
        	
	extern PROGMEM const char * const item_table[];
	
	// POSITION OF EACH CHARACTER OR LOGO IN THE MAX7456
	extern const unsigned char speedUnitAdd[];
	extern const unsigned char speedUnitAdd1[];
	extern const unsigned char temperatureUnitAdd[];
	extern const char MultiWiiLogoL1Add[] PROGMEM;
	extern const char MultiWiiLogoL2Add[] PROGMEM;
	extern const char MultiWiiLogoL3Add[] PROGMEM;
	extern const unsigned char MwAltitudeAdd[];
	extern const unsigned char MwClimbRateAdd[];
	extern const unsigned char GPS_distanceToHomeAdd[];
	extern const unsigned char GPS_distanceToHomeAdd1[];
	extern const unsigned char MwGPSAltPositionAdd[];
	extern const unsigned char MwGPSAltPositionAdd1[];
	//extern const char KVTeamVersionPosition;

	//void initFontMode();
	//int16_t getNextCharToRequest();
	void resetFunc();
			
#endif /* KV_TEAM_OSD_H_ */
