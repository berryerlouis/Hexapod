////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"

#include "DrvEeprom.h"
#include "DrvBootload.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define ADDR_EEPROM_CHECK_EEPROM	( Int8U *)0U
#define ADDR_EEPROM_VERSION_SOFT	( Int8U *)1U
#define ADDR_EEPROM_VERSION_HARD	( Int8U *)2U
#define ADDR_EEPROM_ACC_CALIB_X		( Int8U *)3U
#define ADDR_EEPROM_ACC_CALIB_Y		( Int8U *)5U
#define ADDR_EEPROM_ACC_CALIB_Z		( Int8U *)7U
#define ADDR_EEPROM_GYR_CALIB_X		( Int8U *)9U
#define ADDR_EEPROM_GYR_CALIB_Y		( Int8U *)11U
#define ADDR_EEPROM_GYR_CALIB_Z		( Int8U *)13U
#define ADDR_EEPROM_MAG_CALIB_X		( Int8U *)15U
#define ADDR_EEPROM_MAG_CALIB_Y		( Int8U *)17U
#define ADDR_EEPROM_MAG_CALIB_Z		( Int8U *)19U
#define ADDR_EEPROM_PID_X_P			( Int8U *)21U
#define ADDR_EEPROM_PID_X_I			( Int8U *)25U
#define ADDR_EEPROM_PID_X_D			( Int8U *)29U
#define ADDR_EEPROM_PID_Y_P			( Int8U *)33U
#define ADDR_EEPROM_PID_Y_I			( Int8U *)37U
#define ADDR_EEPROM_PID_Y_D			( Int8U *)41U
#define ADDR_EEPROM_PID_Z_P			( Int8U *)45U
#define ADDR_EEPROM_PID_Z_I			( Int8U *)49U
#define ADDR_EEPROM_PID_Z_D			( Int8U *)53U
#define ADDR_EEPROM_PID_ALT_P		( Int8U *)57U
#define ADDR_EEPROM_PID_ALT_I		( Int8U *)61U
#define ADDR_EEPROM_PID_ALT_D		( Int8U *)65U
#define ADDR_EEPROM_ALTITUDE		( Int8U *)67U
#define ADDR_EEPROM_MOTOR_F_L		( Int8U *)71U
#define ADDR_EEPROM_MOTOR_F_R		( Int8U *)73U
#define ADDR_EEPROM_MOTOR_R_L		( Int8U *)75U
#define ADDR_EEPROM_MOTOR_R_R		( Int8U *)77U
#define ADDR_EEPROM_APP_BOOT		( Int8U *)79U


#define EEPROM_WRITE_ENABLE			TRUE


////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//Fonction de lecture eeprom
static Int8U DrvEepromReadByte ( const Int8U * addr );
//Fonction d ecriture eeprom
static void DrvEepromWriteByte ( Int8U * addr, Int8U byte);
//Fonction de lecture eeprom
static Int16U DrvEepromReadShort( const Int8U * addr );
//Fonction d ecriture eeprom
static void DrvEepromWriteShort ( Int8U *addr, Int16U byte);
//Fonction de lecture eeprom
static float DrvEepromReadFloat( const Int8U * addr );
//enregistre les config du pid
static void DrvEepromWriteFloat ( Int8U *addr, float value);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////



////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean DrvEepromInit ( void )
{
	Boolean oSuccess = TRUE;
	
	//return from boot with OK
	if( DrvEepromIsConfigured() != TRUE ) 
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
	for(Int16U loop = 0U ; loop < 1/*E2SIZE*/ ; loop++)
	{
		DrvEepromWriteByte(( Int8U *)loop,EEPROM_DEFAULT_VALUE);
	}
}

//ecrit l'etat de config de l'eeprom
void DrvEepromSetRunMode ( eepromRunMode configuration )
{
	DrvEepromWriteByte(ADDR_EEPROM_APP_BOOT,configuration);
}

