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

//length in cm of leg's member
#define LEG_COCYX_LENGTH		60		//horizontal
#define LEG_FEMUR_LENGTH		85		//vertical
#define LEG_TIBIA_LENGTH		120		//knee


#define LEG_COCYX_AMPLITUDE			40		
#define LEG_COCYX_AMPLITUDE_DIV_2	LEG_COCYX_AMPLITUDE / 2		
#define LEG_FEMUR_AMPLITUDE			90		
#define LEG_FEMUR_AMPLITUDE_DIV_2	LEG_FEMUR_AMPLITUDE / 2		
#define LEG_TIBIA_AMPLITUDE			140		
#define LEG_TIBIA_AMPLITUDE_DIV_2	LEG_TIBIA_AMPLITUDE / 2		

//leg upper left
#define LEG_COCYX_U_L_OFT	-3
#define LEG_COCYX_U_L_MIN	LEG_COCYX_U_L_OFT //
#define LEG_COCYX_U_L_MID	LEG_COCYX_U_L_OFT + LEG_COCYX_AMPLITUDE_DIV_2
#define LEG_COCYX_U_L_MAX	LEG_COCYX_U_L_OFT + LEG_COCYX_AMPLITUDE

#define LEG_FEMUR_U_L_OFT	2
#define LEG_FEMUR_U_L_MIN	LEG_FEMUR_U_L_OFT - LEG_FEMUR_AMPLITUDE
#define LEG_FEMUR_U_L_MID	LEG_FEMUR_U_L_OFT - LEG_FEMUR_AMPLITUDE_DIV_2
#define LEG_FEMUR_U_L_MAX	LEG_FEMUR_U_L_OFT //

#define LEG_TIBIA_U_L_OFT	90
#define LEG_TIBIA_U_L_MIN	LEG_TIBIA_U_L_OFT - LEG_TIBIA_AMPLITUDE
#define LEG_TIBIA_U_L_MID	LEG_TIBIA_U_L_OFT - LEG_TIBIA_AMPLITUDE_DIV_2
#define LEG_TIBIA_U_L_MAX	LEG_TIBIA_U_L_OFT //

//leg middle left
#define LEG_COCYX_M_L_OFT	5
#define LEG_COCYX_M_L_MIN	LEG_COCYX_M_L_OFT - LEG_COCYX_AMPLITUDE_DIV_2
#define LEG_COCYX_M_L_MID	LEG_COCYX_M_L_OFT //
#define LEG_COCYX_M_L_MAX	LEG_COCYX_M_L_OFT + LEG_COCYX_AMPLITUDE_DIV_2

#define LEG_FEMUR_M_L_OFT	-5
#define LEG_FEMUR_M_L_MIN	LEG_FEMUR_M_L_OFT - LEG_FEMUR_AMPLITUDE
#define LEG_FEMUR_M_L_MID	LEG_FEMUR_M_L_OFT - LEG_FEMUR_AMPLITUDE_DIV_2
#define LEG_FEMUR_M_L_MAX	LEG_FEMUR_M_L_OFT //

#define LEG_TIBIA_M_L_OFT	90
#define LEG_TIBIA_M_L_MIN	LEG_TIBIA_M_L_OFT - LEG_TIBIA_AMPLITUDE
#define LEG_TIBIA_M_L_MID	LEG_TIBIA_M_L_OFT - LEG_TIBIA_AMPLITUDE_DIV_2
#define LEG_TIBIA_M_L_MAX	LEG_TIBIA_M_L_OFT //

//leg bottom left
#define LEG_COCYX_B_L_OFT	-2
#define LEG_COCYX_B_L_MIN	LEG_COCYX_B_L_OFT - LEG_COCYX_AMPLITUDE
#define LEG_COCYX_B_L_MID	LEG_COCYX_B_L_OFT - LEG_COCYX_AMPLITUDE_DIV_2
#define LEG_COCYX_B_L_MAX	LEG_COCYX_B_L_OFT //

#define LEG_FEMUR_B_L_OFT	0
#define LEG_FEMUR_B_L_MIN	LEG_FEMUR_B_L_OFT - LEG_FEMUR_AMPLITUDE
#define LEG_FEMUR_B_L_MID	LEG_FEMUR_B_L_OFT - LEG_FEMUR_AMPLITUDE_DIV_2
#define LEG_FEMUR_B_L_MAX	LEG_FEMUR_B_L_OFT //

#define LEG_TIBIA_B_L_OFT	90
#define LEG_TIBIA_B_L_MIN	LEG_TIBIA_B_L_OFT - LEG_TIBIA_AMPLITUDE
#define LEG_TIBIA_B_L_MID	LEG_TIBIA_B_L_OFT - LEG_TIBIA_AMPLITUDE_DIV_2
#define LEG_TIBIA_B_L_MAX	LEG_TIBIA_B_L_OFT //

//leg upper right
#define LEG_COCYX_U_R_OFT	-12
#define LEG_COCYX_U_R_MIN	LEG_COCYX_U_R_OFT - LEG_COCYX_AMPLITUDE
#define LEG_COCYX_U_R_MID	LEG_COCYX_U_R_OFT - LEG_COCYX_AMPLITUDE_DIV_2
#define LEG_COCYX_U_R_MAX	LEG_COCYX_U_R_OFT//

