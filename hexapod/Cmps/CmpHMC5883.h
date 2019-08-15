/*
 * HMC5883.h
 *
 * Created: 03/07/2012 13:49:01
 *  Author: berryer
 */ 


#ifndef HMC5883_H_
#define HMC5883_H_


#include "Conf/ConfHard.h"
#include "Snrs/SnrMagnetometer.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////


#define HMC5883_ADDRESS			0x1E

//Values Config A
#define HMC5883_0_75HZ			0x00
#define HMC5883_1_5HZ			0x01
#define HMC5883_3HZ				0x02
#define HMC5883_7_5HZ			0x03
#define HMC5883_15HZ			0x04
#define HMC5883_30HZ			0x05
#define HMC5883_75HZ			0x06


#define HMC5883_1_SAMPLE		0x00
#define HMC5883_2_SAMPLE		0x01
#define HMC5883_4_SAMPLE		0x02
#define HMC5883_8_SAMPLE		0x03

#define HMC5883_NORMAL_BIAS		0x00
#define HMC5883_POSITIVE_BIAS	0x01
#define HMC5883_NEGATIVE_BIAS	0x02

//Values Config B
#define HMC5883_0_88GA			0x00
#define HMC5883_1_3GA			0x01
#define HMC5883_1_9GA			0x02
#define HMC5883_2_5GA			0x03
#define HMC5883_4_0GA			0x04
#define HMC5883_4_7GA			0x05
#define HMC5883_5_6GA			0x06
#define HMC5883_8_1GA			0x07
//Values MODE
#define HMC5883_CONTINUOUS   0x00
#define HMC5883_SINGLE	     0x01

#define HMC5883_CONFIG_A     0x00
#define HMC5883_CONFIG_B     0x01
#define HMC5883_MODE         0x02
#define HMC5883_X_MSB        0x03
#define HMC5883_X_LSB        0x04
#define HMC5883_Z_MSB        0x05
#define HMC5883_Z_LSB        0x06
#define HMC5883_Y_MSB        0x07
#define HMC5883_Y_LSB        0x08
#define HMC5883_STATUS       0x09
#define HMC5883_IDENT_A      0x0A
#define HMC5883_IDENT_B      0x0B
#define HMC5883_IDENT_C      0x0C

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


//definition de la structure des fonctions du magnetometer pour le composant HMC5883
extern MagnetometerFunctions hmc5883;
//definition de la structure des data magnetometer pour le composant HMC5883
extern MagnetometerData hmc5883Data;

////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////


#endif /*HMC5883_H_*/
