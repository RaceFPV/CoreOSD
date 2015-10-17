#include "Arduino.h"
#include "Config.h"
#include "Max7456.h"
#include "CoreOSD.h"
#include "Screen.h"

//for calculating amperage
char *ItoaPadded(int16_t val, char *str, uint8_t bytes, uint8_t decimalpos)  {
  uint8_t neg = 0;
  if(val < 0) {
    neg = 1;
    val = -val;
  }

  str[bytes] = 0;
  for(;;) {
    if(bytes == decimalpos) {
      str[--bytes] = '.';
      decimalpos = 0;
    }
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
    if(bytes == 0 || (decimalpos == 0 && val == 0))
      break;
  }

  if(neg && bytes > 0)
    str[--bytes] = '-';

  while(bytes != 0)
    str[--bytes] = ' ';
  return str;
}

// Take time in Seconds and format it as 'MM:SS'
// Alternately Take time in Minutes and format it as 'HH:MM'
// If hhmmss is 1, display as HH:MM:SS
char *formatTime(uint16_t val, char *str, uint8_t hhmmss) {
  int8_t bytes = 5;
  if(hhmmss)
    bytes = 8;
  str[bytes] = 0;
  do {
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
    str[--bytes] = '0' + (val % 6);
    val = val / 6;
    str[--bytes] = ':';
  } while(hhmmss-- != 0);
  do {
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
  } while(val != 0 && bytes != 0);

  while(bytes != 0)
     str[--bytes] = ' ';

  return str;
}

uint8_t FindNull(void)
{
  uint8_t xx;
  for(xx=0;screenBuffer[xx]!=0;xx++)
    ;
  return xx;
}

 
void displayVoltage(void)
{
      ItoaPadded(voltage, screenBuffer, 4, 3);
      MAX7456_WriteString(screenBuffer,445);
}  

void displayCallsign(void)
{
    MAX7456_WriteString(CALLSIGN,421);
}

void displayTime(void)
{
// On Time
      formatTime(onTime, screenBuffer, 0);
      MAX7456_WriteString(screenBuffer,439);
}

void displayAmperage(void)
{
  // Real Ampere is ampere / 10
    ItoaPadded(amperage, screenBuffer, 4, 3);     // 99.9 ampere max!
    screenBuffer[5] = 0;
    MAX7456_WriteString(screenBuffer,10);
}

void displayCrosshair(void)
{
    MAX7456_WriteString("-+-",224);
}

void displaypMeterSum(void)
{
  
    screenBuffer[1]=0;
    itoa(amperagesum,screenBuffer,10);
		MAX7456_WriteString(screenBuffer,50);
}