#define LEG_FEMUR_U_R_OFT	-4
#define LEG_FEMUR_U_R_MIN	LEG_FEMUR_U_R_OFT//
#define LEG_FEMUR_U_R_MID	LEG_FEMUR_U_R_OFT + LEG_FEMUR_AMPLITUDE_DIV_2
#define LEG_FEMUR_U_R_MAX	LEG_FEMUR_U_R_OFT + LEG_FEMUR_AMPLITUDE

#define LEG_TIBIA_U_R_OFT	-80
#define LEG_TIBIA_U_R_MIN	LEG_TIBIA_U_R_OFT//
#define LEG_TIBIA_U_R_MID	LEG_TIBIA_U_R_OFT + LEG_TIBIA_AMPLITUDE_DIV_2
#define LEG_TIBIA_U_R_MAX	LEG_TIBIA_U_R_OFT + LEG_TIBIA_AMPLITUDE

//leg middle right
#define LEG_COCYX_M_R_OFT	-8
#define LEG_COCYX_M_R_MIN	LEG_COCYX_M_R_OFT - LEG_COCYX_AMPLITUDE_DIV_2
#define LEG_COCYX_M_R_MID	LEG_COCYX_M_R_OFT//
#define LEG_COCYX_M_R_MAX	LEG_COCYX_M_R_OFT + LEG_COCYX_AMPLITUDE_DIV_2

#define LEG_FEMUR_M_R_OFT	-3
#define LEG_FEMUR_M_R_MIN	LEG_FEMUR_M_R_OFT//
#define LEG_FEMUR_M_R_MID	LEG_FEMUR_M_R_OFT + LEG_FEMUR_AMPLITUDE_DIV_2
#define LEG_FEMUR_M_R_MAX	LEG_FEMUR_M_R_OFT + LEG_FEMUR_AMPLITUDE

#define LEG_TIBIA_M_R_OFT	-76
#define LEG_TIBIA_M_R_MIN	LEG_TIBIA_M_R_OFT//
#define LEG_TIBIA_M_R_MID	LEG_TIBIA_M_R_OFT + LEG_TIBIA_AMPLITUDE_DIV_2
#define LEG_TIBIA_M_R_MAX	LEG_TIBIA_M_R_OFT + LEG_TIBIA_AMPLITUDE

//leg bottom right
#define LEG_COCYX_B_R_OFT	-16
#define LEG_COCYX_B_R_MIN	LEG_COCYX_B_R_OFT//
#define LEG_COCYX_B_R_MID	LEG_COCYX_B_R_OFT + LEG_COCYX_AMPLITUDE_DIV_2
#define LEG_COCYX_B_R_MAX	LEG_COCYX_B_R_OFT + LEG_COCYX_AMPLITUDE

#define LEG_FEMUR_B_R_OFT	-4
#define LEG_FEMUR_B_R_MIN	LEG_FEMUR_B_R_OFT//
#define LEG_FEMUR_B_R_MID	LEG_FEMUR_B_R_OFT + LEG_FEMUR_AMPLITUDE_DIV_2
#define LEG_FEMUR_B_R_MAX	LEG_FEMUR_B_R_OFT + LEG_FEMUR_AMPLITUDE

#define LEG_TIBIA_B_R_OFT	-86
#define LEG_TIBIA_B_R_MIN	LEG_TIBIA_B_R_OFT//
#define LEG_TIBIA_B_R_MID	LEG_TIBIA_B_R_OFT + LEG_TIBIA_AMPLITUDE_DIV_2
#define LEG_TIBIA_B_R_MAX	LEG_TIBIA_B_R_OFT + LEG_TIBIA_AMPLITUDE
/////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////////

//enum of each leg of the hexapod
typedef enum
{
	E_LEG_U_L,
	E_LEG_M_L,
	E_LEG_B_L,
	E_LEG_U_R,
	E_LEG_M_R,
	E_LEG_B_R,
	//nb legs
	E_NB_LEGS
}ELeg;


/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////

//struct of leg
typedef struct
{
	int8_t id;
	Boolean enable;
	Boolean initialized;
	SServo *coxaAngle;
	SServo *femurAngle;
	SServo *tibiaAngle;
	float currentPositionX;
	float currentPositionY;
	float currentPositionZ;
	float targetPositionX;
	float targetPositionY;
	float targetPositionZ;
	float startPositionX;
	float startPositionY;
	float startPositionZ;
	Int32U startTime;
	Int32U speed;
	E_SERVO_EASES coxaEase;
	E_SERVO_EASES femurEase;
	E_SERVO_EASES tibiaEase;
}SLeg;

//struct of legs, 
typedef struct
{
	Boolean isInitialized;
	SLeg leg [NB_LEGS];
}SLegs;

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean DrvLegInit( void );
Boolean DrvLegUpdate( void );
SLegs* DrvLegGetLegs( void );
Boolean DrvLegIsInitialized( void );
SLeg* DrvLegGetStruct( ELeg indexLeg );
Boolean DrvLegGetXYZ( ELeg indexLeg, float *x, float *y, float *z );
Boolean DrvLegSetPosition( ELeg indexLeg, Int16S coxaAngle, Int16S femurAngle, Int16S tibiaAngle, Int16U speed );
Boolean DrvLegCheckTarget( ELeg indexLeg );
Boolean DrvLegSetTarget( ELeg indexLeg, float x, float y, float z, Int16U speed );
float DrvLegGetXFromCoxaAngle( float angle , float y);

#endif /* DRVLEG_H_ */