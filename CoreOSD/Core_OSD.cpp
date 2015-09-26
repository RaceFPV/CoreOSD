#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "Config.h"
#include "types.h"
#include "EEPROM.h" //Needed to access eeprom read/write functions
#include "Max7456.h"
#include "Serial.h"
#include "Protocol.h"
#include "symbols.h"
#include "Screen.h"
#include "CoreOSD.h"

// ----Variables---
//Analog input defines
//static const uint16_t voltagePin=0;
//static const uint16_t vidvoltagePin=2;
//static const uint16_t amperagePin=1;
//static const uint16_t rssiPin=3;
//static const uint16_t temperaturePin=6;            // Temperature pin 6 for original Rushduino Board V1.2
static const uint8_t rssiSample=30;

//General use variables
uint8_t tenthSec=0;
uint8_t TempBlinkAlarm=0;                       // Temporary for blink alarm
uint8_t BlinkAlarm=0;                           // This is turning on and off at selected freq. (alarm)
uint8_t Blink10hz=0;                            // This is turning on and off at 10hz
uint16_t lastCallSign=0;                         // callsign_timer
uint8_t rssiTimer=0;
uint8_t accCalibrationTimer=0;
uint8_t magCalibrationTimer=0;
uint8_t eepromWriteTimer=0;

uint16_t allSec=0;

// Config status and cursor location
uint8_t ROW=10;
uint8_t COL=4;
uint8_t configPage=MINPAGE;
uint8_t configMode=0;
uint8_t fontMode = 0;
uint8_t fontData[54];
uint8_t nextCharToRequest;
uint8_t lastCharToRequest;
uint8_t retransmitQueue;

// Mode bits
uint32_t mode_armed;
uint32_t mode_stable;
uint32_t mode_horizon;
uint32_t mode_baro;
uint32_t mode_mag;
uint32_t mode_gpshome;
uint32_t mode_gpshold;
uint32_t mode_osd_switch;

uint32_t modeMSPRequests;
uint32_t queuedMSPRequests;

uint8_t Settings[EEPROM_ITEM_LOCATION];

// For Settings Defaults

// Screen is the Screen buffer between program an MAX7456 that will be writen to the screen at 10hz
char screen[480];
// ScreenBuffer is an intermietary buffer to created Strings to send to Screen buffer
char screenBuffer[20];
//uint16_t  MwAccSmooth[3]={0,0,0};       // Those will hold Accelerator data
uint16_t MwRcData[8]={		// This hold receiver pulse signal
1500,1500,1500,1500,1500,1500,1500,1500} ;
	
conf_t conf;
MW_ALTTITUDE_t MW_ALT;
MW_ATTITUDE_t MW_ATT;
MW_status_t MW_STATUS;
MW_imu_t MW_IMU;

//uint8_t MwVersion=0;

//uint8_t MwVBat=0;
//uint16_t MwRssi=0;
//uint16_t MWAmperage=0;
//uint16_t pMeterSum=0;
MW_ANALOG_t MW_ANALOG;

//uint16_t Debug1=0;

uint8_t armed=0;
uint8_t previousarmedstatus=0;  // for statistics after disarming

GPS_t GPS;

//For Current Throttle
uint16_t LowT = 1100;
uint16_t HighT = 1900;

// For Time
uint16_t onTime=0;
uint16_t flyTime=0;
// For Heading
/*const char headGraph[] PROGMEM = {
  0XFB,0XFE,0XFD,0XFE,0XFA,0XFE,0XFD,0XFE,0XFC,0XFE,0XFD,0XFE,0XF9,0XFE,0XFD,0XFE,0XFB,0XFE,0XFD,0XFE,0XFA,0XFE,0XFD,0XFE,0XFC,0};*/
const char headGraph[] PROGMEM = {
  0XFB,0XFE,0XFA,0XFE,0XFC,0XFE,0XF9,0XFE,0XFB,0XFE,0XFA,0XFE,0XFC,0};

//static int16_t MwHeading=0;

