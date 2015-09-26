#include "Arduino.h"
#include "KV_Team_OSD.h"
#include "Max7456.h"



// Selectable by board type
uint8_t MAX7456SELECT;                // output pin
uint8_t MAX7456RESET;                 // output pin

// Selectable by video mode
//uint8_t ENABLE_display;
//uint8_t ENABLE_display_vert;
//uint8_t DISABLE_display;
uint16_t MAX_screen_size;
uint8_t MAX7456_reset;
uint8_t MAX_screen_rows;


volatile uint8_t vsync_wait = 0;


//////////////////////////////////////////////////////////////

uint8_t spi_transfer(uint8_t data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
    ;
  return SPDR;                    // return the received byte
}

void MAX7456_Send(uint8_t add, uint8_t data)
{
	spi_transfer(add);
	spi_transfer(data);
}

ISR(INT0_vect) {
	vsync_wait = 0;
}


void MAX7456Configure() {
	// todo - automatically recognising card.
	if(Settings[S_BOARDTYPE] == 0) {        // Rush
		MAX7456SELECT = 10;      // ss
		MAX7456RESET  = 9;       // RESET
	}

	if(Settings[S_BOARDTYPE] == 1) {        // MinimOSD
		MAX7456SELECT = 6;       // ss
		MAX7456RESET = 10;       // RESET
	}
}


void MAX7456_Setup(void)
{

  MAX7456Configure();

  if(Settings[S_VIDEOSIGNALTYPE]) {    // PAL
    //ENABLE_display = 0x48;
    //ENABLE_display_vert = 0x4c;
    MAX7456_reset = 0x42;
    //DISABLE_display = 0x40;
    MAX_screen_size = 480;
    MAX_screen_rows = 16;
  }
  else {                                // NTSC
    //ENABLE_display = 0x08;
    //ENABLE_display_vert = 0x0c;
    MAX7456_reset = 0x02;
    //DISABLE_display = 0x00;
    MAX_screen_size = 390;
    MAX_screen_rows = 13;
  }

  pinMode(MAX7456RESET,OUTPUT);
  digitalWrite(MAX7456RESET,HIGH); //hard enable

  delay(250);

  pinMode(MAX7456SELECT,OUTPUT);
  digitalWrite(MAX7456SELECT,HIGH); //disable device

  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(VSYNC, INPUT);
  
  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (4 meg)

  SPCR = (1<<SPE)|(1<<MSTR);
  SPSR=(1<<SPI2X);
  uint8_t spi_junk;
  spi_junk=SPSR;
  spi_junk=SPDR;
  delay(250);

  // force soft reset on Max7456
  digitalWrite(MAX7456SELECT,LOW);
  MAX7456_Send(VM0_reg, MAX7456_reset);
  digitalWrite(MAX7456SELECT,HIGH);
  delay(500);

  // set all rows to same character white level, 90%
  digitalWrite(MAX7456SELECT,LOW);
  
  uint8_t x;
  for(x = 0; x < MAX_screen_rows; x++) {
    MAX7456_Send(MAX7456ADD_RB0+x, WHITE_level_120);
  }

  // make sure the Max7456 is enabled
  spi_transfer(VM0_reg);

  if (Settings[S_VIDEOSIGNALTYPE]){
    spi_transfer(OSD_ENABLE|VIDEO_MODE_PAL);
  }
  else{
    spi_transfer(OSD_ENABLE|VIDEO_MODE_NTSC);
  }
  digitalWrite(MAX7456SELECT,HIGH);
  delay(100);
  
  EIMSK |= (1 << INT0);  // enable interuppt
  EICRA |= (1 << ISC01); // interrupt at the falling edge
  sei();
}


// Copy string from ram into screen buffer
void MAX7456_WriteString(const char *string, int Adresse)
{
	uint8_t xx;
	for(xx=0;string[xx]!=0;)
	{
		screen[Adresse++] = string[xx++];
	}
}

// Copy string from progmem into the screen buffer
void MAX7456_WriteString_P(const char *string, int Adresse)
{
	uint8_t xx = 0;
	char c;
	while((c = (char)pgm_read_byte(&string[xx++])) != 0)
	{
		screen[Adresse++] = c;
	}
}

void MAX7456_writeNVM(uint8_t char_address)
{
	#ifdef WRITE_TO_MAX7456
	// disable display
	digitalWrite(MAX7456SELECT,LOW);
	spi_transfer(VM0_reg);
	//spi_transfer(DISABLE_display);

	
	
	//digitalWrite(MAX7456SELECT,LOW);
	//spi_transfer(VM0_reg);
	spi_transfer(Settings[S_VIDEOSIGNALTYPE]?0x40:0);

	spi_transfer(MAX7456ADD_CMAH); // set start address high
	spi_transfer(char_address);

	for(uint8_t x = 0; x < NVM_ram_size; x++) // write out 54 bytes of character to shadow ram
	{
		spi_transfer(MAX7456ADD_CMAL); // set start address low
		spi_transfer(x);
		spi_transfer(MAX7456ADD_CMDI);
		spi_transfer(fontData[x]);
	}

	// transfer 54 bytes from shadow ram to NVM
	spi_transfer(MAX7456ADD_CMM);
	spi_transfer(WRITE_nvr);
	
	// wait until bit 5 in the status register returns to 0 (12ms)
	while ((spi_transfer(MAX7456ADD_STAT) & STATUS_reg_nvr_busy) != 0x00);

	spi_transfer(VM0_reg); // turn on screen next vertical
	//spi_transfer(ENABLE_display_vert);
	spi_transfer(Settings[S_VIDEOSIGNALTYPE]?0x4c:0x0c);
	digitalWrite(MAX7456SELECT,HIGH);
	#else
	delay(12);
	#endif
}
// ============================================================   WRITE TO SCREEN
void MAX7456_DrawScreen()
{
  vsync_wait = 1;
  while (vsync_wait)
  ;
  
  int xx;
  digitalWrite(MAX7456SELECT,LOW);
  
  spi_transfer(DMM_reg);  
  spi_transfer(1); 

  spi_transfer(DMAH_reg);  
  spi_transfer(0); 

  spi_transfer(DMAL_reg);  
  spi_transfer(0); 


  for(xx=0;xx<MAX_screen_size;++xx)
  {
    MAX7456_Send(MAX7456ADD_DMDI, screen[xx]);
    screen[xx] = ' ';
  }

  spi_transfer(DMDI_reg); 
  spi_transfer(END_string); 

  spi_transfer(DMM_reg);  
  spi_transfer(B00000000);
  
  digitalWrite(MAX7456SELECT,HIGH);
}

