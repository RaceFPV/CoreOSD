#ifndef TYPES_H_
#define TYPES_H_

#include "Arduino.h"
#include "Config.h"


typedef struct {
	uint16_t cycleTime;
	int16_t I2CError;
	uint16_t  sensorPresent;
	uint32_t  sensorActive;
	uint8_t version;
} MW_status_t;

typedef struct {
	int16_t Angle[2];
	int16_t Heading;
} MW_ATTITUDE_t;

typedef struct {
	int16_t  accSmooth[3];
	int16_t  gyroData[3];
	int16_t  magADC[3];
} MW_imu_t;

typedef struct {
	uint8_t VBat;
	uint16_t pMeterSum;
	uint16_t Rssi;
	uint16_t Amperage;
} MW_ANALOG_t;

// Settings Locations
enum Setting_ {
  S_CHECK_,					// 0 used for check
  S_RSSIMIN,    			// 1
  S_RSSIMAX,    			// 2
  S_RSSI_ALARM,				// 3
  S_MWRSSI, 				// 4
  S_PWMRSSI,				// 5
  S_PWMRSSIDIVIDER,			// 6
  S_VOLTAGEMIN,				// 7
  S_BATCELLS,				// 8
  S_DIVIDERRATIO, 			// 9
  S_MAINVOLTAGE_VBAT, 		// 10
  S_BOARDTYPE, 				// 11
  S_UNITSYSTEM,				// 12
  S_VIDEOSIGNALTYPE, 		// 13
  S_ENABLEADC,				// 14
  S_BLINKINGHZ,    			// 15 selectable alarm blink freq
  S_MWAMPERAGE,				// 16
  S_CURRSENSSENSITIVITY, 	// 17
  S_CURRSENSOFFSET_H,		// 18
  S_CURRSENSOFFSET_L,		// 19
  // EEPROM_SETTINGS must be last for H/W settings!
  EEPROM_SETTINGS,  		//20
  
// Screen item Locations
// ********* EEProm enum data position and display On/Off option for all items on screen ****************
// Enum valid for both PAL/NTSC  
  L_SENSORPOSITIONROW,				
  L_SENSORPOSITIONCOL,				// 22
  L_SENSORPOSITIONDSPL,
  L_MODEPOSITIONROW,
  L_MODEPOSITIONCOL, 
  L_MODEPOSITIONDSPL,
  L_HORIZONPOSITIONROW,
  L_HORIZONPOSITIONCOL,
  L_HORIZONPOSITIONDSPL,
  L_HORIZONSIDEREFROW,			// 30
  L_HORIZONSIDEREFCOL,
  L_HORIZONSIDEREFDSPL,
  L_HORIZONCENTERREFROW,
  L_HORIZONCENTERREFCOL,
  L_HORIZONCENTERREFDSPL,			// 35
  L_FLYTIMEPOSITIONROW,
  L_FLYTIMEPOSITIONCOL,				// 37
  L_FLYTIMEPOSITIONDSPL,
  L_ONTIMEPOSITIONROW,
  L_ONTIMEPOSITIONCOL,
  L_ONTIMEPOSITIONDSPL,
  L_RSSIPOSITIONROW,
  L_RSSIPOSITIONCOL,				// 43
  L_RSSIPOSITIONDSPL,
  L_VOLTAGEPOSITIONROW,
  L_VOLTAGEPOSITIONCOL,
  L_VOLTAGEPOSITIONDSPL,
  L_MAINBATLEVEVOLUTIONROW,			// 48
  L_MAINBATLEVEVOLUTIONCOL,
  L_MAINBATLEVEVOLUTIONDSPL,  
  L_AMPERAGEPOSITIONROW,
  L_AMPERAGEPOSITIONCOL,
  L_AMPERAGEPOSITIONDSPL,
  L_PMETERSUMPOSITIONROW,
  L_PMETERSUMPOSITIONCOL,			// 55
  L_PMETERSUMPOSITIONDSPL,
  
  // EEPROM_ITEM_LOCATION must be last for Items location!
  EEPROM_ITEM_LOCATION				// 59
};

#endif /* TYPES_H_ */