// For Amperage
float amperageADC =0;
int16_t amperage_Int=0;
float amperage = 0;                // its the real value x10
float amperagesum = 0;

// Rssi
int16_t rssi =0;
int16_t rssiADC=0;
int16_t rssiMin;
int16_t rssiMax;
int16_t rssi_Int=0;


// For Voltage
uint16_t voltage=0;                      // its the value x10
uint16_t vidvoltage=0;                   // its the value x10

// For temperature
static int16_t temperature=0;                  // temperature in degrees Centigrade


// For Statistics
uint16_t speedMAX=0;
int8_t temperMAX=0;
int16_t altitudeMAX=0;
int16_t distanceMAX=0;
float trip=0;
uint16_t flyingTime=0;

// For Intro
const char message0[] PROGMEM = "KV_OSD_TEAM_2.3.2";
const char message5[] PROGMEM = "MW VER:";
const char message6[] PROGMEM = "MENU:THRT MIDDLE";
const char message7[] PROGMEM = "YAW RIGHT";
const char message8[] PROGMEM = "PITCH FULL";
const char message9[] PROGMEM = "UNIQUE ID:";         // Call Sign on the beginning of the transmission

// For Config menu common
const char configMsgON[] PROGMEM = "ON";
const char configMsgOFF[] PROGMEM = "OFF";
const char configMsgNoAct[] PROGMEM = "--";
const char configMsgEXIT[] PROGMEM = "EXIT";
const char configMsgSAVE[] PROGMEM = "SAVE-EXIT";
const char configMsgPGS[] PROGMEM = "<PAGE>";
const char configMsgNTSC[] PROGMEM = "NTSC";
const char configMsgPAL[] PROGMEM = "PAL";

// For Config pages
//-----------------------------------------------------------Page1
const char configMsg10[] PROGMEM = "1/9 PID CONFIG";
const char configMsg11[] PROGMEM = "ROLL";
const char configMsg12[] PROGMEM = "PITCH";
const char configMsg13[] PROGMEM = "YAW";
const char configMsg14[] PROGMEM = "ALT";
const char configMsg15[] PROGMEM = "GPS";
const char configMsg16[] PROGMEM = "LEVEL";
const char configMsg17[] PROGMEM = "MAG";
//-----------------------------------------------------------Page2
const char configMsg20[] PROGMEM = "2/9 RC TUNING";
const char configMsg21[] PROGMEM = "RC RATE";
const char configMsg22[] PROGMEM = "EXPONENTIAL";
const char configMsg23[] PROGMEM = "ROLL PITCH RATE";
const char configMsg24[] PROGMEM = "YAW RATE";
const char configMsg25[] PROGMEM = "THROTTLE PID ATT";
const char configMsg26[] PROGMEM = "MWCYCLE TIME";
const char configMsg27[] PROGMEM = "MWI2C ERRORS";
//-----------------------------------------------------------Page3
const char configMsg30[] PROGMEM = "3/9 SUPPLY & ALARM";
const char configMsg31[] PROGMEM = "VOLTAGE ALARM";
//const char configMsg32[] PROGMEM = "SET TEMP ALARM";                 // Do not remove yet
const char configMsg33[] PROGMEM = "BLINKING FREQ";
//-----------------------------------------------------------Page4
const char configMsg40[] PROGMEM = "4/9 RSSI";
const char configMsg41[] PROGMEM = "ACTUAL RSSI RAW";
const char configMsg42[] PROGMEM = "ACTUAL RSSI %";
const char configMsg43[] PROGMEM = "SET RSSI MIN";
const char configMsg44[] PROGMEM = "SET RSSI MAX";
//-----------------------------------------------------------Page5
const char configMsg50[] PROGMEM = "5/9 CALIBRATION";
const char configMsg51[] PROGMEM = "ACC CALIBRATION";
const char configMsg52[] PROGMEM = "ACC ROLL";
const char configMsg53[] PROGMEM = "ACC PITCH";
const char configMsg54[] PROGMEM = "ACC Z";
const char configMsg55[] PROGMEM = "MAG CALIBRATION";
const char configMsg56[] PROGMEM = "HEADING";
const char configMsg57[] PROGMEM = "MW EEPROM WRITE";
//-----------------------------------------------------------Page6
const char configMsg60[] PROGMEM = "6/9 GPS";
const char configMsg61[] PROGMEM = "DISPLAY GPS DATA";
const char configMsg62[] PROGMEM = "GPS COORD";
const char configMsg63[] PROGMEM = "CALLSIGN";
//-----------------------------------------------------------Page7
const char configMsg70[] PROGMEM = "7/9 ADV SETUP";
const char configMsg71[] PROGMEM = "RESET STATS";
const char configMsg72[] PROGMEM = "HEADING 0-360";
const char configMsg73[] PROGMEM = "UNIT SYST";
const char configMsg74[] PROGMEM = "METRIC";
const char configMsg75[] PROGMEM = "IMPERL";
const char configMsg76[] PROGMEM = "VIDEO SYST";
//-----------------------------------------------------------Page8
const char configMsg80[] PROGMEM = "8/9 SCREEN ITEM POS";
const char configMsg81[] PROGMEM = "ITEM      DSP LINE COL";
const char configMsg82[] PROGMEM = "DEFAULT-EXIT";
//-----------------------------------------------------------Page9
const char configMsg90[] PROGMEM = "9/9 STATS";
const char configMsg91[] PROGMEM = "TRIP";
const char configMsg92[] PROGMEM = "MAX DISTANCE";
const char configMsg93[] PROGMEM = "MAX ALTITUDE";
const char configMsg94[] PROGMEM = "MAX SPEED";
const char configMsg95[] PROGMEM = "FLYING TIME";
const char configMsg96[] PROGMEM = "AMPS DRAINED";
const char configMsg97[] PROGMEM = "MAX TEMP";


