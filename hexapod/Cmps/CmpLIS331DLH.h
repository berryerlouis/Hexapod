/*
 * CmpLIS331DLH.h
 *
 * Created: 11/02/2014 12:00:20
 *  Author: berryer
 */ 


#ifndef CMPLIS331DLH_H_
#define CMPLIS331DLH_H_

#include "Conf/ConfHard.h"
#include "Snrs/SnrAccelerometer.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
#define LIS331DLH_ADDRESS						0x18

#define LIS331DLH_WHO_AM_I					0x0F
#define LIS331DLH_CTRL_REG1					0x20
	#define LIS331DLH_CTRL_REG1_XEN				0x01
	#define LIS331DLH_CTRL_REG1_YEN				0x02
	#define LIS331DLH_CTRL_REG1_ZEN				0x04
	#define LIS331DLH_CTRL_REG1_DR0				0x08
	#define LIS331DLH_CTRL_REG1_DR1				0x10
	#define LIS331DLH_CTRL_REG1_PM0				0x20
	#define LIS331DLH_CTRL_REG1_PM1				0x40
	#define LIS331DLH_CTRL_REG1_PM2				0x80
#define LIS331DLH_CTRL_REG2					0x21
#define LIS331DLH_CTRL_REG3					0x22
#define LIS331DLH_CTRL_REG4					0x23
	#define LIS331DLH_CTRL_REG4_SIM				0x01
	#define LIS331DLH_CTRL_REG4_ST				0x02
	#define LIS331DLH_CTRL_REG4_DUMMY			0x04
	#define LIS331DLH_CTRL_REG4_STsign			0x08
	#define LIS331DLH_CTRL_REG4_FS0				0x10
	#define LIS331DLH_CTRL_REG4_FS1				0x20
	#define LIS331DLH_CTRL_REG4_BLE				0x40
	#define LIS331DLH_CTRL_REG4_BDU				0x80
#define LIS331DLH_CTRL_REG5					0x24
#define LIS331DLH_HP_FILTER_RESET			0x25
#define LIS331DLH_REFERENCE					0x26
#define LIS331DLH_STATUS_REG				0x27
#define LIS331DLH_OUT_X_L					0x28
#define LIS331DLH_OUT_X_H					0x29
#define LIS331DLH_OUT_Y_L					0x2A
#define LIS331DLH_OUT_Y_H					0x2B
#define LIS331DLH_OUT_Z_L					0x2C
#define LIS331DLH_OUT_Z_H					0x2D


#define LIS331DLH_CTRL_REG4_FS_1G_2G		0x00
#define LIS331DLH_CTRL_REG4_FS_1G_4G		0x01
#define LIS331DLH_CTRL_REG4_FS_1G_8G		0x11

#define LIS331DLH_WHO_I_AM					0x32
#define LIS331DLH_ACC_1G_2G					0x3FFF
#define LIS331DLH_ACC_1G_4G					0x1FFF
#define LIS331DLH_ACC_1G_8G					0x0FFF


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


//definition de la structure des fonctions de accelerometer pour le composant LIS331DLH
extern AccelerometerFunctions lis331dlh;
//definition de la structure des data accelerometer pour le composant LIS331DLH
extern AccelerometerData lis331dlhData;
////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

#endif /* CMPLIS331DLH_H_ */