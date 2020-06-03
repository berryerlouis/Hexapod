/*
 * CmpVL53L0X.h
 *
 * Created: 26/05/2020 15:18:42
 *  Author: louis
 */ 


#ifndef CMPVL53L0X_H_
#define CMPVL53L0X_H_



#include "Conf/ConfHard.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
#define VL53L0X_ADDRESS				0x29U ///< I2C address of the VL53L0X module


#define VL53L0X_IDENTIFICATION_MODEL_ID    0xC0U
#define VL53L0X_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV 0x89U
#define VL53L0X_SYSRANGE_START				0x00U
#define VL53L0X_RESULT_RANGE_STATUS      0x14U
#define VL53L0X_SYSTEM_INTERRUPT_CLEAR		0x08U
////////////////////////////////////////PUBLIC STRUCTURES////////////////////////////////////////


////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean CmpVL53L0XInit( void );
Int16U CmpVL53L0XGetDistance( void );



#endif /* CMPVL53L0X_H_ */