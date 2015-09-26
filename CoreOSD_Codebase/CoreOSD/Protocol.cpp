#include "Protocol.h"
#include "Config.h"
#include "EEPROM.h"
#include "Max7456.h"
#include "KV_Team_OSD.h"
#include "Serial.h"

//void (* resetFunc)(void)=0x0000;

static uint8_t checksum[UART_NUMBER];
//static uint8_t indRX[UART_NUMBER];
static uint8_t cmdMSP[UART_NUMBER];

static uint8_t serialBuffer[SERIALBUFFERSIZE]; // this hold the incoming string from serial O string

static uint8_t receiverIndex;
static uint8_t dataSize;
static uint8_t rcvChecksum;
static uint8_t readIndex;

uint8_t read8()  {
	return serialBuffer[readIndex++];
}

uint16_t read16() {
	uint16_t t = read8();
	t |= (uint16_t)read8()<<8;
	return t;
}

uint32_t read32() {
	uint32_t t = read16();
	t |= (uint32_t)read16()<<16;
	return t;
}

void  r_struct(uint8_t *cb,uint8_t siz) {
	while(siz--) *cb++ = read8();
}


void serialize8(uint8_t a) {
	SerialSerialize(0,a);
	checksum[0] ^= a;
}
void serialize16(int16_t a) {
	serialize8((a   ) & 0xFF);
	serialize8((a>>8) & 0xFF);
}
void serialize32(uint32_t a) {
	serialize8((a    ) & 0xFF);
	serialize8((a>> 8) & 0xFF);
	serialize8((a>>16) & 0xFF);
	serialize8((a>>24) & 0xFF);
}

void headSerialResponse(uint8_t err, uint8_t s) {
	serialize8('$');
	serialize8('M');
	serialize8(err ? '!' : '<');
	checksum[0] = 0; // start calculating a new checksum
	serialize8(s);
	serialize8(cmdMSP[0]);
}

void headSerialReply(uint8_t s) {
	headSerialResponse(0, s);
}

void inline headSerialError(uint8_t s) {
	headSerialResponse(1, s);
}

void tailSerialReply() {
	serialize8(checksum[0]);UartSendData(0);
}

void serializeNames(PGM_P s) {
	headSerialReply(strlen_P(s));
	for (PGM_P c = s; pgm_read_byte(c); c++) {
		serialize8(pgm_read_byte(c));
	}
}

void  s_struct(uint8_t *cb,uint8_t siz) {
	headSerialReply(siz);
	while(siz--) serialize8(*cb++);
}

void s_struct_w(uint8_t *cb,uint8_t siz) {
	headSerialReply(0);
	while(siz--) *cb++ = read8();
}

int16_t getNextCharToRequest() {
	if(nextCharToRequest != lastCharToRequest) {	// Not at last char
		if(retransmitQueue & 0x02) {                // Missed char at curr-6. Need retransmit!
			return nextCharToRequest-6;
		}

		if((retransmitQueue & 0x11) == 0x10) {      // Missed char at curr-3. First chance retransmit
			retransmitQueue |= 0x01;                // Mark first chance as used
			return nextCharToRequest-3;
		}

		retransmitQueue = (retransmitQueue >> 1) | 0x80; // Add next to queue
		return nextCharToRequest++;                      // Notice post-increment!
	}

	uint8_t temp1 = retransmitQueue & ~0x01;
	uint8_t temp2 = nextCharToRequest - 6;

	if(temp1 == 0) {
		fontMode = 0;                            // Exit font mode
		setMspRequests();
		return -1;
	}

	// Already at last char... check for missed characters.
	while(!(temp1 & 0x03)) {
		temp1 >>= 1;
		temp2++;
	}

	return temp2;
}


void fontSerialRequest() {
	int16_t cindex = getNextCharToRequest();
	headSerialReply(3);
	serialize8(OSD_GET_FONT);
	serialize16(cindex);
	tailSerialReply();	
}

uint8_t safeMode() {
	return 1;	// XXX
}

// Font upload queue implementation.
// Implement a window for curr + the previous 6 requests.
// First-chance to retransmit at curr-3 (retransmitQueue & 0x10)
// First-chance retransmit marked as used at retransmitQueue |= 0x01
// 2 to N-chance retransmit marked at curr-6 (retransmitQueue & 0x02)
void initFontMode() {
	if(armed || configMode || fontMode|| !safeMode())
	return;
	// queue first char for transmission.
	retransmitQueue = 0x80;

	fontMode = 1;
	setMspRequests();
}

