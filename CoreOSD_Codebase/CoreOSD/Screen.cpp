#include "Arduino.h"
#include "Config.h"
#include "Max7456.h"
#include "symbols.h"
#include "KV_Team_OSD.h"
#include "Screen.h"


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

char *FormatGPSCoord(int32_t val, char *str, uint8_t p, char pos, char neg) {
  if(val < 0) {
    pos = neg;
    val = -val;
  }

//  uint8_t bytes = p+8;
  uint8_t bytes = p+6;  
  val = val / 100;  //  5 decimals instead of 6 after dot
  
  str[bytes] = 0;
  str[--bytes] = pos;
  for(;;) {
    if(bytes == p) {
      str[--bytes] = DECIMAL;
      continue;
    }
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
    if(bytes < 3 && val == 0)
       break;
   }

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


/*void displayTemperature(void)        // WILL WORK ONLY WITH Rushduino V1.2      //Lets see about complains on this function (during two major releases)  before remove...
{
 if(!(MW_STATUS.sensorActive&mode_osd_switch)){  // mode_osd_switch=0 --> Display, =1 --> Hide
  if(Settings[L_TEMPERATUREPOSDSPL]){
    int16_t xxx;
    if (Settings[S_UNITSYSTEM])
      xxx = temperature*1.8+32;       //Fahrenheit conversion for imperial system.
    else
      xxx = temperature;
  
    if(xxx > temperMAX)
      temperMAX = xxx;
  
    itoa(xxx,screenBuffer,10);
    uint8_t xx = FindNull();   // find the NULL
    screenBuffer[xx++]=temperatureUnitAdd[Settings[S_UNITSYSTEM]];
    screenBuffer[xx]=0;  // Restore the NULL
    MAX7456_WriteString(screenBuffer,((Settings[L_TEMPERATUREPOSROW]-1)*30) + Settings[L_TEMPERATUREPOSCOL]);
  }
 }
}
*/

void displayMode(void)
{
  if(!(MW_STATUS.sensorActive&mode_osd_switch)){
    if(Settings[L_MODEPOSITIONDSPL]){
     if(MW_STATUS.sensorActive&mode_horizon)
    {
      screenBuffer[0]=SYM_HORIZON;
      screenBuffer[1]=SYM_HORIZON_1;
      screenBuffer[2]=SYM_HORIZON_2;
    }
    
    else
    
    if(MW_STATUS.sensorActive&mode_stable)
    {
      screenBuffer[0]=' ';
      screenBuffer[1]=SYM_ANGLE;
      screenBuffer[2]=SYM_ANGLE_1;
    }
    else
    {
      screenBuffer[0]=' ';
      screenBuffer[1]=SYM_ACRO;
      screenBuffer[2]=SYM_ACRO_1;
    }
      screenBuffer[3] =0;
    MAX7456_WriteString(screenBuffer,((Settings[L_MODEPOSITIONROW]-1)*30) + Settings[L_MODEPOSITIONCOL]);
    }
  }  
}

void displayArmed(void)
{
  if(!(MW_STATUS.sensorActive&mode_osd_switch)){
    if(Settings[L_MOTORARMEDPOSITIONDSPL]){
      if(!armed){
        screenBuffer[0]=SYM_MOTOR_DIS;
        screenBuffer[1]=SYM_MOTOR_AR;
        screenBuffer[2]=SYM_MOTOR_ME;
        screenBuffer[3]=SYM_MOTOR_D;
        screenBuffer[4]=0;
        MAX7456_WriteString(screenBuffer,((Settings[L_MOTORARMEDPOSITIONROW]-1)*30) + Settings[L_MOTORARMEDPOSITIONCOL]);}
   else if (armed){
        screenBuffer[0]=SYM_MOTOR_AR;
        screenBuffer[1]=SYM_MOTOR_ME;
        screenBuffer[2]=SYM_MOTOR_D;
        screenBuffer[3]=0;
        if(Blink10hz && flyTime < 8)
        MAX7456_WriteString(screenBuffer,((Settings[L_MOTORARMEDPOSITIONROW]-1)*30) + Settings[L_MOTORARMEDPOSITIONCOL]);}
    }
  }  
}

void displayCallsign(void)
{
  if(!(MW_STATUS.sensorActive&mode_osd_switch)){
    if(Settings[L_CALLSIGNPOSITIONDSPL] || Settings[S_CALLSIGN]){
        for(int16_t X=0; X<10; X++) {
            screenBuffer[X] = char(Settings[S_CS0 + X]);
       }   
        screenBuffer[10] = 0;
        MAX7456_WriteString(screenBuffer,((Settings[L_CALLSIGNPOSITIONROW]-1)*30) + Settings[L_CALLSIGNPOSITIONCOL]);
    }
  }
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
		if(pitchAngle>= Settings[S_PITCH_WARNING]  || pitchAngle <=- Settings[S_PITCH_WARNING]) {    // Warn alarm set to 30Â°
			if(BlinkAlarm){    // Flashing synchro with other alarms
				screen[position+4] = SYM_WARN;     // Position: center/top of AH rectangle
				screen[position+5] = SYM_WARN_1;
				screen[position+6] = SYM_WARN_2;
				itoa(pitchAngle, screenBuffer, 10);                // Display angle next to warning
				uint8_t xx = FindNull();
				screenBuffer[xx++] = SYM_DEGREES;
				screenBuffer[xx] = 0;
				MAX7456_WriteString(screenBuffer, position+7);     // Position: center/top of AH rectangle
			}
		}
	}
}

 
void displayVoltage(void)
{
  if(Settings[L_VOLTAGEPOSITIONDSPL]){
    if (Settings[S_MAINVOLTAGE_VBAT]){
      voltage=MW_ANALOG.VBat;
    }
      if (voltage <=(Settings[S_VOLTAGEMIN]) && !BlinkAlarm){  
      ItoaPadded(voltage, screenBuffer, 4, 3);
      return;
   }
   if(!(MW_STATUS.sensorActive&mode_osd_switch) || (voltage <=(Settings[S_VOLTAGEMIN]+2))){ 
      ItoaPadded(voltage, screenBuffer, 4, 3);
      screenBuffer[4] = SYM_VOLTS;
      screenBuffer[5] = 0;
      MAX7456_WriteString(screenBuffer,((Settings[L_VOLTAGEPOSITIONROW]-1)*30) + Settings[L_VOLTAGEPOSITIONCOL]);
      
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
      }
      else {
        screenBuffer[0]=SYM_MAIN_BATT;
      }
      screenBuffer[1]=0;
      MAX7456_WriteString(screenBuffer,((Settings[L_VOLTAGEPOSITIONROW]-1)*30) + Settings[L_VOLTAGEPOSITIONCOL]-1);
   }
  }
} 

