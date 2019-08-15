/*
 * CmpBMP085.h
 *
 * Created: 03/07/2012 13:49:01
 *  Author: berryer
 */ 


#ifndef BMP085_H_
#define BMP085_H_


#include "Conf/ConfHard.h"
#include "Snrs/SnrBarometer.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
//TWI address
#define BMP085_ADDRESS		0x77
// Registers
#define CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define CAL_AC3           0xAE  // R   Calibration data (16 bits)
#define CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define CAL_B1            0xB6  // R   Calibration data (16 bits)
#define CAL_B2            0xB8  // R   Calibration data (16 bits)
#define CAL_MB            0xBA  // R   Calibration data (16 bits)
#define CAL_MC            0xBC  // R   Calibration data (16 bits)
#define CAL_MD            0xBE  // R   Calibration data (16 bits)
#define CONTROL           0xF4  // W   Control register
#define CONTROL_OUTPUT    0xF6  // R   Output registers 0xF6=MSB, 0xF7=LSB, 0xF8=XLSB
// Control register
#define READ_TEMPERATURE        0x2E
#define READ_PRESSURE           0x34


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
//-----------------------------------------------------------------------------------------------//



//definition de la structure des fonctions du baromter pour le composant BMP085
extern BarometerFunctions bmp085;
//definition de la structure des data du baromter pour le composant BMP085
extern BarometerData bmp085Data;
////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////


#endif /*BMA180_H_*/




