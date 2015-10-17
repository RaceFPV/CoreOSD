#ifndef TYPES_H_
#define TYPES_H_

#include "Arduino.h"
#include "Config.h"


// Settings Locations
enum Setting_ {
  S_VOLTAGEMIN,				// 7
  S_DIVIDERRATIO, 			// 9
  S_MAINVOLTAGE_VBAT, 		// 10
  S_VIDEOSIGNALTYPE, 		// 13
  S_BLINKINGHZ,    			// 15 selectable alarm blink freq
  S_CURRSENSSENSITIVITY, 	// 17
  S_CURRSENSOFFSET_H,		// 18
  S_CURRSENSOFFSET_L,		// 19
  // EEPROM_SETTINGS must be last for H/W settings!
  EEPROM_SETTINGS,  		//20
  
  // EEPROM_ITEM_LOCATION must be last for Items location!
  EEPROM_ITEM_LOCATION				// 59
};

#endif /* TYPES_H_ */