void displayVidVoltage(void)
{
 if(!(MW_STATUS.sensorActive&mode_osd_switch)){
    if(Settings[L_VIDVOLTAGEPOSITIONDSPL]){    
      if (Settings[S_VIDVOLTAGE_VBAT]){
      vidvoltage=MW_ANALOG.VBat;
      }
      if (vidvoltage <=(Settings[S_VOLTAGEMIN]) && !BlinkAlarm){
      ItoaPadded(vidvoltage, screenBuffer, 4, 3);
      return;
      }
      ItoaPadded(vidvoltage, screenBuffer, 4, 3);
      screenBuffer[4]=SYM_VOLTS;
      screenBuffer[5]=0;
      MAX7456_WriteString(screenBuffer,((Settings[L_VIDVOLTAGEPOSITIONROW]-1)*30) + Settings[L_VIDVOLTAGEPOSITIONCOL]);
      screenBuffer[0]=SYM_VID_BAT;
      screenBuffer[1]=0;
      MAX7456_WriteString(screenBuffer,((Settings[L_VIDVOLTAGEPOSITIONROW]-1)*30) + Settings[L_VIDVOLTAGEPOSITIONCOL]-1);
    }
  }
} 


void displayCurrentThrottle(void)
{
 if(!(MW_STATUS.sensorActive&mode_osd_switch)){
  if(Settings[L_CURRENTTHROTTLEPOSITIONDSPL]){
    if (MwRcData[THROTTLESTICK] > HighT) HighT = MwRcData[THROTTLESTICK] -5;
    if (MwRcData[THROTTLESTICK] < LowT) LowT = MwRcData[THROTTLESTICK];      // Calibrate high and low throttle settings  --defaults set in GlobalVariables.h 1100-1900
  int16_t CurThrottle = map(MwRcData[THROTTLESTICK],LowT,HighT,0,100);
    screenBuffer[2]=0;
          if(CurThrottle >95) screenBuffer[0] = SYM_THR_POINTER_TOP;
           else if(CurThrottle > 90) screenBuffer[0] = SYM_THR_POINTER_4;
              else if(CurThrottle >85) screenBuffer[0] = SYM_THR_POINTER_3;
                else if(CurThrottle > 80) screenBuffer[0] = SYM_THR_POINTER_2;
                  else if(CurThrottle > 75)  screenBuffer[0] = SYM_THR_POINTER_1;
                    else if(CurThrottle > 70) screenBuffer[0] = SYM_THR_POINTER_BOT;
                      else if(CurThrottle > 65) screenBuffer[0] = SYM_THR_POINTER_DIV_LOW;            // High throttle % Row 1
                        else screenBuffer[0] = ' ';
    screenBuffer[1]=SYM_THR_SCALE_MAX;
    MAX7456_WriteString(screenBuffer,((Settings[L_CURRENTTHROTTLEPOSITIONROW]-1)*30) + Settings[L_CURRENTTHROTTLEPOSITIONCOL]);
    if (CurThrottle > 70) screenBuffer[0] = ' ';
           else if(CurThrottle >65) screenBuffer[0] = SYM_THR_POINTER_DIV_HIGH;
             else if(CurThrottle > 60) screenBuffer[0] = SYM_THR_POINTER_TOP;
               else if(CurThrottle > 55) screenBuffer[0] = SYM_THR_POINTER_4;
                 else if(CurThrottle > 50) screenBuffer[0] = SYM_THR_POINTER_3;
                   else if(CurThrottle > 45) screenBuffer[0] = SYM_THR_POINTER_2;
                     else if(CurThrottle > 40)  screenBuffer[0] = SYM_THR_POINTER_1;
                       else if(CurThrottle > 35) screenBuffer[0] = SYM_THR_POINTER_BOT;
                         else if(CurThrottle > 30) screenBuffer[0] = SYM_THR_POINTER_DIV_LOW;          // Mid throttle % Row 2
    screenBuffer[1]=SYM_THR_SCALE_MED;
    MAX7456_WriteString(screenBuffer,((Settings[L_CURRENTTHROTTLEPOSITIONROW]-1)*30) + Settings[L_CURRENTTHROTTLEPOSITIONCOL]+LINE);   
    
    if (CurThrottle > 35) screenBuffer[0] = ' ';
           else if(CurThrottle >30) screenBuffer[0] = SYM_THR_POINTER_DIV_HIGH;
             else if(CurThrottle > 25) screenBuffer[0] = SYM_THR_POINTER_TOP;
              else if(CurThrottle > 20) screenBuffer[0] = SYM_THR_POINTER_4;
                else if(CurThrottle > 15) screenBuffer[0] = SYM_THR_POINTER_3;
                  else if(CurThrottle > 10)  screenBuffer[0] = SYM_THR_POINTER_2;
                    else if(CurThrottle > 5) screenBuffer[0] = SYM_THR_POINTER_1;          // Minimum throttle % Row 3
                      else screenBuffer[0] = SYM_THR_POINTER_BOT;
    screenBuffer[1]=SYM_THR_SCALE_LOW;    
    MAX7456_WriteString(screenBuffer,((Settings[L_CURRENTTHROTTLEPOSITIONROW]-1)*30) + Settings[L_CURRENTTHROTTLEPOSITIONCOL]+LINE+LINE);

     if(armed&&(CurThrottle <=15 && !BlinkAlarm)){                                 //Stall warning at 15% throttle
      screenBuffer[0]=SYM_THR_STALL;
      screenBuffer[1]=SYM_THR_STALL1;                                        
      screenBuffer[2]=0;
      MAX7456_WriteString(screenBuffer,((Settings[L_CURRENTTHROTTLEPOSITIONROW]-1)*30) + Settings[L_CURRENTTHROTTLEPOSITIONCOL]+LINE+LINE+LINE);
      return;
      }  
    }
  }
 }

