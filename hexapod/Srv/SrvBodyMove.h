/*
 * SrvBodyMove.h
 *
 * Created: 18/04/2020 14:35:38
 *  Author: louis
 */ 
#ifndef SRVBODYMOVE_H_
#define SRVBODYMOVE_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////
typedef enum
{
	E_BODY_STOP,
	E_BODY_STAR,
	E_BODY_STRAIGHT,
	E_BODY_UNKNOWNN,
} EBodyBehavior;

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct 
{
	EBodyBehavior behavior;
	float elevation;
	float groundSize;
	float roll;
	float pitch;
	float yaw;
	float x;
	float y;
	float z;
	Boolean initialized;
}SBodyMove;
////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvBodyMoveInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvBodyMoveUpdate (void) ;
SBodyMove *SrvBodyMoveGetStruct( void );
//Set Position
Boolean SrvBodyMoveSetBehavior ( EBodyBehavior pos, uint16_t delay );
//Get Position
EBodyBehavior SrvBodyMoveGetBehavior ( void );
//Set vertical Rotation
Boolean SrvBodyMoveSetRotationAndTranslation ( float roll, float pitch, float yaw, float x, float y, float z, uint16_t delay );
//Set Ground Size 
Boolean SrvBodyMoveSetGroundSize ( float groundSize, uint16_t delay );
//Set Elevation
Boolean SrvBodyMoveSetElevation ( float elevation, uint16_t delay );

#endif //SRVBODYMOVE_H_