/*
 * SrvWalk.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVWALK_H_
#define SRVWALK_H_

#include "../Conf/ConfHard.h"
#include "../Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////
 
typedef enum
{
	E_GAITS_TRIPOD,
	E_GAITS_WAVE,
	E_GAITS_RIPPLE,
}E_GAIT;

typedef enum
{
	E_WALK_FW,
	E_WALK_RV,
	E_WALK_STOP,
	E_WALK_UNKNOWN,
}E_WALK;

#define GAITS_TRIPOD_STEPS	2U
#define GAITS_WAVE_STEPS	6U
#define GAITS_RIPPLE_STEPS	6U

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

//struct of legs, 
typedef struct
{
	E_GAIT gaiting;
	E_WALK walking;
	Int16U walkingDelay;
	Int16U walkingAmplitude;
	Int8S  direction;
	
}SWalk;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvWalkInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvWalkUpdate (void) ;

SWalk* SrvWalkgetStruct (void) ;

Boolean SrvWalkSetWalk( E_GAIT gait, E_WALK walk,Int8U amplitude, Int8S direction, uint16_t delay );
Boolean SrvWalkStopWalk( void );
#endif //SRVWALK_H_