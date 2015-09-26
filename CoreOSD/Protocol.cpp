#include "Protocol.h"
#include "Config.h"
#include "EEPROM.h"
#include "Max7456.h"
#include "CoreOSD.h"
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
	if(fontMode|| !safeMode())
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
  if (cmdMSP[0]==MSP_RAW_IMU)
  {
	r_struct((uint8_t*)&MW_IMU,18);
  }
  if (cmdMSP[0]==MSP_ANALOG)
  {
	r_struct((uint8_t*)&MW_ANALOG,7);
  }
}
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