void displayTime(void)
{
// Fly Time
  if(!(MW_STATUS.sensorActive&mode_osd_switch)){
  if(Settings[L_FLYTIMEPOSITIONDSPL]){
      screenBuffer[0] = SYM_FLY;
      formatTime(flyTime, screenBuffer+1, 0);
      MAX7456_WriteString(screenBuffer,((Settings[L_FLYTIMEPOSITIONROW]-1)*30) + Settings[L_FLYTIMEPOSITIONCOL]);
  }
 }  
 
// On Time
 if(!(MW_STATUS.sensorActive&mode_osd_switch)){
  if(Settings[L_ONTIMEPOSITIONDSPL]){
    if (armed) return ;
      screenBuffer[0] = SYM_ON;
      formatTime(onTime, screenBuffer+1, 0);
      MAX7456_WriteString(screenBuffer,((Settings[L_ONTIMEPOSITIONROW]-1)*30) + Settings[L_ONTIMEPOSITIONCOL]);
  }
 }
}

void displayAmperage(void)
{
  // Real Ampere is ampere / 10
 if(!(MW_STATUS.sensorActive&mode_osd_switch)){
  if(Settings[L_AMPERAGEPOSITIONDSPL]){
    ItoaPadded(amperage, screenBuffer, 4, 3);     // 99.9 ampere max!
    screenBuffer[4] = SYM_AMPS;
    screenBuffer[5] = 0;
    MAX7456_WriteString(screenBuffer,((Settings[L_AMPERAGEPOSITIONROW]-1)*30) + Settings[L_AMPERAGEPOSITIONCOL]);
  }
 }
}

void displaypMeterSum(void)
{

  if(Settings[L_PMETERSUMPOSITIONDSPL]){    
    screenBuffer[0]=SYM_CURRENT;
    screenBuffer[1]=0;
    itoa(amperagesum,screenBuffer+1,10);
	 if(!(MW_STATUS.sensorActive&mode_osd_switch))
		MAX7456_WriteString(screenBuffer,((Settings[L_PMETERSUMPOSITIONROW]-1)*30) + Settings[L_PMETERSUMPOSITIONCOL]);
 }
}

void displayRSSI(void)
{
  if(Settings[L_RSSIPOSITIONDSPL]){
    if (rssi <=(Settings[S_RSSI_ALARM]) && !BlinkAlarm){
      screenBuffer[0] = SYM_RSSI;
      return;
      }
    screenBuffer[0] = SYM_RSSI;
    // Calcul et affichage du Rssi
    itoa(rssi,screenBuffer+1,10);
    uint8_t xx = FindNull();
    screenBuffer[xx++] = '%';
    screenBuffer[xx] = 0;
	if(!(MW_STATUS.sensorActive&mode_osd_switch) || rssi<=(Settings[S_RSSI_ALARM]+5))
		MAX7456_WriteString(screenBuffer,((Settings[L_RSSIPOSITIONROW]-1)*30) + Settings[L_RSSIPOSITIONCOL]);
  
  }
}

void displayHeading(void)
{

  if(Settings[L_MW_HEADINGPOSITIONDSPL]){
      int16_t heading = MW_ATT.Heading;
      if (Settings[S_HEADING360]) {
        if(heading < 0)
          heading += 360;
          
          
        ItoaPadded(heading,screenBuffer,3,0);
        screenBuffer[3]=SYM_DEGREES;
        screenBuffer[4]=0;
      }
      else {
        if(heading < 0)
          heading += -0;
        ItoaPadded(heading,screenBuffer,4,0);
        screenBuffer[4]=SYM_DEGREES;
        screenBuffer[5]=0;
      }
	if(!(MW_STATUS.sensorActive&mode_osd_switch))
	MAX7456_WriteString(screenBuffer,((Settings[L_MW_HEADINGPOSITIONROW]-1)*30) + Settings[L_MW_HEADINGPOSITIONCOL]);
 }
}

void displayHeadingGraph(void)
{
 if(!(MW_STATUS.sensorActive&mode_osd_switch)  || (!GPS.fix)){
  if(Settings[L_MW_HEADINGGRAPHPOSDSPL]){
    int16_t xx;
    xx = MW_ATT.Heading * 2;
    xx = xx + 440 -36;
    xx = xx / 90;
    uint16_t pos = ((Settings[L_MW_HEADINGGRAPHPOSROW]-1)*30) + Settings[L_MW_HEADINGGRAPHPOSCOL];
    memcpy_P(screen+pos, headGraph+xx, 5);
    screenBuffer[0] = SYM_HEADING_POINTER;
    screenBuffer[1] = 0;
    MAX7456_WriteString(screenBuffer,((Settings[L_MW_HEADINGGRAPHPOSROW]-1)*30) + Settings[L_MW_HEADINGGRAPHPOSCOL]-LINE+2);
  }
 }
}

void displayIntro(char position)
{
  MAX7456_WriteString_P(message0, position);
  
  MAX7456_WriteString_P(configMsg76, position+30);  // "VIDEO SYSTEM"
  if (Settings[S_VIDEOSIGNALTYPE])
    MAX7456_WriteString_P(configMsgPAL, position+43);  // PAL
  else
    MAX7456_WriteString_P(configMsgNTSC, position+43);  // NTSC
    
  MAX7456_WriteString_P(MultiWiiLogoL1Add, position+120);
  MAX7456_WriteString_P(MultiWiiLogoL2Add, position+120+LINE);
  MAX7456_WriteString_P(MultiWiiLogoL3Add, position+120+LINE+LINE);

  MAX7456_WriteString_P(message5, position+120+LINE+LINE+LINE);
  MAX7456_WriteString(itoa(MW_STATUS.version,screenBuffer,10),position+131+LINE+LINE+LINE);

  MAX7456_WriteString_P(message6, position+120+LINE+LINE+LINE+LINE+LINE);
  MAX7456_WriteString_P(message7, position+125+LINE+LINE+LINE+LINE+LINE+LINE);
  MAX7456_WriteString_P(message8, position+125+LINE+LINE+LINE+LINE+LINE+LINE+LINE);
  
  MAX7456_WriteString_P(message9, position+120+LINE+LINE+LINE+LINE+LINE+LINE+LINE+LINE);
  if(Settings[L_CALLSIGNPOSITIONDSPL]){
      for(int16_t X=0; X<10; X++) {
          screenBuffer[X] = char(Settings[S_CS0 + X]);
      }
   if (!BlinkAlarm)
   MAX7456_WriteString(screenBuffer, position+130+LINE+LINE+LINE+LINE+LINE+LINE+LINE+LINE);;     // Call Sign on the beginning of the transmission (blink at sel freq)  
   }
}

