/*
 * DrvEeprom.h
 *
 * Created: 14/11/2011 15:18:48
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#ifndef DRV_EEPROM_H_
#define DRV_EEPROM_H_

#include "../Tools/tools_typedefs.h"

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
#define EEPROM_DEFAULT_VALUE	0xFFU
////////////////////////////////////////PUBLIC ENUMS//////////////////////////////////////////////
typedef enum
{
	VAL_EEPROM_CHECK_RUN_APP	= 0x00U,
	VAL_EEPROM_CHECK_RUN_BOOT	= 0xFFU,
}eepromRunMode;
////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean DrvEepromInit ( void ) ;

//ecrit l'etat de config de l'eeprom
void DrvEepromDeconfigure ( void ) ;

//retourne l'etat de config de l'eeprom
Boolean DrvEepromIsConfigured ( void ) ;

//l'eeprom est maintenant configur�e, on place l'octet � une valeur differente de celle par defaut
void DrvEepromSetConfiguration ( void );

//ecrit l'etat de config de l'eeprom
void DrvEepromSetRunMode ( eepromRunMode configuration );

//retourne l'etat de config de l'eeprom
eepromRunMode DrvEepromGetRunMode ( void );

//retourne le numero de version
void DrvEepromReadVersion ( Int8U *verison );

//retourne les config de l'accelerometre
void DrvEepromReadAcc ( Int16S calib[ 3U ] );

//enregistre les config de l'accelerometre
void DrvEepromWriteAcc ( Int16S calib[ 3U ] );

//retourne les config du gyroscope
void DrvEepromReadGyr ( Int16S calib[ 3U ] );

//enregistre les config du gyroscope
void DrvEepromWriteGyr ( Int16S calib[ 3U ] );

//retourne les config du magnetometer
void DrvEepromReadMag ( Int16S calib[ 3U ] );

//enregistre les config du magnetometer
void DrvEepromWriteMag ( Int16S calib[ 3U ] );

//retourne les config du pid
void DrvEepromReadPID(Int8U index,float *P, float *I, float *D);

//enregistre les config du pid
void DrvEepromWritePID (Int8U index,float P, float I, float D);

//retourne les config du altitude
void DrvEepromReadAltitude( Int16S *altitude);

//enregistre les config du altitude
void DrvEepromWriteAltitude ( Int16S altitude);

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorFrontRight( Int16U *startup);

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorFrontRight ( Int16U startup);

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorFrontLeft( Int16U *startup);

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorFrontLeft ( Int16U startup);

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorRearRight( Int16U *startup);

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorRearRight ( Int16U startup);

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorRearLeft( Int16U *startup);

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorRearLeft ( Int16U startup);

#endif /* DRV_EEPROM_H_ */