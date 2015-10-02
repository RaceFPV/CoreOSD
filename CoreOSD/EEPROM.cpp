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
1,   // used for check              0

0,   // S_RSSIMIN                   1
255, // S_RSSIMAX                   2
60,  // S_RSSI_ALARM                3
1,   // S_MWRSSI                    4
0,   // S_PWMRSSI                   5
8,   // S_PWMRSSIDIVIDER            6    // PWM Freq 500Hz=8, 1KHz=4 (Divider to avoid value >255)
105, // S_VOLTAGEMIN                7
3,   // S_BATCELLS                  8
100, // S_DIVIDERRATIO              9
1,   // S_MAINVOLTAGE_VBAT          10
0,   // S_UNITSYSTEM                18
1,   // S_VIDEOSIGNALTYPE           19
1,   // S_ENABLEADC                 21
5,   // S_BLINKINGHZ                22   // 10=1Hz, 9=1.1Hz, 8=1,25Hz, 7=1.4Hz, 6=1.6Hz, 5=2Hz, 4=2,5Hz, 3=3,3Hz, 2=5Hz, 1=10Hz
0,   // S_MWAMPERAGE                23
40,  // S_CURRSENSSENSITIVITY       24   // May vary from 17 to 40mV/A (Sensor type)
2,   // S_CURRSENSOFFSET_H          25   // offset(H/L) =0 for unidir sensors or =512 for bidirectional sensors, may be changed only of few units.
0,   // S_CURRSENSOFFSET_L          26   // 2H+0L=512
};


// PAL item position Defaults
static const uint8_t EEPROM_PAL_DEFAULT[EEPROM_ITEM_LOCATION-EEPROM_SETTINGS] = {
	// ROW= Row position on screen (255= no action)
	// COL= Column position on screen (255= no action)
	// DSPL= Display item on screen

	6,   // L_HORIZONPOSITIONROW LINE06+8
	8,   // L_HORIZONPOSITIONCOL
	0,   // L_HORIZONPOSITIONDSPL

	255, // L_HORIZONSIDEREFROW
	255, // L_HORIZONSIDEREFCOL
	0,   // L_HORIZONSIDEREFDSPL

	255, // L_HORIZONCENTERREFROW
	255, // L_HORIZONCENTERREFCOL
	0,   // L_HORIZONCENTERREFDSPL

	15,   // L_FLYTIMEPOSITIONROW LINE15+22
	16,   // L_FLYTIMEPOSITIONCOL
	0,    // L_FLYTIMEPOSITIONDSPL

	15,   // L_ONTIMEPOSITIONROW LINE15+22
	16,   // L_ONTIMEPOSITIONCOL
	1,    // L_ONTIMEPOSITIONDSPL

	2,    // L_RSSIPOSITIONROW LINE14+2
	12,   // L_RSSIPOSITIONCOL
	1,    // L_RSSIPOSITIONDSPL

	15,   // L_VOLTAGEPOSITIONROW LINE15+3
	23,   // L_VOLTAGEPOSITIONCOL
	1,    // L_VOLTAGEPOSITIONDSPL

	255,  // L_MAINBATLEVEVOLUTIONROW,
	255,  // L_MAINBATLEVEVOLUTIONCOL,
	1,    // L_MAINBATLEVEVOLUTIONDSPL,

	13,   // L_VIDVOLTAGEPOSITIONROW LINE13+3
	23,   // L_VIDVOLTAGEPOSITIONCOL
	0,    // L_VIDVOLTAGEPOSITIONDSPL

	14,   // L_AMPERAGEPOSITIONROW LINE15+10
	23,   // L_AMPERAGEPOSITIONCOL
	0,    // L_AMPERAGEPOSITIONDSPL

	14,   // L_PMETERSUMPOSITIONROW LINE15+16
	14,   // L_PMETERSUMPOSITIONCOL
	0,    // L_PMETERSUMPOSITIONDSPL
};