void displayFontScreen(void) {
  MAX7456_WriteString_P(PSTR("UPLOADING FONT"), 35);                  // Do not remove
  //MAX7456_WriteString(itoa(nextCharToRequest, screenBuffer, 10), 51);

  for(uint16_t i = 0; i < 256; i++)
    screen[90+i] = i;
}

void displayGPSPosition(void)
{
 if(!(MW_STATUS.sensorActive&mode_osd_switch) || rssi<=(Settings[S_RSSI_ALARM]-10)){ 
  if(Settings[S_COORDINATES]){
    if(Settings[L_MW_GPS_LATPOSITIONDSPL]){
        screenBuffer[0] = SYM_LAT;
        FormatGPSCoord(GPS.latitude,screenBuffer+1,4,'N','S');
        MAX7456_WriteString(screenBuffer,((Settings[L_MW_GPS_LATPOSITIONROW]-1)*30) + Settings[L_MW_GPS_LATPOSITIONCOL]);
       }
     if(Settings[L_MW_GPS_LONPOSITIONDSPL]){
         screenBuffer[0] = SYM_LON;
         FormatGPSCoord(GPS.longitude,screenBuffer+1,4,'E','W');
         MAX7456_WriteString(screenBuffer,((Settings[L_MW_GPS_LONPOSITIONROW]-1)*30) + Settings[L_MW_GPS_LONPOSITIONCOL]);
      }
    }
  }  
}


/*void displayGPS_altitude (void)
{
  if(!GPS.fix)
      return;
      
   if(!(MW_STATUS.sensorActive&mode_osd_switch)){
     //if(Settings[L_MW_GPS_ALTPOSITIONDSPL]){
        screenBuffer[0] = SYM_GPS;
        screenBuffer[1] = SYM_ALT;
        uint16_t xx;
          if(Settings[S_UNITSYSTEM])
           xx = GPS_altitude * 3.2808; // Mt to Feet
      else
           xx = GPS_altitude;          // Mt
        itoa(xx,screenBuffer+2,10);
        //MAX7456_WriteString(screenBuffer,((Settings[L_MW_GPS_ALTPOSITIONROW]-1)*30) + Settings[L_MW_GPS_ALTPOSITIONCOL]);
     }
   }
 } */
   
void displayNumberOfSat(void)
{
  if(Settings[L_GPS_NUMSATPOSITIONDSPL]){
    screenBuffer[0] = SYM_SAT;
    screenBuffer[1] = SYM_SAT_1;
    itoa(GPS.numSat,screenBuffer+2,10);
	if(!(MW_STATUS.sensorActive&mode_osd_switch))
    MAX7456_WriteString(screenBuffer,((Settings[L_GPS_NUMSATPOSITIONROW]-1)*30) + Settings[L_GPS_NUMSATPOSITIONCOL]);
 }
}

void displayGPS_speed(void)
{
//  if(Settings[L_SPEEDPOSITIONDSPL]){

  if(!armed) GPS.speed=0;

  int16_t xx;
  if(!Settings[S_UNITSYSTEM])
    xx = GPS.speed * 0.036;           // From MWii cm/sec to Km/h
  else
    xx = GPS.speed * 0.02236932;      // (0.036*0.62137)  From MWii cm/sec to mph 
  if(xx > speedMAX)
    speedMAX = xx;
    
  if(Settings[L_SPEEDPOSITIONDSPL]){      
    screenBuffer[0]=SYM_SPEED;
    screenBuffer[1]=SYM_SPEED_1;
    itoa(xx,screenBuffer+2,10);
	if(!(MW_STATUS.sensorActive&mode_osd_switch))
		MAX7456_WriteString(screenBuffer,((Settings[L_SPEEDPOSITIONROW]-1)*30) + Settings[L_SPEEDPOSITIONCOL]);

 }
}

void displayAltitude(void)
{
    int16_t altitude;
//    if(Settings[L_MW_ALTITUDEPOSITIONDSPL]){
    if(Settings[S_UNITSYSTEM])
      altitude = MW_ALT.Altitude*0.032808;    // cm to feet
    else
      altitude = MW_ALT.Altitude/100;         // cm to mt
    if(armed && allSec>5 && altitude > altitudeMAX)
      altitudeMAX = altitude;
    if(Settings[L_MW_ALTITUDEPOSITIONDSPL]){
      if(!(MW_STATUS.sensorActive&mode_osd_switch) || altitude >= (Settings[S_VOLUME_ALT_MAX]*10) || flyTime > 30 && altitude < (Settings[S_VOLUME_ALT_MIN])){
      if(altitude >= (Settings[S_VOLUME_ALT_MAX]*10) && !BlinkAlarm || flyTime > 60 && altitude < (Settings[S_VOLUME_ALT_MIN]) && !BlinkAlarm)
      return;
      screenBuffer[0]=SYM_ALT;
      itoa(altitude,screenBuffer+1,10);
      MAX7456_WriteString(screenBuffer,((Settings[L_MW_ALTITUDEPOSITIONROW]-1)*30) + Settings[L_MW_ALTITUDEPOSITIONCOL]);
      }
  }
}

