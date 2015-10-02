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

// For Time
uint16_t onTime=0;
uint16_t flyTime=0;

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
int8_t temperMAX=0;
uint16_t flyingTime=0;

//-------------- Timed Service Routine vars (No need Metro.h library)
uint16_t previous_millis_low=0;
uint16_t previous_millis_high =0;
uint8_t hi_speed_cycle = 50;
uint8_t lo_speed_cycle = 100;
//----------------

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
  else {
    modeMSPRequests =
      REQ_MSP_RAW_IMU;
    }
   
  if(Settings[S_MAINVOLTAGE_VBAT] ||
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
      case REQ_MSP_ANALOG:
        MSPcmdsend = MSP_ANALOG;
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
      else
      {
        
        displayVoltage();
        displayRSSI();
        displayTime();
        //if((temperature<Settings[S_TEMPERATUREMAX])||(BlinkAlarm)) displayTemperature();
        //displayAmperage();
        //displaypMeterSum();

        if(MW_STATUS.sensorPresent&ACCELEROMETER)
           displayHorizon(MW_ATT.Angle[0],MW_ATT.Angle[1]);
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

    if((eepromWriteTimer==1)&&(configMode)) {
      blankserialRequest(MSP_EEPROM_WRITE);
      eepromWriteTimer=0;
    }

    if(eepromWriteTimer>0) eepromWriteTimer--;

    if((rssiTimer==1)&&(configMode)) {
      Settings[S_RSSIMIN]=rssiADC;  // set MIN RSSI signal received (tx off?)
      rssiTimer=0;
    }
    if(rssiTimer>0) rssiTimer--;
  }
  
  serialMSPreceive();

}  // End of main loop