// NTSC item position Defaults
uint8_t EEPROM_NTSC_DEFAULT[EEPROM_ITEM_LOCATION-EEPROM_SETTINGS] = {
	// ROW= Row position on screen (255= no action)
	// COL= Column position on screen (255= no action)
	// DSPL= Display item on screen

	/*12,  // L_TEMPERATUREPOSROW LINE11+2       // Do not remove yet
	2,   // L_TEMPERATUREPOSCOL
	0,   // L_TEMPERATUREPOSDSPL*/

	6,   // L_HORIZONPOSITIONROW LINE06+8
	8,   // L_HORIZONPOSITIONCOL
	0,   // L_HORIZONPOSITIONDSPL

	255, // L_HORIZONSIDEREFROW
	255, // L_HORIZONSIDEREFCOL
	0,   // L_HORIZONSIDEREFDSPL

	255, // L_HORIZONCENTERREFROW
	255, // L_HORIZONCENTERREFCOL
	0,   // L_HORIZONCENTERREFDSPL
  
	13,   // L_FLYTIMEPOSITIONROW LINE15+22
	16,   // L_FLYTIMEPOSITIONCOL
	1,    // L_FLYTIMEPOSITIONDSPL

	13,   // L_ONTIMEPOSITIONROW LINE15+22
	14,   // L_ONTIMEPOSITIONCOL
	0,    // L_ONTIMEPOSITIONDSPL

	2,    // L_RSSIPOSITIONROW LINE14+2
	12,   // L_RSSIPOSITIONCOL
	1,    // L_RSSIPOSITIONDSPL

	13,   // L_VOLTAGEPOSITIONROW LINE15+3
	23,   // L_VOLTAGEPOSITIONCOL
	1,    // L_VOLTAGEPOSITIONDSPL

	255,  // L_MAINBATLEVEVOLUTIONROW
	255,  // L_MAINBATLEVEVOLUTIONCOL
	1,    // L_MAINBATLEVEVOLUTIONDSPL

	11,   // L_VIDVOLTAGEPOSITIONROW LINE13+3
	23,   // L_VIDVOLTAGEPOSITIONCOL
	0,    // L_VIDVOLTAGEPOSITIONDSPL

	12,   // L_AMPERAGEPOSITIONROW LINE15+10
	23,   // L_AMPERAGEPOSITIONCOL
	0,    // L_AMPERAGEPOSITIONDSPL

	12,   // L_PMETERSUMPOSITIONROW LINE15+16
	14,   // L_PMETERSUMPOSITIONCOL
	0,    // L_PMETERSUMPOSITIONDSPL
};

void checkEEPROM(void)
{
	// For H/W Settings
	uint8_t EEPROM_Loaded = EEPROM.read(0);
	if (!EEPROM_Loaded){
		for(uint8_t en=0;en<EEPROM_SETTINGS;en++){
			if (EEPROM.read(en) != EEPROM_DEFAULT[en])  EEPROM.write(en,EEPROM_DEFAULT[en]);
		}
		// For items on screen.
		// First run, the default will be NTSC (show all data lines with NTSC systems that has only 13 lines)
		// In OSD menu' it's possible a quick default setup for PAL or NTSC
		for(uint16_t en=0;en<EEPROM_ITEM_LOCATION-EEPROM_SETTINGS;en++) {
			if (EEPROM.read(en+EEPROM_SETTINGS+1) != EEPROM_NTSC_DEFAULT[en]) EEPROM.write(en+EEPROM_SETTINGS+1,EEPROM_NTSC_DEFAULT[en]);
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

// back to default setting & position for PAL/NTSC
void WriteScreenLayoutDefault(void)
{
	if (Settings[S_VIDEOSIGNALTYPE]){  // PAL
		for(uint16_t en=0;en<EEPROM_ITEM_LOCATION-EEPROM_SETTINGS;en++) {
			if (EEPROM.read(en+EEPROM_SETTINGS+1) != EEPROM_PAL_DEFAULT[en]) EEPROM.write(en+EEPROM_SETTINGS+1,EEPROM_PAL_DEFAULT[en]);
		}
	}
	else {
		for(uint16_t en=0;en<EEPROM_ITEM_LOCATION-EEPROM_SETTINGS;en++) {
			if (EEPROM.read(en+EEPROM_SETTINGS+1) != EEPROM_NTSC_DEFAULT[en]) EEPROM.write(en+EEPROM_SETTINGS+1,EEPROM_NTSC_DEFAULT[en]);
		}
	}
	readEEPROM();  // Refresh with default data
}