void displayClimbRate(void)
{
 if(!(MW_STATUS.sensorActive&mode_osd_switch)) {
  if(Settings[L_CLIMBRATEPOSITIONDSPL]){
    

    if(MW_ALT.Vario > 100)       screenBuffer[0] = SYM_POS_CLIMB3;
    else if(MW_ALT.Vario > 70)  screenBuffer[0] = SYM_POS_CLIMB2;
    else if(MW_ALT.Vario > 50)  screenBuffer[0] = SYM_POS_CLIMB1;
    else if(MW_ALT.Vario > 30)  screenBuffer[0] = SYM_POS_CLIMB;
    else if(MW_ALT.Vario < -100) screenBuffer[0] = SYM_NEG_CLIMB3;
    else if(MW_ALT.Vario < -70) screenBuffer[0] = SYM_NEG_CLIMB2;
    else if(MW_ALT.Vario < -50) screenBuffer[0] = SYM_NEG_CLIMB1;
    else if(MW_ALT.Vario < -30) screenBuffer[0] = SYM_NEG_CLIMB;
    else                   screenBuffer[0] = SYM_ZERO_CLIMB;
    screenBuffer [1] =0;
    
    
    
  MAX7456_WriteString(screenBuffer,((Settings[L_CLIMBRATEPOSITIONROW]-1)*30) + Settings[L_CLIMBRATEPOSITIONCOL]+3);
     screenBuffer[0] = SYM_VSPEED;    
     int16_t vario;
    if(Settings[S_UNITSYSTEM])
      vario = MW_ALT.Vario * 0.032808;       // cm/sec ----> ft/sec
    else
      vario = MW_ALT.Vario / 100;            // cm/sec ----> mt/sec
      itoa(vario, screenBuffer+1, 10);
    if(MW_ALT.Vario <= -Settings[S_CLIMB_RATE_ALARM]*100 && !BlinkAlarm)
      return;
    if(MW_ALT.Vario >= +Settings[S_CLIMB_RATE_ALARM]*100 && !BlinkAlarm)  
      return;
  
    MAX7456_WriteString(screenBuffer,((Settings[L_CLIMBRATEPOSITIONROW]-1)*30) + Settings[L_CLIMBRATEPOSITIONCOL]);
   }
 }
}

void displayDebug1(int16_t x) //SJa
{
	if(!(MW_STATUS.sensorActive&mode_osd_switch)){

		itoa(x,screenBuffer,10);
		MAX7456_WriteString(screenBuffer,242);
	}
}

void displayDistanceToHome(void)
{
//  if(Settings[L_GPS_DISTANCETOHOMEPOSDSPL]){

  if (GPS.distanceToHome >= (Settings[S_VOLUME_DIST_MAX]*100) && !BlinkAlarm) 
    return;

  int16_t dist;
  if(Settings[S_UNITSYSTEM])
    dist = GPS.distanceToHome * 3.2808;           // mt to feet
  else
    dist = GPS.distanceToHome;                    // mt
  if(dist > distanceMAX)
    distanceMAX = dist;  

  if(Settings[L_GPS_DISTANCETOHOMEPOSDSPL]){
    screenBuffer[0] = SYM_LOS;
    itoa(dist, screenBuffer+1, 10);
	if(!(MW_STATUS.sensorActive&mode_osd_switch) || GPS.distanceToHome >= (Settings[S_VOLUME_DIST_MAX]*100))
		MAX7456_WriteString(screenBuffer,((Settings[L_GPS_DISTANCETOHOMEPOSROW]-1)*30) + Settings[L_GPS_DISTANCETOHOMEPOSCOL]);
  }
}

void displayDirectionToHome(void)
{
 if(!(MW_STATUS.sensorActive&mode_osd_switch) || (rssi<=(Settings[S_RSSI_ALARM]) || (voltage <=(Settings[S_VOLTAGEMIN]+1) ||  GPS.distanceToHome >= (Settings[S_VOLUME_DIST_MAX]*100)))){
  if(Settings[L_GPS_DIRECTIONTOHOMEPOSDSPL]){
    if(!GPS.fix)
      return;      
    if(GPS.distanceToHome <= 2 && !BlinkAlarm)
      return;
    int16_t d = MW_ATT.Heading + 360 + 180 - GPS.directionToHome;
    d *= 4;
    d += 45;
    d = 2*(d/90)%32;
    screenBuffer[0] = SYM_ARROW_SOUTH + d;
    screenBuffer[1] = SYM_ARROW_SOUTH_1 + d;
    screenBuffer[2] = 0;
    MAX7456_WriteString(screenBuffer,((Settings[L_GPS_DIRECTIONTOHOMEPOSROW]-1)*30) + Settings[L_GPS_DIRECTIONTOHOMEPOSCOL]); 
  }  
 }
}

void displayAngleToHome(void)
{
  if(Settings[L_GPS_ANGLETOHOMEPOSDSPL]){

    if(GPS.distanceToHome <= 2 && !BlinkAlarm)
      return;
  
    ItoaPadded(GPS.directionToHome,screenBuffer,3,0);
    screenBuffer[3] = SYM_DEGREES;
    screenBuffer[4] = 0;
	if(!(MW_STATUS.sensorActive&mode_osd_switch))
		MAX7456_WriteString(screenBuffer,((Settings[L_GPS_ANGLETOHOMEPOSROW]-1)*30) + Settings[L_GPS_ANGLETOHOMEPOSCOL]);
  }
}