void fontCharacterReceived(uint8_t cindex) {
	if(!fontMode)
	return;

	uint8_t bit = (0x80 >> (nextCharToRequest-cindex));

	// Just received a char..
	if(retransmitQueue & bit) {
		// this char war requested and now received for the first time
		retransmitQueue &= ~bit;  // mark as already received
		MAX7456_writeNVM(cindex);       // Write to MVRam
	}
}



void blankserialRequest(uint8_t requestMSP)
{
	if(requestMSP == MSP_OSD && fontMode) {
		fontSerialRequest();
		return;
	}
	serialize8('$');
	serialize8('M');
	serialize8('<');
	serialize8((uint8_t)0x00);
	serialize8(requestMSP);
	serialize8(requestMSP);
	UartSendData(0);
}

void configExit()
{
	configPage=1;
	ROW=10;
	COL=4;
	configMode=0;
	//waitStick=3;
	previousarmedstatus = 0;
	if (Settings[S_RESETSTATISTICS]){
		trip=0;
		distanceMAX=0;
		altitudeMAX=0;
		speedMAX=0;
		temperMAX =0;
		flyingTime=0;
	}
	setMspRequests();
}



void saveExit()
{
	//uint8_t txCheckSum;
	//uint8_t txSize;

	if (configPage==1){
		cmdMSP[0] = MSP_SET_PID;
		s_struct((uint8_t*)&conf.pid[0].P8,3*PIDITEMS);
		tailSerialReply();
	}

	if (configPage==2){
		cmdMSP[0] = MSP_SET_RC_TUNING;
		s_struct((uint8_t*)&conf.rcRate8,7);
		tailSerialReply();
	}
	if (configPage==3 || configPage==4 || configPage==6 || configPage==7 || configPage==8){
		writeEEPROM();
	}
	configExit();
}