// Variables for items pos change on screen
//-----------------------------------------------------------
int8_t screenitemselect=0; // pointer for item text strings
int8_t screen_pos_item_pointer=EEPROM_SETTINGS+1;  // pointer for first item display/row/col positions 

// Strings for item select on screen
//-----------------------------------------------------------
const char screen_item_00[] PROGMEM = "NUM SAT";
const char screen_item_01[] PROGMEM = "DIR TO HOME";
const char screen_item_02[] PROGMEM = "DIST TO HOME";
const char screen_item_03[] PROGMEM = "GPS SPEED";
const char screen_item_04[] PROGMEM = "ANGLE TO HOM";
//const char screen_item_05[] PROGMEM = "GPS ALTITUDE";
const char screen_item_05[] PROGMEM = "SENSORS";
const char screen_item_06[] PROGMEM = "FLIGHT MODE";
const char screen_item_07[] PROGMEM = "HEADING";
const char screen_item_08[] PROGMEM = "HEAD GRAPH";
//const char screen_item_09[] PROGMEM = "TEMPERATURE";  //Do not remove yet
const char screen_item_09[] PROGMEM = "BARO ALTIT";
const char screen_item_10[] PROGMEM = "CLIMB RATE";
const char screen_item_11[] PROGMEM = "HORIZON";
const char screen_item_12[] PROGMEM = "AH SIDE REF";
const char screen_item_13[] PROGMEM = "AH CENTR REF";
const char screen_item_14[] PROGMEM = "THROTTLE";
const char screen_item_15[] PROGMEM = "FLY TIME";
const char screen_item_16[] PROGMEM = "ON TIME";
const char screen_item_17[] PROGMEM = "ARMED INDIC";
const char screen_item_18[] PROGMEM = "GPS LATIT";
const char screen_item_19[] PROGMEM = "GPS LONGIT";
const char screen_item_20[] PROGMEM = "RSSI";
const char screen_item_21[] PROGMEM = "MAIN BATT";
const char screen_item_22[] PROGMEM = "MAIN BAT EVO";
const char screen_item_23[] PROGMEM = "VIDEO BATT";
const char screen_item_24[] PROGMEM = "AMPERAGE";
const char screen_item_25[] PROGMEM = "MA/H CONSUM";
const char screen_item_26[] PROGMEM = "CALLSIGN";

