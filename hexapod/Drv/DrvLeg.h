/*
 * DrvLeg.h
 *
 * Created: 18/02/2018 21:59:54
 *  Author: berryer
 */ 


#ifndef DRVLEG_H_
#define DRVLEG_H_

#include "DrvServo.h"

/////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////////
#define	NB_LEGS					6U
#define	NB_SERVOS_PER_LEG		3U


#define LEG_COCYX_LENGHT		30U		//horizontal
#define LEG_FEMUR_LENGHT		75U		//vertical
#define LEG_TIBIA_LENGHT		125U	//knee


#define LEG_OFFSET_ANGLE		30

/////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////////
typedef enum
{
	E_LEG_L_F,
	E_LEG_L_M,
	E_LEG_L_R,
	E_LEG_R_F,
	E_LEG_R_M,
	E_LEG_R_R,
	E_NB_LEGS
}E_LEG;

typedef enum
{
	E_LEG_COXA,
	E_LEG_FEMUR,
	E_LEG_TIBIA,
}E_LEG_SERVOS;

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int8U id;
	SServo *coxaAngle;
	SServo *femurAngle;
	SServo *tibiaAngle;
}S_Leg;


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean DrvLegInit( void );
Boolean DrvLegUpdate( void );
S_Leg* DrvLegGetLeg( E_LEG indexLeg );
Boolean DrvLegSetXYZ( E_LEG indexLeg, Int16S x, Int16S y, Int16S z, Int16U speed );
Boolean DrvLegCheckTarget( E_LEG indexLeg );


#endif /* DRVLEG_H_ */