void displayCursor(void)
{
  int16_t cursorpos;

  if(ROW==10){
    if(COL==4) COL=3;
    if(COL==3) cursorpos=SAVEP+16-1;    // page
    if(COL==1) cursorpos=SAVEP-1;       // exit
    if(COL==2) cursorpos=SAVEP+6-1;     // save/exit
  }
  if(ROW<10){
    if(configPage==1){
      if (ROW==9) ROW=7;
      if (ROW==8) ROW=10;
      if(COL==4) COL=3;
      if(COL==1) cursorpos=(ROW+2)*30+10;
      if(COL==2) cursorpos=(ROW+2)*30+10+6;
      if(COL==3) cursorpos=(ROW+2)*30+10+6+6;
      }
    if(configPage==2){
      COL=3;
      if (ROW==7) ROW=5;
      if (ROW==6) ROW=10;
      if (ROW==9) ROW=5;
      cursorpos=(ROW+2)*30+10+6+6;
      }
    if(configPage==3){
      COL=3;
      if (ROW==9) ROW=2; // ROW=3 w/ Temperature
      if (ROW==4) ROW=10;  
      cursorpos=(ROW+2)*30+10+6+6; 
     
      }
    if(configPage==4){
      COL=3;
      if (ROW==2) ROW=3;
      if (ROW==9) ROW=4;
      if (ROW==5) ROW=10;
      cursorpos=(ROW+2)*30+10+6+6;
      }
      
    if(configPage==5)
      {  
      COL=3;
      if (ROW==9) ROW=7;
      if (ROW==8) ROW=10;
      cursorpos=(ROW+2)*30+10+6+6;
      }
      
    if(configPage==6)
      {  
      COL=3;
      if (ROW==9) ROW=3;
      if (ROW==4) ROW=10;
      cursorpos=(ROW+2)*30+10+6+6;
      }

    if(configPage==7)
      {  
      COL=3;
      if (ROW==9) ROW=4;
      if (ROW==5) ROW=10;
      if (ROW==3) cursorpos=(ROW+2)*30+10+6+6-2;  // Metric/Imperial string longer            
      else cursorpos=(ROW+2)*30+10+6+6;
      }
      
    if(configPage==8){
      if (ROW==9) ROW=7;
      if (ROW==6) ROW=4;
      if (ROW<4) ROW=4;
      if (ROW==5) ROW=7;
      if (ROW==8) ROW=10;
      if(COL==1) cursorpos=(ROW+2)*30+2;   // Item
      if(COL==2) cursorpos=(ROW+2)*30+15;  // On/Off
      if(COL==3) cursorpos=(ROW+2)*30+20;  // LINE
      if(COL==4) cursorpos=(ROW+2)*30+24;  // COL
      }
      
    if(configPage==9){
      ROW=10;
      }      
  }
  if(Blink10hz)
    screen[cursorpos] = SYM_CURSOR;
}