void handleRawRC() {
	static uint8_t waitStick;
	static uint32_t stickTime;
	static uint32_t timeout;

	if(MwRcData[PITCHSTICK] > 1300 && MwRcData[PITCHSTICK] < 1700 &&
	MwRcData[ROLLSTICK] > 1300 && MwRcData[ROLLSTICK] < 1700 &&
	MwRcData[YAWSTICK] > 1300 && MwRcData[YAWSTICK] < 1700) {
		waitStick = 0;
		timeout = 1000;
	}
	else if(waitStick == 1) {
		if((millis() - stickTime) > timeout)
		waitStick = 0;
		timeout = 300;
	}

	if(!waitStick)
	{
		if((MwRcData[PITCHSTICK]>MAXSTICK)&&(MwRcData[YAWSTICK]>MAXSTICK)&&(MwRcData[THROTTLESTICK]>MINSTICK)&&!configMode&&(allSec>5)&&!armed)
		{
			// Enter config mode using stick combination
			waitStick =  2;	// Sticks must return to center before continue!
			configMode = 1;
			setMspRequests();
		}
		else if(configMode) {
			if(previousarmedstatus&&(MwRcData[THROTTLESTICK]>MINSTICK))
			{
				// EXIT from SHOW STATISTICS AFTER DISARM (push throttle up)
				waitStick = 2;
				configExit();
			}
			if(!previousarmedstatus&&configMode&&(MwRcData[THROTTLESTICK]<MINSTICK)) // EXIT
			{
				waitStick = 2;
				configExit();
			}
			else if(configMode&&(MwRcData[ROLLSTICK]>MAXSTICK)) // MOVE RIGHT
			{
				waitStick = 1;
				COL++;
				if(COL>4) COL=4;
			}
			else if(configMode&&(MwRcData[ROLLSTICK]<MINSTICK)) // MOVE LEFT
			{
				waitStick = 1;
				COL--;
				if(COL<1) COL=1;
			}
			else if(configMode&&(MwRcData[PITCHSTICK]>MAXSTICK)) // MOVE UP
			{
				waitStick = 1;
				ROW--;
				if(ROW<1)
				ROW=1;
			}
			else if(configMode&&(MwRcData[PITCHSTICK]<MINSTICK)) // MOVE DOWN
			{
				waitStick = 1;
				ROW++;
				if(ROW>10)
				ROW=10;
			}
			else if(!previousarmedstatus&&configMode&&(MwRcData[YAWSTICK]<MINSTICK)) // DECREASE
			{
				waitStick = 1;

				if(configPage == 1) {
					if(ROW >= 1 && ROW <= 5) {
						if(COL==1) conf.pid[ROW-1].P8--;
						if(COL==2) conf.pid[ROW-1].I8--;
						if(COL==3) conf.pid[ROW-1].D8--;
					}

					if(ROW == 6) {
						if(COL==1) conf.pid[7].P8--;
						if(COL==2) conf.pid[7].I8--;
						if(COL==3) conf.pid[7].D8--;
					}

					if((ROW==7)&&(COL==1)) conf.pid[8].P8--;
				}

				if(configPage == 2 && COL == 3) {
					if(ROW==1) conf.rcRate8--;
					if(ROW==2) conf.rcExpo8--;
					if(ROW==3) conf.rollPitchRate--;
					if(ROW==4) conf.yawRate--;
					if(ROW==5) conf.dynThrPID--;
				}

				if(configPage == 3 && COL == 3) {
					if(ROW==1) Settings[S_VOLTAGEMIN]--;
					//if(ROW==2) Settings[S_TEMPERATUREMAX]--;
					if(ROW==3) {
						Settings[S_BLINKINGHZ]--;
						if (Settings[S_BLINKINGHZ] <1) Settings[S_BLINKINGHZ]=1;
					}
				}

				if(configPage == 4 && COL == 3) {
					if(ROW==3) rssiTimer=15;
					if(ROW==4) Settings[S_RSSIMAX]=rssiADC;  // set MAX RSSI signal received (tx ON and rx near to tx)
				}

				if(configPage == 5 && COL == 3) {
					if(ROW==1) accCalibrationTimer=0;
					if(ROW==5) magCalibrationTimer=0;
					if(ROW==7) eepromWriteTimer=0;
				}

				if(configPage == 6 && COL == 3) {
					if(ROW==1) Settings[S_DISPLAYGPS]=!Settings[S_DISPLAYGPS];
					if(ROW==2) Settings[S_COORDINATES]=!Settings[S_COORDINATES];
					if(ROW==3) Settings[L_CALLSIGNPOSITIONDSPL]=!Settings[L_CALLSIGNPOSITIONDSPL];
				}

				if(configPage == 7 && COL == 3) {
					if(ROW==1) Settings[S_RESETSTATISTICS]=!Settings[S_RESETSTATISTICS];
					if(ROW==2) Settings[S_HEADING360]=!Settings[S_HEADING360];
					if(ROW==3) Settings[S_UNITSYSTEM]=!Settings[S_UNITSYSTEM];
					if(ROW==4) {
						Settings[S_VIDEOSIGNALTYPE]=!Settings[S_VIDEOSIGNALTYPE];
						MAX7456_Setup();
					}
				}

				//------------------------------ page for screen item change position
				if(configPage == 8) {
					if(ROW == 4) {
						if (COL==1) {
							screenitemselect--;  // Select previous Item
							screen_pos_item_pointer=screen_pos_item_pointer-3; // Point to previous item
							if (screenitemselect <0) {
								screenitemselect=MAXSCREENITEMS;
								screen_pos_item_pointer=EEPROM_ITEM_LOCATION-3; // Point to last item
							}
						}
						if (COL==2) Settings[screen_pos_item_pointer+2]=!Settings[screen_pos_item_pointer+2];  // Display/Hide
						if (COL==3) {
							if ((Settings[screen_pos_item_pointer] > 1) && (Settings[screen_pos_item_pointer] !=255)) Settings[screen_pos_item_pointer]--;  // subtract 1 line
						}
						if (COL==4) {
							if ((Settings[screen_pos_item_pointer+1] > 1) && (Settings[screen_pos_item_pointer+1] !=255)) Settings[screen_pos_item_pointer+1]--;  // subtract 1 column
						}
					}
					if((ROW==7)&&(COL==1)) {
						WriteScreenLayoutDefault(); // Back and save to all default positions
						configExit();  // Exit
					}
				}
				//--------------------------------------------------

				if((ROW==10)&&(COL==3)) configPage--;
				if(configPage<MINPAGE) configPage = MAXPAGE;
				if((ROW==10)&&(COL==1)) configExit();
				if((ROW==10)&&(COL==2)) saveExit();
			}
			else if(!previousarmedstatus&&configMode&&(MwRcData[YAWSTICK]>MAXSTICK)) // INCREASE
			{
				waitStick =1;

				if(configPage == 1) {
					if(ROW >= 1 && ROW <= 5) {
						if(COL==1) conf.pid[ROW-1].P8++;
						if(COL==2) conf.pid[ROW-1].I8++;
						if(COL==3) conf.pid[ROW-1].D8++;
					}

					if(ROW == 6) {
						if(COL==1) conf.pid[7].P8++;
						if(COL==2) conf.pid[7].I8++;
						if(COL==3) conf.pid[7].D8++;
					}

					if((ROW==7)&&(COL==1)) conf.pid[8].P8++;
				}

				if(configPage == 2 && COL == 3) {
					if(ROW==1) conf.rcRate8++;
					if(ROW==2) conf.rcExpo8++;
					if(ROW==3) conf.rollPitchRate++;
					if(ROW==4) conf.yawRate++;
					if(ROW==5) conf.dynThrPID++;
				}

				if(configPage == 3 && COL == 3) {
					if(ROW==1) Settings[S_VOLTAGEMIN]++;
					//if(ROW==2) Settings[S_TEMPERATUREMAX]++;
					if(ROW==3) {
						Settings[S_BLINKINGHZ]++;
						if (Settings[S_BLINKINGHZ] >10) Settings[S_BLINKINGHZ]=10;
					}
				}

				if(configPage == 4 && COL == 3) {
					if(ROW==3) rssiTimer=15;
					if(ROW==4) Settings[S_RSSIMAX]=rssiADC;  // set MAX RSSI signal received (tx ON and rx near to tx)
				}

				if(configPage == 5 && COL == 3) {
					if(ROW==1) accCalibrationTimer=CALIBRATION_DELAY;
					if(ROW==5) magCalibrationTimer=CALIBRATION_DELAY;
					if(ROW==7) eepromWriteTimer=EEPROM_WRITE_DELAY;
				}

				if(configPage == 6 && COL == 3) {
					if(ROW==1) Settings[S_DISPLAYGPS]=!Settings[S_DISPLAYGPS];
					if(ROW==2) Settings[S_COORDINATES]=!Settings[S_COORDINATES];
					if(ROW==3) Settings[L_CALLSIGNPOSITIONDSPL]=!Settings[L_CALLSIGNPOSITIONDSPL];
				}

				if(configPage == 7 && COL == 3) {
					if(ROW==1) Settings[S_RESETSTATISTICS]=!Settings[S_RESETSTATISTICS];
					if(ROW==2) Settings[S_HEADING360]=!Settings[S_HEADING360];
					if(ROW==3) Settings[S_UNITSYSTEM]=!Settings[S_UNITSYSTEM];
					if(ROW==4) {
						Settings[S_VIDEOSIGNALTYPE]=!Settings[S_VIDEOSIGNALTYPE];
						MAX7456_Setup();
					}
				}

				//------------------------------  page for screen item change position
				if(configPage == 8) {
					if(ROW == 4) {
						if (COL==1) {
							screenitemselect++;            // Select next Item
							screen_pos_item_pointer=screen_pos_item_pointer+3; // Point to next item
							if (screenitemselect >MAXSCREENITEMS) {
								screenitemselect=0;
								screen_pos_item_pointer=EEPROM_SETTINGS+1;  // Point to first item in enum
							}
						}
						if (COL==2) Settings[screen_pos_item_pointer+2]=!Settings[screen_pos_item_pointer+2];  // Display/Hide
						if (COL==3) {
							if(Settings[S_VIDEOSIGNALTYPE]){
								if (Settings[screen_pos_item_pointer]  < 15) Settings[screen_pos_item_pointer]++; // add 1 line (Max 15 for PAL)
							}
							else if(Settings[screen_pos_item_pointer]  < 13) Settings[screen_pos_item_pointer]++; // add 1 line (Max 13 for NTSC)
						}
						if (COL==4) {
							if (Settings[screen_pos_item_pointer+1] < 25) Settings[screen_pos_item_pointer+1]++;  // add 1 column
						}
					}
					if((ROW==7)&&(COL==1)) {
							WriteScreenLayoutDefault(); // Back and save to all default positions
							configExit();
					}
				}
				//--------------------------------------------------

				if((ROW==10)&&(COL==3)) configPage++;
				if(configPage>MAXPAGE) configPage = MINPAGE;
				if((ROW==10)&&(COL==1)) configExit();
				if((ROW==10)&&(COL==2)) saveExit();
			}
		}

		if(waitStick == 1)
		stickTime = millis();
	}
}

