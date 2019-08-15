/*
 * SrvMotors.h
 *
 * Created: 31/05/2012 16:04:46
 *  Author: berryer
 */ 
#ifndef SrvMotors_H_
#define SrvMotors_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"

#include "Drv/DrvPwmSoft.h"


////////////////////////////////////////////MOTORS MIN MAX////////////////////////////////////////////
#define MOTOR_POWER_MIN 0
#define MOTOR_POWER_MAX 1000
#define MOTOR_POWER_MID (MOTOR_POWER_MIN + MOTOR_POWER_MAX) / 2
////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

typedef struct
{
	Int8U	id;
	Int16U	startup;
	Int16U	speed;
	PwmSoft	*servo;
}Motor;

typedef struct
{
	Int16U throttle;
	Int16U minThrottle;
	Int16U midThrottle;
	Int16U maxThrottle;
	#ifdef BI
		Motor right;
		Motor left;
	#endif
	#ifdef QUADX
		Motor frontRight;
		Motor frontLeft;
		Motor rearRight;
		Motor rearLeft;
	#endif
}Motors;


typedef struct
{
	Int16S roulis;
	Int16S tangage;
	Int16S lacet;
	Int16S altitude;
}ErrorPid;

////////////////////////////////////////PUBLIC FONCTIONS/////////////////////////////////////////

//init des moteurs
Boolean SrvMotorsInit( void ) ;

//dispatcher d'evenements
void SrvMotorsDispatcher (void) ;

//get structure of motors
Motors * SrvMotorsGetStruct( void );

//Update des moteurs
void SrvMotorsUpdate(ErrorPid pid_error) ;

//applique une vittesse aux moteurs
Boolean SrvMotorsApplyAbsoluteSpeed(Int16U speed) ;

//applique une vittesse aux moteurs
Boolean SrvMotorsApplyRelativeSpeed(Int16S speed);

//recupere une vittesse des moteurs
Int16U SrvMotorsGetSpeed( void ) ;

//configure la vitesse des moteurs
void SrvMotorsSetSpeed( Int8U motorIndex, Int16U speed );

#endif /* SrvMotors_H_ */