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

void displayHorizon(int16_t rollAngle, int16_t pitchAngle)
{
	uint8_t X;
	int16_t Y;
	uint16_t pos;
	
	if(Settings[L_HORIZONPOSITIONDSPL]){
		uint16_t position = ((Settings[L_HORIZONPOSITIONROW]-1)*30) + Settings[L_HORIZONPOSITIONCOL];
		
		for(X=0; X<=1; X++) {
			Y = (rollAngle * (2-X)) / 64;
			Y -= pitchAngle / 8;
			Y += -5;
			if(Y >= 10 && Y <= 50) {
				pos = position + LINE+LINE*(Y/9) + 4 - 2*LINE + X;
				screen[pos] = SYM_AH_BAR_0+(Y%9);
				if(Y>=9 && (Y%9) == 0)
				screen[pos-LINE] = SYM_AH_BAR_9;
			}
		}

		for(X=0; X<=1; X++) {
			Y = (rollAngle * (-1-X)) / 64;
			Y -= pitchAngle / 8;
			Y += -5;
			if(Y >= 10 && Y <= 50) {
				pos = position + LINE+LINE*(Y/9) + 7 - 2*LINE + X;
				screen[pos] = SYM_AH_BAR_0+(Y%9);
				if(Y>=9 && (Y%9) == 0)
				screen[pos-LINE] = SYM_AH_BAR_9;
			}
		}

		for(X=0; X<=2; X++) {
			Y = (rollAngle * (4-X)) / 64;
			Y -= pitchAngle / 8;
			Y += 41;
			if(Y >= 15 && Y <= 60) {
				pos = position + LINE*(Y/9) + 3 - 2*LINE + X;
				screen[pos] = SYM_AH_BAR_0+(Y%9);
				if(Y>=9 && (Y%9) == 0)
				screen[pos-LINE] = SYM_AH_BAR_9;
			}
		}

		for(X=0; X<=2; X++) {
			Y = (rollAngle * (0-X)) / 64;
			Y -= pitchAngle / 8;
			Y += 41;
			if(Y >= 15 && Y <= 60) {
				pos = position + LINE*(Y/9) + 7 - 2*LINE + X;
				screen[pos] = SYM_AH_BAR_0+(Y%9);
				if(Y>=9 && (Y%9) == 0)
				screen[pos-LINE] = SYM_AH_BAR_9;
			}
		}
		
		for(X=0; X<=1; X++) {
			Y = (rollAngle * (2-X)) / 64;
			Y -= pitchAngle / 8;
			Y += 78;
			if(Y >= 17 && Y <= 60) {
				pos = position + LINE*(Y/9) + 4 - 2*LINE + X;
				screen[pos] = SYM_AH_BAR_0+(Y%9);
				if(Y>=9 && (Y%9) == 0)
				screen[pos-LINE] = SYM_AH_BAR_9;
			}
		}

		for(X=0; X<=1; X++) {
			Y = (rollAngle * (-1-X)) / 64;
			Y -= pitchAngle / 8;
			Y += 78;
			if(Y >= 17 && Y <= 60) {
				pos = position + LINE*(Y/9) + 7 - 2*LINE + X;
				screen[pos] = SYM_AH_BAR_0+(Y%9);
				if(Y>=9 && (Y%9) == 0)
				screen[pos-LINE] = SYM_AH_BAR_9;
			}
		}

		if(Settings[L_HORIZONCENTERREFDSPL]){
			//Draw center screen
			screen[position+2*LINE+6] =   SYM_AH_CENTER;
			screen[position+2*LINE+1] =   SYM_AH_LEFT;
			screen[position+2*LINE+11] =  SYM_AH_RIGHT;
		}
		if(Settings[L_HORIZONSIDEREFDSPL]){
			// Draw AH sides
			screen[position+0*LINE +11] = SYM_AH_UP_15DG_L;
			screen[position+1*LINE +11] = SYM_AH_UP_5_10DG_L;
			screen[position+0*LINE +1] = SYM_AH_UP_15DG_R;
			screen[position+1*LINE +1] = SYM_AH_UP_5_10DG_R;
			screen[position+4*LINE +11] = SYM_AH_DOWN_15DG_L;
			screen[position+3*LINE +11] = SYM_AH_DOWN_5_10DG_L;
			screen[position+4*LINE +1] = SYM_AH_DOWN_15DG_R;
			screen[position+3*LINE +1] = SYM_AH_DOWN_5_10DG_R;
		}
		pitchAngle=pitchAngle/10;
	}
}

 
void displayVoltage(void)
{
    if (Settings[S_MAINVOLTAGE_VBAT]){
      voltage=MW_ANALOG.VBat;
    }
      if (voltage <=(Settings[S_VOLTAGEMIN]) && !BlinkAlarm){  
      //ItoaPadded(voltage, screenBuffer, 4, 3);
      return;
   }
   if(!(MW_STATUS.sensorActive) || (voltage <=(Settings[S_VOLTAGEMIN]+2))){ 
      ItoaPadded(voltage, screenBuffer, 4, 3);
      screenBuffer[5] = 0;
      MAX7456_WriteString(screenBuffer,443);
      //MAX7456_WriteString(screenBuffer,((Settings[L_VOLTAGEPOSITIONROW]-1)*30) + Settings[L_VOLTAGEPOSITIONCOL]);
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
      screenBuffer[1]=0;
      
      MAX7456_WriteString(screenBuffer,443);
      //MAX7456_WriteString(screenBuffer,((Settings[L_VOLTAGEPOSITIONROW]-1)*30) + Settings[L_VOLTAGEPOSITIONCOL]-1);
   }
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
 if(!(MW_STATUS.sensorActive)){
  if(Settings[L_AMPERAGEPOSITIONDSPL]){
    ItoaPadded(amperage, screenBuffer, 4, 3);     // 99.9 ampere max!
    screenBuffer[5] = 0;
    MAX7456_WriteString(screenBuffer,((Settings[L_AMPERAGEPOSITIONROW]-1)*30) + Settings[L_AMPERAGEPOSITIONCOL]);
  }
 }
}

void displaypMeterSum(void)
{

  if(Settings[L_PMETERSUMPOSITIONDSPL]){  
    screenBuffer[1]=0;
    itoa(amperagesum,screenBuffer,10);
	 if(!(MW_STATUS.sensorActive))
		MAX7456_WriteString(screenBuffer,((Settings[L_PMETERSUMPOSITIONROW]-1)*30) + Settings[L_PMETERSUMPOSITIONCOL]);
 }
}

void displayRSSI(void)
{
  if(Settings[L_RSSIPOSITIONDSPL]){
    if (rssi <=(Settings[S_RSSI_ALARM]) && !BlinkAlarm){
      return;
      }
    itoa(rssi,screenBuffer,10);
    uint8_t xx = FindNull();
    screenBuffer[xx] = 0;
  }
}