PROGMEM const char * const item_table[] =
{
	screen_item_00,
	screen_item_01,
	screen_item_02,
	screen_item_03,
	screen_item_04,
	screen_item_05,
	screen_item_06,
	screen_item_07,
	screen_item_08,
	screen_item_09,
	screen_item_10,
	screen_item_11,
	screen_item_12,
	screen_item_13,
	screen_item_14,
	screen_item_15,
	screen_item_16,
	screen_item_17,
	screen_item_18,
	screen_item_19,
	screen_item_20,
	screen_item_21,
	screen_item_22,
	screen_item_23,
	screen_item_24,
	screen_item_25,
	screen_item_26,
	//screen_item_27,
};


/* POSITION OF EACH CHARACTER OR LOGO IN THE MAX7456
const unsigned char speedUnitAdd[2] ={
SYM_KMH_1,SYM_MPH_1} ;                               // [0][0] and [0][1] = Km/h   [1][0] and [1][1] = Mph
const unsigned char speedUnitAdd1[2] ={
SYM_KMH_2,SYM_MPH_2} ;
const unsigned char temperatureUnitAdd[2] = {
0x0e,0x0d};
*/

// POSITION OF EACH CHARACTER OR LOGO IN THE MAX7456
const char MultiWiiLogoL1Add[17] PROGMEM = {
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0};
const char MultiWiiLogoL2Add[17] PROGMEM = {
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0};
const char MultiWiiLogoL3Add[17] PROGMEM = {
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0};

static const char KVTeamVersionPosition = 35;


//-------------- Timed Service Routine vars (No need Metro.h library)
uint16_t previous_millis_low=0;
uint16_t previous_millis_high =0;
uint8_t hi_speed_cycle = 50;
uint8_t lo_speed_cycle = 100;
//----------------


void calculateTrip(void)
{
	if(GPS.fix && armed && (GPS.speed>0)) {
		if(Settings[S_UNITSYSTEM])
		      trip += GPS.speed *0.0016404;     //  50/(100*1000)*3.2808=0.0016404     cm/sec ---> ft/50msec
		//trip += GPS.speed*screen_update_cycle_time*0.000032808;     //  1/(100*1000)*3.2808=0.000032808     cm/sec ---> ft/msec
		else
		      trip += GPS.speed *0.0005;        //  50/(100*1000)=0.0005               cm/sec ---> mt/50msec (trip var is float)
		//trip += GPS.speed*screen_update_cycle_time*0.00001;        //  1/(100*1000)=0.00001               cm/sec ---> mt/msec (trip var is float)
	}
}


void calculateRssi(void)
{
	float aa=0;
	
	aa = rssiADC;  // actual RSSI signal received  (already divided by 4)
	aa = ((aa-Settings[S_RSSIMIN]) *101)/(Settings[S_RSSIMAX]-Settings[S_RSSIMIN]) ;  // Percentage of signal strength
	rssi_Int += ( ( (signed int)((aa*rssiSample) - rssi_Int )) / rssiSample );  // Smoothing the readings
	rssi = rssi_Int / rssiSample ;
	if(rssi<0) rssi=0;
	if(rssi>100) rssi=100;
}



void setup()
{
  SerialOpen(0,115200);
  SerialFlush(0);
  
  //PWM RSSI
  pinMode(PWMrssiPin, INPUT);
  
  //Led output
  pinMode(7,OUTPUT);  // PD7
 
  checkEEPROM();
  readEEPROM();
  MAX7456_Setup();
  
  analogReference(DEFAULT);

  setMspRequests();

  blankserialRequest(MSP_IDENT);
}


