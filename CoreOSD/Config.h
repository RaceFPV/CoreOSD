 #ifndef CONFIG_H_
 #define CONFIG_H_
 
 /*--------------------------------------------------       Configurable parameters      ----------------------------------------------------*/ 
  // Variables
 #define METRIC 0
 #define IMPERIAL 1
 
 //Analog input defines
 // **** WiteSpy hardware **** //
 #define voltagePin A0
 #define amperagePin A1
 #define rssiPin A3
 #define PWMrssiPin A3           // PWM RSSI uses same pin of analog RSSI A3
 
/**********************          Serial port Speed       *************************/

#define SPEED 115200 // 115200,57600,38400,28800,19200,14400,9600

/**********    Here you can define time out for Mag calibration and EEProm write (mostly useful for mag calibration)    ***********/
#define CALIBRATION_DELAY 6
#define EEPROM_WRITE_DELAY 3      

/***********************         Display Settings         ************************/

#define DECIMAL '.'                 // Decimal point character, change to what suits you best (.) (,)

/*----------------------------------------------       End of configurable parameters      ----------------------------------------------------*/

#define ACCELEROMETER  1//0b00000001


#define LINE      30
#define LINE01    0
#define LINE02    30
#define LINE03    60
#define LINE04    90
#define LINE05    120
#define LINE06    150
#define LINE07    180
#define LINE08    210
#define LINE09    240
#define LINE10    270
#define LINE11    300
#define LINE12    330
#define LINE13    360
#define LINE14    390
#define LINE15    420
#define LINE16    450


#endif /* CONFIG_H_ */

