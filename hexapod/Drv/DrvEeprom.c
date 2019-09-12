////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"

#include "DrvEeprom.h"
#include "DrvBootload.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define ADDR_EEPROM_CHECK_EEPROM	( Int8U *)0U
#define ADDR_EEPROM_VERSION_SOFT	( Int8U *)1U
#define ADDR_EEPROM_VERSION_HARD	( Int8U *)2U
#define ADDR_EEPROM_BOOTLOADER		( Int8U *)3U


#define EEPROM_WRITE_ENABLE			TRUE


////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////



////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean DrvEepromInit ( void )
{
	Boolean oSuccess = TRUE;
	
	//return from boot with OK
	if( DrvEepromIsConfigured() == FALSE ) 
	{
		//on ecrit la version software
		DrvEepromWriteByte(ADDR_EEPROM_VERSION_SOFT, VERSION_SOFTWARE);
		//on ecrit la version hardware
		DrvEepromWriteByte(ADDR_EEPROM_VERSION_HARD, VERSION_HARDWARE);
	}

	return oSuccess;
}

//ecrit l'etat de config de l'eeprom
void DrvEepromDeconfigure ( void )
{	
	DrvEepromWriteByte(ADDR_EEPROM_CHECK_EEPROM, EEPROM_DEFAULT_VALUE);
}

//ecrit l'etat de config de l'eeprom
void DrvEepromSetRunMode ( eepromRunMode configuration )
{
	DrvEepromWriteByte(ADDR_EEPROM_BOOTLOADER,configuration);
}

//retourne l'etat de config de l'eeprom
eepromRunMode DrvEepromGetRunMode ( void )
{
	return DrvEepromReadByte( ADDR_EEPROM_BOOTLOADER );
}

//retourne l'etat de config de l'eeprom
Boolean DrvEepromIsConfigured ( void )
{
	return DrvEepromReadByte( ADDR_EEPROM_CHECK_EEPROM ) == EEPROM_DEFAULT_VALUE ? FALSE : TRUE ;
}

//l'eeprom est maintenant configur�e, on place l'octet � une valeur differente de celle par defaut
void DrvEepromSetConfiguration ( void )
{
	//configure is now ok
	DrvEepromWriteByte(ADDR_EEPROM_CHECK_EEPROM,EEPROM_WRITE_ENABLE);
}

//retourne le numero de version
void DrvEepromReadVersion ( Int8U *version )
{
	version[ 0U ] = DrvEepromReadByte( ADDR_EEPROM_VERSION_SOFT );
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//Fonction de lecture eeprom
Int8U DrvEepromReadByte ( const Int8U * addr )
{
	return eeprom_read_byte(addr);
}

//Fonction d ecriture eeprom
void DrvEepromWriteByte ( Int8U * addr, Int8U byte)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_byte(addr,byte);
	}
}

//Fonction de lecture eeprom
Int16U DrvEepromReadShort( const Int8U * addr )
{
	Int16U ret = 0;
	ret = eeprom_read_byte(addr) ;
	ret = ret << 8U ;
	ret |= eeprom_read_byte((addr + 1));
	return ret;
}
	
//Fonction d ecriture eeprom
void DrvEepromWriteShort ( Int8U *addr, Int16U byte)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_byte((addr), (Int8U)(byte >> 8U) );
		eeprom_write_byte((addr + 1) ,(Int8U)(byte));
	}
}

//Fonction de lecture eeprom
float DrvEepromReadFloat( const Int8U * addr )
{
	return eeprom_read_float((float*)addr);
}
	
//Fonction d ecriture eeprom
void DrvEepromWriteFloat ( Int8U *addr, float value)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_float((float*)(addr), value );
	}
}
