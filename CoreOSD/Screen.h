#ifndef SCREEN_H_
#define SCREEN_H_
#include "Arduino.h"

	void displayHorizon(int16_t rollAngle, int16_t pitchAngle);
	void displayVoltage(void);
	void displayTime(void);
	void displayAmperage(void);
	void displaypMeterSum(void);
	void displayRSSI(void);
		
#endif /* SCREEN_H_ */
