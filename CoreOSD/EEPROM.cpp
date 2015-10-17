#include <avr/eeprom.h>
#include "Arduino.h"
#include "EEPROM.h"
#include "types.h"
#include "CoreOSD.h"


uint8_t EEPROMClass::read(int address)
{
	return eeprom_read_byte((unsigned char *) address);
}

void EEPROMClass::write(int address, uint8_t value)
{
	eeprom_write_byte((unsigned char *) address, value);
}

EEPROMClass EEPROM;

// For Settings Defaults
static const uint8_t EEPROM_DEFAULT[EEPROM_SETTINGS] = {
105, // S_VOLTAGEMIN                7
100, // S_DIVIDERRATIO              9
1,   // S_MAINVOLTAGE_VBAT          10
1,   // S_VIDEOSIGNALTYPE           19
5,   // S_BLINKINGHZ                22   // 10=1Hz, 9=1.1Hz, 8=1,25Hz, 7=1.4Hz, 6=1.6Hz, 5=2Hz, 4=2,5Hz, 3=3,3Hz, 2=5Hz, 1=10Hz
40,  // S_CURRSENSSENSITIVITY       24   // May vary from 17 to 40mV/A (Sensor type)
2,   // S_CURRSENSOFFSET_H          25   // offset(H/L) =0 for unidir sensors or =512 for bidirectional sensors, may be changed only of few units.
0,   // S_CURRSENSOFFSET_L          26   // 2H+0L=512
};


void checkEEPROM(void)
{
	// For H/W Settings
	uint8_t EEPROM_Loaded = EEPROM.read(0);
	if (!EEPROM_Loaded){
		for(uint8_t en=0;en<EEPROM_SETTINGS;en++){
			if (EEPROM.read(en) != EEPROM_DEFAULT[en])  EEPROM.write(en,EEPROM_DEFAULT[en]);
		}
	}
}

void writeEEPROM(void)
{
	// For Settings
	for(uint16_t en=0;en<EEPROM_SETTINGS;en++){
		if (EEPROM.read(en) != Settings[en]) EEPROM.write(en,Settings[en]);
	}
	// For Position of items on screen
	for(uint16_t en=0;en<EEPROM_ITEM_LOCATION-EEPROM_SETTINGS;en++){
		if (EEPROM.read(en+EEPROM_SETTINGS+1) != Settings[en+EEPROM_SETTINGS+1]) EEPROM.write(en+EEPROM_SETTINGS+1,Settings[en+EEPROM_SETTINGS+1]);
	}
}

void readEEPROM(void)
{
	// For Settings
	for(uint16_t en=0;en<EEPROM_SETTINGS;en++){
		Settings[en] = EEPROM.read(en);
	}
	// For Position of items on screen
	for(uint16_t en=0;en<EEPROM_ITEM_LOCATION-EEPROM_SETTINGS;en++){
		Settings[en+EEPROM_SETTINGS+1] = EEPROM.read(en+EEPROM_SETTINGS+1);
	}
}
