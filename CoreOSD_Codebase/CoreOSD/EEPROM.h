#ifndef EEPROM_H_
#define EEPROM_H_

#include <inttypes.h>

class EEPROMClass
{
  public:
    uint8_t read(int);
    void write(int, uint8_t);
};

extern EEPROMClass EEPROM;


void checkEEPROM(void);
void writeEEPROM(void);
void readEEPROM(void);
void WriteScreenLayoutDefault(void);

#endif /* EEPROM_H_ */