// --------------------------------------------------------------------------------------
// Here are decoded received commands from MultiWii
void serialMSPCheck()
{
  readIndex = 0;

  if (cmdMSP[0] == MSP_OSD) {
    uint8_t cmd = read8();
    if(cmd == OSD_READ_CMD) {
	  headSerialReply(EEPROM_SETTINGS + 1);
	  serialize8(cmd);
	  for(uint8_t i=0; i<EEPROM_SETTINGS; i++) serialize8(Settings[i]);
	  tailSerialReply();
    }

    if (cmd == OSD_WRITE_CMD) {
      for(uint16_t en=0;en<EEPROM_SETTINGS; en++){
			uint8_t inSetting = read8();
			if (inSetting != Settings[en])
			EEPROM.write(en,inSetting);
			Settings[en] = inSetting;
      }
      readEEPROM();
      setMspRequests();
    }

    if(cmd == OSD_GET_FONT) {
      if(dataSize == 5) {
        if(read16() == 7456) {
          nextCharToRequest = read8();
          lastCharToRequest = read8();
          initFontMode();
      }
      }
      else if(dataSize == 56) {
        //for(uint8_t i = 0; i < 54; i++)
        //  fontData[i] = read8();
		r_struct((uint8_t*)&fontData,54);
      
		uint8_t c = read8();
        MAX7456_writeNVM(c);
	   //fontCharacterReceived(c);
      }
    }
/*    if(cmd == OSD_RESET) {
		//resetFunc(); // not a good practice to do jmp 0 ...
    }
    if(cmd == OSD_SERIAL_SPEED) {
    
    }
  */                  
  }

  if (cmdMSP[0]==MSP_IDENT)
  {
    MW_STATUS.version= read8();                             // MultiWii Firmware version
    modeMSPRequests &=~ REQ_MSP_IDENT;
  }

  if (cmdMSP[0]==MSP_STATUS)
  {
	r_struct((uint8_t*)&MW_STATUS,10);
	armed = (MW_STATUS.sensorActive & mode_armed) != 0;
  }

  if (cmdMSP[0]==MSP_RAW_IMU)
  {
	r_struct((uint8_t*)&MW_IMU,18);
  }

  if (cmdMSP[0]==MSP_RC)
  {
	r_struct((uint8_t*)&MwRcData,16);
    handleRawRC();
  }

  if (cmdMSP[0]==MSP_RAW_GPS)
  {
	r_struct((uint8_t*)&GPS,16);
  }

  if (cmdMSP[0]==MSP_COMP_GPS)
  {
	r_struct((uint8_t*)&GPS.distanceToHome,4);
  }

  if (cmdMSP[0]==MSP_ATTITUDE)
  {
		r_struct((uint8_t*)&MW_ATT,6);
  }

  if (cmdMSP[0]==MSP_ALTITUDE)
  {
	r_struct((uint8_t*)&MW_ALT,8);
  }

  if (cmdMSP[0]==MSP_ANALOG)
  {
	r_struct((uint8_t*)&MW_ANALOG,7);
  }

  if (cmdMSP[0]==MSP_RC_TUNING)
  {
	r_struct((uint8_t*)&conf.rcRate8,7);
    modeMSPRequests &=~ REQ_MSP_RC_TUNING;
  }

  if (cmdMSP[0]==MSP_PID)
  {
	r_struct((uint8_t*)&conf.pid[0].P8,3*PIDITEMS);
    modeMSPRequests &=~ REQ_MSP_PID;
  }

/*if (Settings[S_USE_BOXNAMES]){ 
  if(cmdMSP[0]==MSP_BOXNAMES) {
    uint32_t bit = 1;
    uint8_t remaining = dataSize;
    uint8_t len = 0;
    char firstc, lastc;

    mode_armed = 0;
    mode_stable = 0;
    mode_baro = 0;
    mode_mag = 0;
    mode_gpshome = 0;
    mode_gpshold = 0;
    mode_osd_switch = 0;

    while(remaining > 0) {
      char c = read8();
      if(len == 0)
        firstc = c;
      len++;
      if(c == ';') {
        // Found end of name; set bit if first and last c matches.
        if(firstc == 'A') {
          if(lastc == 'M') // "ARM;"
            mode_armed |= bit;
          if(lastc == 'E') // "ANGLE;"
            mode_stable |= bit;
        }
        //if(firstc == 'H' && lastc == 'N') // "HORIZON;"
          //mode_stable |= bit;
        if(firstc == 'H' && lastc == 'N') // "HORIZON;"
          mode_horizon |= bit;
        if(firstc == 'M' && lastc == 'G') // "MAG;"
           mode_mag |= bit;
        if(firstc == 'B' && lastc == 'O') // "BARO;"
          mode_baro |= bit;
        if(firstc == 'G') {
          if(lastc == 'E') // "GPS HOME;"
            mode_gpshome |= bit;
          if(lastc == 'D') // "GPS HOLD;"
            mode_gpshold |= bit;
            }
          if(firstc == 'O' && lastc == 'W') // "OSD SW;"
            mode_osd_switch |= bit;
            
            len = 0;
             bit <<= 1L;
      }
      lastc = c;
      --remaining;
    }
    modeMSPRequests &=~ REQ_MSP_BOX;
  }
}else*/
{ 
	
// use MSP_BOXIDS
  if(cmdMSP[0]==MSP_BOXIDS) {
    uint32_t bit = 1;
    uint8_t remaining = dataSize;

    mode_armed = 0;
    mode_stable = 0;
    mode_baro = 0;
    mode_mag = 0;
    mode_gpshome = 0;
    mode_gpshold = 0;
    mode_osd_switch = 0;

    while(remaining > 0) {
      char c = read8();
      switch(c) {
      case 0:
        mode_armed |= bit;
        break;
      case 1:
      case 2:
        mode_stable |= bit;
        break;
      case 3:
        mode_baro |= bit;
        break;
      case 5:
        mode_mag |= bit;
        break;
      case 10:
        mode_gpshome |= bit;
        break;
      case 11:
        mode_gpshold |= bit;
        break;
      case 19:
        mode_osd_switch |= bit;
        break;
      }
      bit <<= 1;
      --remaining;
    }
    modeMSPRequests &=~ REQ_MSP_BOX;
  }
}
}