//retourne l'etat de config de l'eeprom
eepromRunMode DrvEepromGetRunMode ( void )
{
	return DrvEepromReadByte( ADDR_EEPROM_APP_BOOT );
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

//retourne les config de l'accelerometre
void DrvEepromReadAcc ( Int16S calib[ 3U ] )
{
	calib[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_ACC_CALIB_X);
	calib[ 1U ] = DrvEepromReadShort(ADDR_EEPROM_ACC_CALIB_Y);
	calib[ 2U ] = DrvEepromReadShort(ADDR_EEPROM_ACC_CALIB_Z);
}

//enregistre les config de l'accelerometre
void DrvEepromWriteAcc ( Int16S calib[ 3U ] )
{
	DrvEepromWriteShort(ADDR_EEPROM_ACC_CALIB_X,calib[ 0U ]);
	DrvEepromWriteShort(ADDR_EEPROM_ACC_CALIB_Y,calib[ 1U ]);
	DrvEepromWriteShort(ADDR_EEPROM_ACC_CALIB_Z,calib[ 2U ]);
}

//retourne les config du gyroscope
void DrvEepromReadGyr ( Int16S calib[ 3U ] )
{
	calib[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_GYR_CALIB_X);
	calib[ 1U ] = DrvEepromReadShort(ADDR_EEPROM_GYR_CALIB_Y);
	calib[ 2U ] = DrvEepromReadShort(ADDR_EEPROM_GYR_CALIB_Z);
}

//enregistre les config du gyroscope
void DrvEepromWriteGyr ( Int16S calib[ 3U ] )
{
	DrvEepromWriteShort(ADDR_EEPROM_GYR_CALIB_X,calib[ 0U ]);
	DrvEepromWriteShort(ADDR_EEPROM_GYR_CALIB_Y,calib[ 1U ]);
	DrvEepromWriteShort(ADDR_EEPROM_GYR_CALIB_Z,calib[ 2U ]);
}


//retourne les config du magnetometer
void DrvEepromReadMag ( Int16S calib[ 3U ] )
{
	calib[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MAG_CALIB_X);
	calib[ 1U ] = DrvEepromReadShort(ADDR_EEPROM_MAG_CALIB_Y);
	calib[ 2U ] = DrvEepromReadShort(ADDR_EEPROM_MAG_CALIB_Z);
}

//enregistre les config du magnetometer
void DrvEepromWriteMag ( Int16S calib[ 3U ] )
{
	DrvEepromWriteShort(ADDR_EEPROM_MAG_CALIB_X,calib[ 0U ]);
	DrvEepromWriteShort(ADDR_EEPROM_MAG_CALIB_Y,calib[ 1U ]);
	DrvEepromWriteShort(ADDR_EEPROM_MAG_CALIB_Z,calib[ 2U ]);
}


//retourne les config du pid
void DrvEepromReadPID(Int8U index,float *P, float *I, float *D)
{
	if( index == 0 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_X_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_X_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_X_D);
	}
	else if( index == 1 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_Y_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_Y_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_Y_D);
	}
	else if( index == 2 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_Z_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_Z_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_Z_D);
	}
	else if( index == 3 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_ALT_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_ALT_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_ALT_D);
	}
}

//enregistre les config du pid
void DrvEepromWritePID (Int8U index,float P, float I, float D)
{
	if( index == 0 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_X_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_X_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_X_D, D);
	}
	else if( index == 1 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Y_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Y_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Y_D, D);
	}
	else if( index == 2 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Z_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Z_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Z_D, D);
	}
	else if( index == 3 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_ALT_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_ALT_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_ALT_D, D);
	}
}


//retourne les config du altitude
void DrvEepromReadAltitude( Int16S *altitude )
{
	altitude[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_ALTITUDE);
}

//enregistre les config du altitude
void DrvEepromWriteAltitude ( Int16S altitude )
{
	DrvEepromWriteShort(ADDR_EEPROM_ALTITUDE, altitude);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorFrontRight( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_F_R);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorFrontRight ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_F_R, startup);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorFrontLeft( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_F_L);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorFrontLeft ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_F_L, startup);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorRearRight( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_R_R);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorRearRight ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_R_R, startup);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorRearLeft( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_R_L);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorRearLeft ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_R_L, startup);
}
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//Fonction de lecture eeprom
static Int8U DrvEepromReadByte ( const Int8U * addr )
{
	return eeprom_read_byte(addr);
}

//Fonction d ecriture eeprom
static void DrvEepromWriteByte ( Int8U * addr, Int8U byte)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_byte(addr,byte);
	}
}

//Fonction de lecture eeprom
static Int16U DrvEepromReadShort( const Int8U * addr )
{
	Int16U ret = 0;
	ret = eeprom_read_byte(addr) ;
	ret = ret << 8U ;
	ret |= eeprom_read_byte((addr + 1));
	return ret;
}
	
//Fonction d ecriture eeprom
static void DrvEepromWriteShort ( Int8U *addr, Int16U byte)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_byte((addr), (Int8U)(byte >> 8U) );
		eeprom_write_byte((addr + 1) ,(Int8U)(byte));
	}
}

//Fonction de lecture eeprom
static float DrvEepromReadFloat( const Int8U * addr )
{
	return eeprom_read_float((float*)addr);
}
	
//Fonction d ecriture eeprom
static void DrvEepromWriteFloat ( Int8U *addr, float value)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_float((float*)(addr), value );
	}
}
