/*
 * BMA180.h
 *
 * Created: 03/07/2012 13:49:01
 *  Author: berryer
 */ 


#ifndef BMA180_H_
#define BMA180_H_


#include "Conf/conf_hard.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
#define BMA180_ADDRESS		0x40

#define BMA180_CHIP_ID		0x03

#define BMA180_ACC_1G		256

#define BMA180_REG_CHIP_ID		0x00
#define BMA180_REG_ACC_X_LSB	0x02
#define BMA180_REG_ACC_X_MSB	0x03
#define BMA180_REG_ACC_Y_LSB	0x04
#define BMA180_REG_ACC_Y_MSB	0x05
#define BMA180_REG_ACC_Z_LSB	0x06
#define BMA180_REG_ACC_Z_MSB	0x07
#define BMA180_REG_CTRL_REG0	0x0D
#define BMA180_REG_BW_TCS		0x20
#define BMA180_REG_TC0_Z		0x30
#define BMA180_REG_OFFSET_LSB1	0x35

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int16S x;
	Int16S y;
	Int16S z;
}AccelerometerRawData;

typedef struct
{
	float roulis;
	float tangage;
}Accelerometer;



////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////



//fonction init du capteur
Boolean CmpBMA180Init(void);
//Acceleration X Y Z
Boolean CmpBMA180GetAcceleration(AccelerometerRawData *acc);
Boolean CmpBMA180IsCalibrate(void);

#endif /*BMA180_H_*/