// End of decoded received commands from MultiWii
// --------------------------------------------------------------------------------------


void serialMSPreceive()
{
	uint8_t c;

	static enum _serial_state {
		IDLE,
		HEADER_START,
		HEADER_M,
		HEADER_ARROW,
		HEADER_SIZE,
		HEADER_CMD,
	}
	c_state = IDLE;

	while(SerialAvailable(0))
	{
		c = SerialRead(0);
		//c = read8();

		if (c_state == IDLE)
		{
			c_state = (c=='$') ? HEADER_START : IDLE;
		}
		else if (c_state == HEADER_START)
		{
			c_state = (c=='M') ? HEADER_M : IDLE;
		}
		else if (c_state == HEADER_M)
		{
			c_state = (c=='>') ? HEADER_ARROW : IDLE;
		}
		else if (c_state == HEADER_ARROW)
		{
			if (c > SERIALBUFFERSIZE)
			{  // now we are expecting the payload size
				c_state = IDLE;
			}
			else
			{
				dataSize = c;
				c_state = HEADER_SIZE;
				rcvChecksum = c;
			}
		}
		else if (c_state == HEADER_SIZE)
		{
			c_state = HEADER_CMD;
			cmdMSP[0] = c;
			rcvChecksum ^= c;
			receiverIndex=0;
		}
		else if (c_state == HEADER_CMD)
		{
			rcvChecksum ^= c;
			if(receiverIndex == dataSize) // received checksum byte
			{
				if(rcvChecksum == 0) {
					serialMSPCheck();
				}
				c_state = IDLE;
			}
			else
			serialBuffer[receiverIndex++]=c;
		}
	}
}