void displayConfigScreen(void)
{
  MAX7456_WriteString_P(configMsgEXIT, SAVEP);       // EXIT
  if(!previousarmedstatus) {
    MAX7456_WriteString_P(configMsgSAVE, SAVEP+6);   // SaveExit
    MAX7456_WriteString_P(configMsgPGS, SAVEP+16);   // <Page>
  }

  if(configPage==1)
  {
	  MAX7456_WriteString_P(configMsg10, 38);
	  MAX7456_WriteString_P(configMsg11, ROLLT);
	  MAX7456_WriteString(itoa(conf.pid[0].P8,screenBuffer,10),ROLLP);
	  MAX7456_WriteString(itoa(conf.pid[0].I8,screenBuffer,10),ROLLI);
	  MAX7456_WriteString(itoa(conf.pid[0].D8,screenBuffer,10),ROLLD);

	  MAX7456_WriteString_P(configMsg12, PITCHT);
	  MAX7456_WriteString(itoa(conf.pid[1].P8,screenBuffer,10), PITCHP);
	  MAX7456_WriteString(itoa(conf.pid[1].I8,screenBuffer,10), PITCHI);
	  MAX7456_WriteString(itoa(conf.pid[1].D8,screenBuffer,10), PITCHD);

	  MAX7456_WriteString_P(configMsg13, YAWT);
	  MAX7456_WriteString(itoa(conf.pid[2].P8,screenBuffer,10),YAWP);
	  MAX7456_WriteString(itoa(conf.pid[2].I8,screenBuffer,10),YAWI);
	  MAX7456_WriteString(itoa(conf.pid[2].D8,screenBuffer,10),YAWD);

	  MAX7456_WriteString_P(configMsg14, ALTT);
	  MAX7456_WriteString(itoa(conf.pid[3].P8,screenBuffer,10),ALTP);
	  MAX7456_WriteString(itoa(conf.pid[3].I8,screenBuffer,10),ALTI);
	  MAX7456_WriteString(itoa(conf.pid[3].D8,screenBuffer,10),ALTD);

	  MAX7456_WriteString_P(configMsg15, VELT);
	  MAX7456_WriteString(itoa(conf.pid[4].P8,screenBuffer,10),VELP);
	  MAX7456_WriteString(itoa(conf.pid[4].I8,screenBuffer,10),VELI);
	  MAX7456_WriteString(itoa(conf.pid[4].D8,screenBuffer,10),VELD);

	  MAX7456_WriteString_P(configMsg16, LEVT);
	  MAX7456_WriteString(itoa(conf.pid[7].P8,screenBuffer,10),LEVP);
	  MAX7456_WriteString(itoa(conf.pid[7].I8,screenBuffer,10),LEVI);
	  MAX7456_WriteString(itoa(conf.pid[7].D8,screenBuffer,10),LEVD);

	  MAX7456_WriteString_P(configMsg17, MAGT);
	  MAX7456_WriteString(itoa(conf.pid[8].P8,screenBuffer,10),MAGP);

	  MAX7456_WriteString("P",71);
	  MAX7456_WriteString("I",77);
	  MAX7456_WriteString("D",83);
  }

  if(configPage==2)
  {
    MAX7456_WriteString_P(configMsg20, 38);
    MAX7456_WriteString_P(configMsg21, ROLLT);
    MAX7456_WriteString(itoa(conf.rcRate8,screenBuffer,10),ROLLD);
    MAX7456_WriteString_P(configMsg22, PITCHT);
    MAX7456_WriteString(itoa(conf.rcExpo8,screenBuffer,10),PITCHD);
    MAX7456_WriteString_P(configMsg23, YAWT);
    MAX7456_WriteString(itoa(conf.rollPitchRate,screenBuffer,10),YAWD);
    MAX7456_WriteString_P(configMsg24, ALTT);
    MAX7456_WriteString(itoa(conf.yawRate,screenBuffer,10),ALTD);
    MAX7456_WriteString_P(configMsg25, VELT);
    MAX7456_WriteString(itoa(conf.dynThrPID,screenBuffer,10),VELD);
    MAX7456_WriteString_P(configMsg26, LEVT);
    MAX7456_WriteString(itoa(MW_STATUS.cycleTime,screenBuffer,10),LEVD);
    MAX7456_WriteString_P(configMsg27, MAGT);
    MAX7456_WriteString(itoa(MW_STATUS.I2CError,screenBuffer,10),MAGD);
  }

  if(configPage==3)
  {
    MAX7456_WriteString_P(configMsg30, 35);

    MAX7456_WriteString_P(configMsg31, ROLLT);
    MAX7456_WriteString(itoa(Settings[S_VOLTAGEMIN],screenBuffer,10),ROLLD);

    /*MAX7456_WriteString_P(configMsg32, PITCHT);                                         // Do not remove yet
    MAX7456_WriteString(itoa(Settings[S_TEMPERATUREMAX],screenBuffer,10),PITCHD);*/ 
    
    MAX7456_WriteString_P(configMsg33, PITCHT); //YAWT);    
    MAX7456_WriteString(itoa(Settings[S_BLINKINGHZ],screenBuffer,10),PITCHD);  //YAWD);
  }

  if(configPage==4)
  {
    MAX7456_WriteString_P(configMsg40, 39);

    MAX7456_WriteString_P(configMsg41, ROLLT);
    MAX7456_WriteString(itoa(rssiADC,screenBuffer,10),ROLLD);

    MAX7456_WriteString_P(configMsg42, PITCHT);
    MAX7456_WriteString(itoa(rssi,screenBuffer,10),PITCHD);

    MAX7456_WriteString_P(configMsg43, YAWT);
    if(rssiTimer>0) MAX7456_WriteString(itoa(rssiTimer,screenBuffer,10),YAWD-5);
    MAX7456_WriteString(itoa(Settings[S_RSSIMIN],screenBuffer,10),YAWD);

    MAX7456_WriteString_P(configMsg44, ALTT);
    MAX7456_WriteString(itoa(Settings[S_RSSIMAX],screenBuffer,10),ALTD);
  }

  if(configPage==5)
  {
    MAX7456_WriteString_P(configMsg50, 37);

    MAX7456_WriteString_P(configMsg51, ROLLT);
    if(accCalibrationTimer>0)
      MAX7456_WriteString(itoa(accCalibrationTimer,screenBuffer,10),ROLLD);
    else
      MAX7456_WriteString("-",ROLLD);

    MAX7456_WriteString_P(configMsg52, PITCHT);
    MAX7456_WriteString(itoa(MW_IMU.accSmooth[0],screenBuffer,10),PITCHD);

    MAX7456_WriteString_P(configMsg53, YAWT);
    MAX7456_WriteString(itoa(MW_IMU.accSmooth[1],screenBuffer,10),YAWD);

    MAX7456_WriteString_P(configMsg54, ALTT);
    MAX7456_WriteString(itoa(MW_IMU.accSmooth[2],screenBuffer,10),ALTD);

    MAX7456_WriteString_P(configMsg55, VELT);
    if(magCalibrationTimer>0)
      MAX7456_WriteString(itoa(magCalibrationTimer,screenBuffer,10),VELD);
    else
      MAX7456_WriteString("-",VELD);

    MAX7456_WriteString_P(configMsg56, LEVT);
    MAX7456_WriteString(itoa(MW_ATT.Heading,screenBuffer,10),LEVD);

    MAX7456_WriteString_P(configMsg57, MAGT);
    if(eepromWriteTimer>0)
      MAX7456_WriteString(itoa(eepromWriteTimer,screenBuffer,10),MAGD);
    else
      MAX7456_WriteString("-",MAGD);
  }

  if(configPage==6)
  {
    MAX7456_WriteString_P(configMsg60, 39);

    MAX7456_WriteString_P(configMsg61, ROLLT);
    if(Settings[S_DISPLAYGPS]){
      MAX7456_WriteString_P(configMsgON, ROLLD);
    }
    else{
      MAX7456_WriteString_P(configMsgOFF, ROLLD);
    }

    MAX7456_WriteString_P(configMsg62, PITCHT);
    if(Settings[S_COORDINATES]){
      MAX7456_WriteString_P(configMsgON, PITCHD);
    }
    else{
      MAX7456_WriteString_P(configMsgOFF, PITCHD);
    }

    MAX7456_WriteString_P(configMsg63, YAWT);
    if(Settings[L_CALLSIGNPOSITIONDSPL]){
      MAX7456_WriteString_P(configMsgON, YAWD);
    }
    else{
      MAX7456_WriteString_P(configMsgOFF, YAWD);
    }
 }

  if(configPage==7)
  {
    MAX7456_WriteString_P(configMsg70, 39);

    MAX7456_WriteString_P(configMsg71, ROLLT);
    if(Settings[S_RESETSTATISTICS]){
      MAX7456_WriteString_P(configMsgON, ROLLD);
    }
    else {
      MAX7456_WriteString_P(configMsgOFF, ROLLD);
      }

    MAX7456_WriteString_P(configMsg72, PITCHT);
    if(Settings[S_HEADING360]){
      MAX7456_WriteString_P(configMsgON, PITCHD);
    }
    else{
      MAX7456_WriteString_P(configMsgOFF, PITCHD);      
    }   
    
    MAX7456_WriteString_P(configMsg73, YAWT);
    if(Settings[S_UNITSYSTEM]==METRIC){
      MAX7456_WriteString_P(configMsg74, YAWD-2);
    }
    else {
      MAX7456_WriteString_P(configMsg75, YAWD-2);
    }

    MAX7456_WriteString_P(configMsg76, ALTT);
    if(Settings[S_VIDEOSIGNALTYPE]){
      MAX7456_WriteString_P(configMsgPAL, ALTD);
    }
    else {
      MAX7456_WriteString_P(configMsgNTSC, ALTD);
      }
   }

  if(configPage==8)
  {
    int16_t screenrow,screencol,displayOn;
    MAX7456_WriteString_P(configMsg80, LINE02+6);       // "8/9 SCREEN ITEM POS"
    
    MAX7456_WriteString_P(configMsg76, LINE04+7);       // "VIDEO SYSTEM"
    if(Settings[S_VIDEOSIGNALTYPE]){
      MAX7456_WriteString_P(configMsgPAL, LINE04+20);   // PAL
    }
    else {
      MAX7456_WriteString_P(configMsgNTSC, LINE04+20);  // NTSC
    }
    
    MAX7456_WriteString_P(configMsg81, LINE06+6);       // "ITEM    DSP LINE COL"
    
    strcpy_P(screenBuffer, (char*)pgm_read_word(&(item_table[screenitemselect]))); // selected item
    MAX7456_WriteString(screenBuffer,LINE07+3);

    screenrow=Settings[screen_pos_item_pointer];
    screencol=Settings[screen_pos_item_pointer+1];
    displayOn=Settings[screen_pos_item_pointer+2];
    if(displayOn){                                                                          // On/Off
      MAX7456_WriteString_P(configMsgON, LINE07+16);
    }
    else{
      MAX7456_WriteString_P(configMsgOFF, LINE07+16);
    }    
    if(screenrow!=255) MAX7456_WriteString(itoa((screenrow),screenBuffer,10),LINE07+21);    // Line position
      else  MAX7456_WriteString_P(configMsgNoAct, LINE07+21);
    if(screencol!=255) MAX7456_WriteString(itoa(screencol,screenBuffer,10),LINE07+25);      // Col position   
      else  MAX7456_WriteString_P(configMsgNoAct, LINE07+25);
      
    MAX7456_WriteString_P(configMsg82, LINE10+3);  // "DEFAULT-EXIT"
  } 

  if(configPage==9)
  {
    int16_t xx;
    MAX7456_WriteString_P(configMsg90, 38);

    MAX7456_WriteString_P(configMsg91, ROLLT);
    MAX7456_WriteString(itoa(trip,screenBuffer,10),ROLLD-3);

    MAX7456_WriteString_P(configMsg92, PITCHT);
    MAX7456_WriteString(itoa(distanceMAX,screenBuffer,10),PITCHD-3);

    MAX7456_WriteString_P(configMsg93, YAWT);
    MAX7456_WriteString(itoa(altitudeMAX,screenBuffer,10),YAWD-3);

    MAX7456_WriteString_P(configMsg94, ALTT);
    MAX7456_WriteString(itoa(speedMAX,screenBuffer,10),ALTD-3);

    MAX7456_WriteString_P(configMsg95, VELT);

    formatTime(flyingTime, screenBuffer, 1);
    MAX7456_WriteString(screenBuffer,VELD-4);

    MAX7456_WriteString_P(configMsg96, LEVT);
    MAX7456_WriteString(itoa(amperagesum,screenBuffer,10),LEVD-3);    

   /* MAX7456_WriteString_P(configMsg97, MAGT);
    MAX7456_WriteString(itoa(temperMAX,screenBuffer,10),MAGD-3);*/
    }
    
  displayCursor();
}