void setMspRequests() {
  if(fontMode) {
      modeMSPRequests = REQ_MSP_FONT;
  }
  else if(configMode) {
    modeMSPRequests = 
      REQ_MSP_IDENT|
      REQ_MSP_STATUS|
      REQ_MSP_RAW_GPS|
      REQ_MSP_ATTITUDE|
      REQ_MSP_RAW_IMU|
      REQ_MSP_ALTITUDE|
      REQ_MSP_RC_TUNING|
      REQ_MSP_PID|
      REQ_MSP_RC;
  }
  else {
    modeMSPRequests = 
      REQ_MSP_IDENT|
      REQ_MSP_STATUS|
      REQ_MSP_RAW_GPS|
      REQ_MSP_COMP_GPS|
      REQ_MSP_ATTITUDE|
      REQ_MSP_RAW_IMU|      
	  REQ_MSP_ALTITUDE;

    if(MW_STATUS.version == 0)
      modeMSPRequests |= REQ_MSP_IDENT;

    if(!armed || Settings[L_CURRENTTHROTTLEPOSITIONDSPL])
      modeMSPRequests |= REQ_MSP_RC;

    if(mode_armed == 0) {
        modeMSPRequests |= REQ_MSP_BOX;

    }
  }
 
  if(Settings[S_MAINVOLTAGE_VBAT] ||
     Settings[S_VIDVOLTAGE_VBAT] ||
     Settings[S_MWRSSI])
    modeMSPRequests |= REQ_MSP_ANALOG;

  // so we do not send requests that are not needed.
  queuedMSPRequests &= modeMSPRequests;
}

void calculateAmperage(void)
{
	float aa=0;
	// calculation of amperage [A*10]
	aa = amperageADC*10;   // *10 We want one decimal digit
	amperage_Int += ( ( (signed int)((aa* 10) - amperage_Int )) / 10 );  // Smoothing the displayed value with average of 10 samples
	amperage = (amperage_Int / 10);
	if (amperage >=999) amperage=999;  // [A*10]
	
	// Calculation of amperagesum = amperage integration on 50msec (without reading average)
	// 720= *100/72000 --> where:
	// *100=mA (instead of *1000 as the value is already multiplied by 10)
	// 72000=n. of 50ms in 1 hour
	amperagesum += aa /720; // [mAh]    // NOTE: if want add 3%, change this "amperagesum += aa /690;"
}


