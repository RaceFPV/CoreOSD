#include "Arduino.h"
#include "Config.h"
#include "Max7456.h"
#include "symbols.h"
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
      str[--bytes] = DECIMAL;
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
      MAX7456_WriteString(screenBuffer,443);
      /*
    if(Settings[L_MAINBATLEVEVOLUTIONDSPL]){
      // For battery evolution display
      int16_t BATTEV1 =Settings[S_BATCELLS] * 35;
      int16_t BATTEV2 =Settings[S_BATCELLS] * 36;
      int16_t BATTEV3 =Settings[S_BATCELLS] * 37;
      int16_t BATTEV4 =Settings[S_BATCELLS] * 38;
      int16_t BATTEV5 =Settings[S_BATCELLS] * 40;
      int16_t BATTEV6 = Settings[S_BATCELLS] * 41;
  
      if (voltage < BATTEV1) screenBuffer[0]=SYM_BATT_EMPTY;
      else if (voltage < BATTEV2) screenBuffer[0]=SYM_BATT_1;
      else if (voltage < BATTEV3) screenBuffer[0]=SYM_BATT_2;
      else if (voltage < BATTEV4) screenBuffer[0]=SYM_BATT_3;
      else if (voltage < BATTEV5) screenBuffer[0]=SYM_BATT_4;
      else if (voltage < BATTEV6) screenBuffer[0]=SYM_BATT_5;
      else screenBuffer[0]=SYM_BATT_FULL;              // Max charge icon
      }*/
}  

void displayCallsign(void)
{
    MAX7456_WriteString(CALLSIGN,420);
}

void displayTime(void)
{
// Fly Time
  /*if(!(MW_STATUS.sensorActive)){
  if(Settings[L_FLYTIMEPOSITIONDSPL]){
      screenBuffer[0] = SYM_FLY;
      formatTime(flyTime, screenBuffer+1, 0);
      MAX7456_WriteString(screenBuffer,((Settings[L_FLYTIMEPOSITIONROW]-1)*30) + Settings[L_FLYTIMEPOSITIONCOL]);
  }
 }*/  
 
// On Time
      formatTime(onTime, screenBuffer, 0);
      MAX7456_WriteString(screenBuffer,435);
      //MAX7456_WriteString(screenBuffer,((Settings[L_ONTIMEPOSITIONROW]-1)*30) + Settings[L_ONTIMEPOSITIONCOL]);
}

void displayAmperage(void)
{
  // Real Ampere is ampere / 10
  if(Settings[L_AMPERAGEPOSITIONDSPL]){
    ItoaPadded(amperage, screenBuffer, 4, 3);     // 99.9 ampere max!
    screenBuffer[5] = 0;
    MAX7456_WriteString(screenBuffer,((Settings[L_AMPERAGEPOSITIONROW]-1)*30) + Settings[L_AMPERAGEPOSITIONCOL]);
  }
}

void displaypMeterSum(void)
{

  if(Settings[L_PMETERSUMPOSITIONDSPL]){  
    screenBuffer[1]=0;
    itoa(amperagesum,screenBuffer,10);
		MAX7456_WriteString(screenBuffer,((Settings[L_PMETERSUMPOSITIONROW]-1)*30) + Settings[L_PMETERSUMPOSITIONCOL]);
 }
}
