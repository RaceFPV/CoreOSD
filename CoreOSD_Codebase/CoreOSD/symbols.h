#ifndef SYMBOLS_H_
#define SYMBOLS_H_


// Satellite Graphics
#define SYM_SAT 0XCE
#define SYM_SAT_1 0XCF

// Degrees Icon for HEADING/DIRECTION HOME
#define SYM_DEGREES 0XBC

// Direction arrows
#define SYM_ARROW_SOUTH 0X60
#define SYM_ARROW_SOUTH_1 0X61
#define SYM_HEADING_POINTER 0X99

// GPS Coordinates and Altitude
#define SYM_LAT 0xCA
#define SYM_LON 0XCB
#define SYM_GPS 0XA3

// Altitude
#define SYM_ALT 0XCC

// GPS Mode
#define SYM_3DFIX 0X9D
#define SYM_HOLD 0X8A
#define SYM_G_HOME 0X9A

// SensorÂ´s Presence
#define SYM_ACC 0X02
#define SYM_MAG 0X12
#define SYM_BAR 0X13
#define SYM_GPS 0X03
#define SYM_CHECK 0XBD

// SensorÂ´s Active
#define SYM_ACC_ON 0XA0
#define SYM_MAG_ON 0XA1
#define SYM_BAR_ON 0XA2
#define SYM_GPS_ON 0XA3
#define SYM_GPS_SEARCH 0XA3

// AH Center screen Graphics
#define SYM_AH_CENTER 0XC5
#define SYM_AH_RIGHT 0XC6
#define SYM_AH_LEFT 0XC7
#define SYM_AH_CENTER_LINE 0XC4     // No line (moved to 0XC3)
#define SYM_AH_DECORATION_RIGHT 0XB9
#define SYM_AH_DECORATION_LEFT 0XBA
#define SYM_AH_SIDE_L_TOP 0X02
#define SYM_AH_SIDE_R_TOP 0X03
#define SYM_AH_SIDE_BAR_L 0X11
#define SYM_AH_SIDE_BAR_R 0X01
#define SYM_AH_SIDE_L_BOT 0X12
#define SYM_AH_SIDE_R_BOT 0X13

// AH Bars
#define SYM_AH_BAR_0 0x80
#define SYM_AH_BAR_9 0x89

//AH Side Bars
#define SYM_AH_UP_15DG_L 0X15
#define SYM_AH_UP_5_10DG_L 0X05
#define SYM_AH_UP_5_10DG_R 0X04
#define SYM_AH_UP_15DG_R 0X14
#define SYM_AH_DOWN_15DG_L 0XEF
#define SYM_AH_DOWN_5_10DG_L 0XDF
#define SYM_AH_DOWN_5_10DG_R 0XDE
#define SYM_AH_DOWN_15DG_R 0XEE


// Climb Rate Arrows
#define SYM_POS_CLIMB 0XB0
#define SYM_POS_CLIMB1 0XB1
#define SYM_POS_CLIMB2 0XB2
#define SYM_POS_CLIMB3 0XB3
#define SYM_NEG_CLIMB3 0XB4
#define SYM_NEG_CLIMB2 0XB5
#define SYM_NEG_CLIMB1 0XB6
#define SYM_NEG_CLIMB 0XB7
#define SYM_ZERO_CLIMB 0XB8

// VS Gadget
/*#define SYM_L_TOP_CLIMB 0X81
#define SYM_L_3UP_CLIMB 0X82
#define SYM_L_2UP_CLIMB 0X83
#define SYM_L_1UP_CLIMB 0X84
#define SYM_L_ZERO_CLIMB 0X85
#define SYM_L_1DOWN_CLIMB 0X86
#define SYM_L_2DOWN_CLIMB 0X87
#define SYM_L_3DOWN_CLIMB 0X88
#define SYM_L_BOT_CLIMB 0X88

// VS Scale Gadget
#define SYM_VS_SCALE_HIGH 0X8E
#define SYM_VS_SCALE_MID 0X9E
#define SYM_VS_SCALE_LOW 0X8F*/

// Temperature           //DO NOT REMOVE
/*#define SYM_TEMP_F 0XC1
#define SYM_TEMP_C 0XC2*/

// Batt evolution
#define SYM_BATT_FULL 0X90
#define SYM_BATT_5 0X91
#define SYM_BATT_4 0X92
#define SYM_BATT_3 0X93
#define SYM_BATT_2 0X94
#define SYM_BATT_1 0X95
#define SYM_BATT_EMPTY 0X96

// Batt IconÂ´s
#define SYM_MAIN_BATT 0X97
#define SYM_VID_BAT 0X98

// Voltage and amperage
#define SYM_VOLTS 0XC9
#define SYM_AMPS 0XC8
//#define SYM_CURRENT 0XA4
#define SYM_CURRENT 0XBA

// Flying Mode
#define SYM_ACRO 0XAE
#define SYM_ACRO_1 0XAF
#define SYM_ANGLE 0XAC
#define SYM_ANGLE_1 0XAD
#define SYM_HORIZON 0XA9
#define SYM_HORIZON_1 0XAA
#define SYM_HORIZON_2 0xAB

// Time
#define SYM_ON 0X9B
#define SYM_FLY 0X9C

// Throttle
#define SYM_THR_POINTER_DIV_HIGH 0X0A
#define SYM_THR_POINTER_DIV_LOW 0X0B
#define SYM_THR_POINTER_TOP 0X09
#define SYM_THR_POINTER_4 0X08
#define SYM_THR_POINTER_3 0X1B
#define SYM_THR_POINTER_2 0X1A
#define SYM_THR_POINTER_1 0X19
#define SYM_THR_POINTER_BOT 0X18
#define SYM_THR_SCALE_LOW 0X07
#define SYM_THR_SCALE_MED 0X17
#define SYM_THR_SCALE_MAX 0X16

// Stall Warning
#define SYM_THR_STALL 0XBE
#define SYM_THR_STALL1 0XBF

//Warn sign
#define SYM_WARN 0XA5
#define SYM_WARN_1 0XA6
#define SYM_WARN_2 0XA7

// RSSI
#define SYM_RSSI 0XBB

// Menu cursor
#define SYM_CURSOR 0XCD

//Autopilot
#define SYM_AUTOPILOT 0X1C
#define SYM_AUTOPILOT_1 0X1D
#define SYM_AUTOPILOT_2 0X1E
#define SYM_AUTOPILOT_3 0X1F

//Armed/Disarmed
#define SYM_MOTOR_DIS 0X0C
#define SYM_MOTOR_AR 0X0D
#define SYM_MOTOR_ME 0X0E
#define SYM_MOTOR_D 0X0F

//LOS
#define SYM_LOS 0X8B

//SPEED
#define SYM_SPEED 0X8C
#define SYM_SPEED_1 0X8D

//Vertical Speed
#define SYM_VSPEED 0X9F



#endif /* SYMBOLS_H_ */
