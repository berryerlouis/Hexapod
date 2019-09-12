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
//Fonction de lecture eeprom
Int8U DrvEepromReadByte ( const Int8U * addr );
//Fonction d ecriture eeprom
void DrvEepromWriteByte ( Int8U * addr, Int8U byte);
//Fonction de lecture eeprom
Int16U DrvEepromReadShort( const Int8U * addr );
//Fonction d ecriture eeprom
void DrvEepromWriteShort ( Int8U *addr, Int16U byte);
//Fonction de lecture eeprom
float DrvEepromReadFloat( const Int8U * addr );
//enregistre les config du pid
void DrvEepromWriteFloat ( Int8U *addr, float value);
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



#endif /* DRV_EEPROM_H_ */