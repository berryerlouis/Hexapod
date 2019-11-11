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


#define LEG_AMPLITUDE_COCYX		30 
#define LEG_AMPLITUDE_FEMUR		80 
#define LEG_AMPLITUDE_TIBIA		70 


#define LEG_COXA_U_L_MID	6
#define LEG_COXA_U_L_MIN	LEG_COXA_U_L_MID - LEG_AMPLITUDE_COCYX
#define LEG_COXA_U_L_MAX	LEG_COXA_U_L_MID + LEG_AMPLITUDE_COCYX
#define LEG_FEMUR_U_L_MID	10
#define LEG_FEMUR_U_L_MIN	LEG_FEMUR_U_L_MID - LEG_AMPLITUDE_FEMUR
#define LEG_FEMUR_U_L_MAX	LEG_FEMUR_U_L_MID + LEG_AMPLITUDE_FEMUR
#define LEG_TIBIA_U_L_MID	0
#define LEG_TIBIA_U_L_MIN	LEG_TIBIA_U_L_MID - LEG_AMPLITUDE_TIBIA
#define LEG_TIBIA_U_L_MAX	LEG_TIBIA_U_L_MID + LEG_AMPLITUDE_TIBIA

#define LEG_COXA_M_L_MID	12
#define LEG_COXA_M_L_MIN	LEG_COXA_M_L_MID - LEG_AMPLITUDE_COCYX
#define LEG_COXA_M_L_MAX	LEG_COXA_M_L_MID + LEG_AMPLITUDE_COCYX
#define LEG_FEMUR_M_L_MID	3
#define LEG_FEMUR_M_L_MIN	LEG_FEMUR_M_L_MID - LEG_AMPLITUDE_FEMUR
#define LEG_FEMUR_M_L_MAX	LEG_FEMUR_M_L_MID + LEG_AMPLITUDE_FEMUR
#define LEG_TIBIA_M_L_MID	-3
#define LEG_TIBIA_M_L_MIN	LEG_TIBIA_M_L_MID - LEG_AMPLITUDE_TIBIA
#define LEG_TIBIA_M_L_MAX	LEG_TIBIA_M_L_MID + LEG_AMPLITUDE_TIBIA

#define LEG_COXA_B_L_MID	10
#define LEG_COXA_B_L_MIN	LEG_COXA_B_L_MID - LEG_AMPLITUDE_COCYX
#define LEG_COXA_B_L_MAX	LEG_COXA_B_L_MID + LEG_AMPLITUDE_COCYX
#define LEG_FEMUR_B_L_MID	5
#define LEG_FEMUR_B_L_MIN	LEG_FEMUR_B_L_MID - LEG_AMPLITUDE_FEMUR
#define LEG_FEMUR_B_L_MAX	LEG_FEMUR_B_L_MID + LEG_AMPLITUDE_FEMUR
#define LEG_TIBIA_B_L_MID	-10
#define LEG_TIBIA_B_L_MIN	LEG_TIBIA_B_L_MID - LEG_AMPLITUDE_TIBIA
#define LEG_TIBIA_B_L_MAX	LEG_TIBIA_B_L_MID + LEG_AMPLITUDE_TIBIA

#define LEG_COXA_U_R_MID	5
#define LEG_COXA_U_R_MIN	LEG_COXA_U_R_MID - LEG_AMPLITUDE_COCYX
#define LEG_COXA_U_R_MAX	LEG_COXA_U_R_MID + LEG_AMPLITUDE_COCYX
#define LEG_FEMUR_U_R_MID	9
#define LEG_FEMUR_U_R_MIN	LEG_FEMUR_U_R_MID - LEG_AMPLITUDE_FEMUR
#define LEG_FEMUR_U_R_MAX	LEG_FEMUR_U_R_MID + LEG_AMPLITUDE_FEMUR
#define LEG_TIBIA_U_R_MID	15
#define LEG_TIBIA_U_R_MIN	LEG_TIBIA_U_R_MID - LEG_AMPLITUDE_TIBIA
#define LEG_TIBIA_U_R_MAX	LEG_TIBIA_U_R_MID + LEG_AMPLITUDE_TIBIA

#define LEG_COXA_M_R_MID	8
#define LEG_COXA_M_R_MIN	LEG_COXA_M_R_MID - LEG_AMPLITUDE_COCYX
#define LEG_COXA_M_R_MAX	LEG_COXA_M_R_MID + LEG_AMPLITUDE_COCYX
#define LEG_FEMUR_M_R_MID	10
#define LEG_FEMUR_M_R_MIN	LEG_FEMUR_M_R_MID - LEG_AMPLITUDE_FEMUR
#define LEG_FEMUR_M_R_MAX	LEG_FEMUR_M_R_MID + LEG_AMPLITUDE_FEMUR
#define LEG_TIBIA_M_R_MID	20
#define LEG_TIBIA_M_R_MIN	LEG_TIBIA_M_R_MID - LEG_AMPLITUDE_TIBIA
#define LEG_TIBIA_M_R_MAX	LEG_TIBIA_M_R_MID + LEG_AMPLITUDE_TIBIA

#define LEG_COXA_B_R_MID	3
#define LEG_COXA_B_R_MIN	LEG_COXA_B_R_MID - LEG_AMPLITUDE_COCYX
#define LEG_COXA_B_R_MAX	LEG_COXA_B_R_MID + LEG_AMPLITUDE_COCYX
#define LEG_FEMUR_B_R_MID	11
#define LEG_FEMUR_B_R_MIN	LEG_FEMUR_B_R_MID - LEG_AMPLITUDE_FEMUR
#define LEG_FEMUR_B_R_MAX	LEG_FEMUR_B_R_MID + LEG_AMPLITUDE_FEMUR
#define LEG_TIBIA_B_R_MID	15
#define LEG_TIBIA_B_R_MIN	LEG_TIBIA_B_R_MID - LEG_AMPLITUDE_TIBIA
#define LEG_TIBIA_B_R_MAX	LEG_TIBIA_B_R_MID + LEG_AMPLITUDE_TIBIA
/////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////////
typedef enum
{
	E_LEG_U_L,
	E_LEG_M_L,
	E_LEG_B_L,
	E_LEG_U_R,
	E_LEG_M_R,
	E_LEG_B_R,
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
}SLeg;

typedef struct
{
	Boolean isInitializing;
	SLeg leg [NB_LEGS];
}SLegs;

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean DrvLegInit( void );
Boolean DrvLegUpdate( void );
SLegs* DrvLegGetLegs( void );
Boolean DrvLegIsInitialized( void );
SLeg* DrvLegGetLeg( E_LEG indexLeg );
Boolean DrvLegSetXYZ( E_LEG indexLeg, Int16S x, Int16S y, Int16S z, Int16U speed );
Boolean DrvLegSetPosition( E_LEG indexLeg, Int16S coxaAngle, Int16S femurAngle, Int16S tibiaAngle, Int16U speed );
Boolean DrvLegCheckTarget( E_LEG indexLeg );


#endif /* DRVLEG_H_ */