void displaySensor(void)
{
	if(!(MW_STATUS.sensorActive&mode_osd_switch)){
		if(Settings[L_SENSORPOSITIONDSPL]){
			
			// Sensor Present
			screenBuffer[0] = (MW_STATUS.sensorPresent&GPSSENSOR) ? SYM_GPS : ' ';
			screenBuffer[1] = (MW_STATUS.sensorPresent&BAROMETER) ? SYM_BAR : ' ';
			screenBuffer[2] = (MW_STATUS.sensorPresent&MAGNETOMETER) ? SYM_MAG : ' ';
			screenBuffer[3] = (MW_STATUS.sensorPresent&ACCELEROMETER) ? SYM_ACC : ' ';
			screenBuffer[4] = 0;
			MAX7456_WriteString(screenBuffer,((Settings[L_SENSORPOSITIONROW]-1)*30) + Settings[L_SENSORPOSITIONCOL]);
			
			//GPS Search routine
			if((!GPS.fix)&(!BlinkAlarm))
			screenBuffer[0] = (MW_STATUS.sensorPresent&GPSSENSOR)? SYM_GPS_SEARCH:' ';   // Searching sats
			MAX7456_WriteString(screenBuffer,((Settings[L_SENSORPOSITIONROW]-1)*30) + Settings[L_SENSORPOSITIONCOL]);
			
			// Sensor Active
			if(MW_STATUS.sensorActive&mode_baro) screenBuffer[1] = SYM_BAR_ON;
			if(MW_STATUS.sensorActive&mode_mag) screenBuffer[2] = SYM_MAG_ON;
			if(MW_STATUS.sensorActive&mode_stable|(MW_STATUS.sensorActive&mode_horizon && MwRcData[PITCHSTICK] > 1450 && MwRcData[PITCHSTICK] < 1550)) screenBuffer[3] =  SYM_ACC_ON;
			MAX7456_WriteString(screenBuffer,((Settings[L_SENSORPOSITIONROW]-1)*30) + Settings[L_SENSORPOSITIONCOL]);
		}
	}
}

void displayGPSMode(void)                                                                          // Moves together with Settings[L_SENSORPOSITION].
{  if((GPS.fix) & (!(MW_STATUS.sensorActive&mode_osd_switch) & (Settings[L_SENSORPOSITIONDSPL])))
	if(MW_STATUS.sensorActive&mode_gpshome)
	screenBuffer[0]=SYM_G_HOME;
	else
	if(MW_STATUS.sensorActive&mode_gpshold)
	screenBuffer[0]=SYM_HOLD;
	else
	screenBuffer[0]=SYM_3DFIX;
	else
	return;              //Back to routine
	screenBuffer[1]=0;
	MAX7456_WriteString(screenBuffer,((Settings[L_SENSORPOSITIONROW]-1)*30) + Settings[L_SENSORPOSITIONCOL]);
}

void displayautoPilot(void){
	if(GPS.fix & flyTime > 7)
	if(MW_STATUS.sensorActive&mode_gpshome||MW_STATUS.sensorActive&mode_gpshold){    // Autopilot warning override SMART SW
		screenBuffer[0]=SYM_AUTOPILOT;
		screenBuffer[1]=SYM_AUTOPILOT_1;
		screenBuffer[2]=SYM_AUTOPILOT_2;
		screenBuffer[3]=SYM_AUTOPILOT_3;
		screenBuffer[4]=0;
		if (BlinkAlarm)
		MAX7456_WriteString(screenBuffer,((Settings[L_SENSORPOSITIONROW]-1)*30) + Settings[L_SENSORPOSITIONCOL]+LINE);
	}
}