void loop()
{
       // Process AI   
  if (Settings[S_ENABLEADC]){
    //temperature=(analogRead(temperaturePin)-102)/2.048; // Does someone use this ATM??
    if (!Settings[S_MAINVOLTAGE_VBAT]){
      static uint16_t ind = 0;
      static uint32_t voltageRawArray[8];
      voltageRawArray[(ind++)%8] = analogRead(voltagePin);                  
      uint16_t voltageRaw = 0;
      for (uint16_t i=0;i<8;i++)
        voltageRaw += voltageRawArray[i];
      voltage = float(voltageRaw) * Settings[S_DIVIDERRATIO] /1023; 
    }
    if (!Settings[S_VIDVOLTAGE_VBAT]) {     
      static uint16_t ind = 0;
      static uint32_t voltageRawArray[8];
      voltageRawArray[(ind++)%8] = analogRead(vidvoltagePin);                  
      uint16_t voltageRaw = 0;
      for (uint16_t i=0;i<8;i++)
        voltageRaw += voltageRawArray[i];
      vidvoltage = float(voltageRaw) * Settings[S_VIDDIVIDERRATIO] /1023;  
    }
    if (!Settings[S_MWRSSI] && !Settings[S_PWMRSSI]) {
      rssiADC = analogRead(rssiPin)/4;  // RSSI Readings, rssiADC=0 to 1023/4 (avoid a number > 255)
    }
    if (!Settings[S_MWAMPERAGE]) {
      int16_t currsensOffSet=(Settings[S_CURRSENSOFFSET_L] | (Settings[S_CURRSENSOFFSET_H] << 8));  // Read OffSetH/L
      amperageADC = analogRead(amperagePin);
      if (amperageADC > currsensOffSet) amperageADC=((amperageADC-currsensOffSet)*4.8828)/Settings[S_CURRSENSSENSITIVITY]; // [A] Positive Current flow (512...1023) or Unidir (0...1023)
      else amperageADC=((currsensOffSet-amperageADC)*4.8828)/Settings[S_CURRSENSSENSITIVITY]; // [A] Negative Current flow (0...512)
      }
}
   if (Settings[S_MWAMPERAGE]) {
     amperagesum = MW_ANALOG.pMeterSum;
     amperage = MW_ANALOG.Amperage /100;
    }
	
  if (Settings[S_MWRSSI]) {
      rssiADC = MW_ANALOG.Rssi/4;  // RSSI from MWii, rssiADC=0 to 1023/4 (avoid a number > 255)
    } 
	
  if (Settings[S_PWMRSSI] && !Settings[S_MWRSSI]){
	rssiADC = pulseIn(PWMrssiPin, HIGH,15000)/Settings[S_PWMRSSIDIVIDER]; // Reading W/time out (microseconds to wait for pulse to start: 15000=0.015sec)
    }
   
  // Blink Basic Sanity Test Led at 1hz - this stuff introduces strange behavior on my system
  if(tenthSec>10)
    digitalWrite(7,HIGH);
  else
    digitalWrite(7,LOW);


  //---------------  Start Timed Service Routines  ---------------------------------------
  uint16_t currentMillis = millis();

  if((currentMillis - previous_millis_low) >= lo_speed_cycle)  // 10 Hz (Executed every 100ms)
  {
    previous_millis_low = currentMillis;
	
	tenthSec++;
	TempBlinkAlarm++;
	Blink10hz=!Blink10hz;
	
	if(!fontMode)
      blankserialRequest(MSP_ATTITUDE);
      
    if(Settings[L_RSSIPOSITIONDSPL])
      calculateRssi();      
  }  // End of slow Timed Service Routine (100ms loop)

  if((currentMillis - previous_millis_high) >= hi_speed_cycle)  // 20 Hz (Executed every 50ms)
  {
    previous_millis_high = currentMillis;   

    calculateTrip();      // Speed integration on 50msec
    if (!Settings[S_MWAMPERAGE]) calculateAmperage();  // Amperage and amperagesum integration on 50msec
    
      uint8_t MSPcmdsend;
      if(queuedMSPRequests == 0)
        queuedMSPRequests = modeMSPRequests;
    uint32_t req = queuedMSPRequests & -queuedMSPRequests;
    queuedMSPRequests &= ~req;
      switch(req) {
      case REQ_MSP_IDENT:
        MSPcmdsend = MSP_IDENT;
        break;
      case REQ_MSP_STATUS:
        MSPcmdsend = MSP_STATUS;
        break;
      case REQ_MSP_RAW_IMU:
        MSPcmdsend = MSP_RAW_IMU;
        break;
      case REQ_MSP_RC:
        MSPcmdsend = MSP_RC;
        break;
      case REQ_MSP_RAW_GPS:
        MSPcmdsend = MSP_RAW_GPS;
        break;
      case REQ_MSP_COMP_GPS:
        MSPcmdsend = MSP_COMP_GPS;
        break;
      case REQ_MSP_ATTITUDE:
        MSPcmdsend = MSP_ATTITUDE;
        break;
      case REQ_MSP_ALTITUDE:
        MSPcmdsend = MSP_ALTITUDE;
        break;
      case REQ_MSP_ANALOG:
        MSPcmdsend = MSP_ANALOG;
        break;
      case REQ_MSP_RC_TUNING:
        MSPcmdsend = MSP_RC_TUNING;
        break;
      case REQ_MSP_PID:
        MSPcmdsend = MSP_PID;
        break;
      case REQ_MSP_BOX:
        MSPcmdsend = MSP_BOXIDS;
         break;
      case REQ_MSP_FONT:
        MSPcmdsend = MSP_OSD;
      break;
    }
      if(!fontMode)
      blankserialRequest(MSPcmdsend);     

    //MAX7456_DrawScreen();
    
    if( allSec < 6 ){
      displayIntro(KVTeamVersionPosition);
      lastCallSign = onTime;
    }  
    else
    {
      if(armed){
        previousarmedstatus=1;
      }
      if(previousarmedstatus && !armed){
        configPage=9;
        ROW=10;
        COL=1;
        configMode=1;
        setMspRequests();
      }
      if(fontMode) {
         displayFontScreen();
      }
      else if(configMode)
      {
        displayConfigScreen();
      }
      else
      {
        
        displayVoltage();
        displayVidVoltage();
        displayRSSI();
        displayTime();
        displaySensor();
        displayGPSMode();
        displayMode();
        //if((temperature<Settings[S_TEMPERATUREMAX])||(BlinkAlarm)) displayTemperature();
        displayAmperage();
        displaypMeterSum();
        displayArmed();
        displayCurrentThrottle();
        displayautoPilot(); 

        if ( (onTime > (lastCallSign+300)) || (onTime < (lastCallSign+4)))
       {
           // Displays 4 sec every 5min (no blink during flight)
        if ( onTime > (lastCallSign+300))lastCallSign = onTime; 
        displayCallsign(); 
       
       }

        if(MW_STATUS.sensorPresent&ACCELEROMETER)
           displayHorizon(MW_ATT.Angle[0],MW_ATT.Angle[1]);

        if(MW_STATUS.sensorPresent&MAGNETOMETER) {
          displayHeadingGraph();
          displayHeading();
        }

        if(MW_STATUS.sensorPresent&BAROMETER) {
          displayAltitude();
          displayClimbRate();
        }

        if(MW_STATUS.sensorPresent&GPSSENSOR) 
          if(Settings[S_DISPLAYGPS]){
            displayNumberOfSat();
            displayDirectionToHome();
            displayDistanceToHome();
            displayAngleToHome();
            displayGPS_speed();
            displayGPSPosition();
            //displayGPS_altitude(); // Do not remove yet
          }
      }
    }
	 MAX7456_DrawScreen();
  }  // End of fast Timed Service Routine (50ms loop)
//---------------------  End of Timed Service Routine ---------------------------------------


  if(TempBlinkAlarm >= Settings[S_BLINKINGHZ]) {    // selectable alarm blink freq
    TempBlinkAlarm = 0;
    BlinkAlarm =!BlinkAlarm;     // 10=1Hz, 9=1.1Hz, 8=1.25Hz, 7=1.4Hz, 6=1.6Hz, 5=2Hz, 4=2.5Hz, 3=3.3Hz, 2=5Hz, 1=10Hz
  }

 
  if(tenthSec >= 10)     // this execute 1 time a second
  {
    onTime++;
    
    tenthSec=0;

    if(!armed) {
      flyTime=0;
    }
    else {
      flyTime++;
      flyingTime++;
      configMode=0;
      setMspRequests();
    }
    allSec++;
    
    

    if((accCalibrationTimer==1)&&(configMode)) {
      blankserialRequest(MSP_ACC_CALIBRATION);
      accCalibrationTimer=0;
    }

    if((magCalibrationTimer==1)&&(configMode)) {
      blankserialRequest(MSP_MAG_CALIBRATION);
      magCalibrationTimer=0;
    }

    if((eepromWriteTimer==1)&&(configMode)) {
      blankserialRequest(MSP_EEPROM_WRITE);
      eepromWriteTimer=0;
    }

    if(accCalibrationTimer>0) accCalibrationTimer--;
    if(magCalibrationTimer>0) magCalibrationTimer--;
    if(eepromWriteTimer>0) eepromWriteTimer--;

    if((rssiTimer==1)&&(configMode)) {
      Settings[S_RSSIMIN]=rssiADC;  // set MIN RSSI signal received (tx off?)
      rssiTimer=0;
    }
    if(rssiTimer>0) rssiTimer--;
  }
  
  serialMSPreceive();

}  // End of main loop



