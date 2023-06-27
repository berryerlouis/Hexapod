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
#define	NB_SERVOS_PER_LEG		3U

//length in cm of leg's member
#define LEG_COCYX_LENGTH		29		//in mm 
#define LEG_FEMUR_LENGTH		85		//in mm 
#define LEG_TIBIA_LENGTH		125		//in mm 

#define LEG_TIBIA_ANGLE_OFFSET  100		//13°

#define LEG_COCYX_AMPLITUDE			500		
#define LEG_COCYX_AMPLITUDE_DIV_2	LEG_COCYX_AMPLITUDE / 2		
#define LEG_FEMUR_AMPLITUDE			900		
#define LEG_FEMUR_AMPLITUDE_DIV_2	LEG_FEMUR_AMPLITUDE / 2		
#define LEG_TIBIA_AMPLITUDE			1400		
#define LEG_TIBIA_AMPLITUDE_DIV_2	LEG_TIBIA_AMPLITUDE / 2		

//leg upper left
#define LEG_COCYX_U_L_OFT	350
#define LEG_FEMUR_U_L_OFT	20 - LEG_TIBIA_ANGLE_OFFSET
#define LEG_TIBIA_U_L_OFT	900

//leg middle left
#define LEG_COCYX_M_L_OFT	70
#define LEG_FEMUR_M_L_OFT	-30 - LEG_TIBIA_ANGLE_OFFSET
#define LEG_TIBIA_M_L_OFT	900

//leg bottom left
#define LEG_COCYX_B_L_OFT	-20
#define LEG_FEMUR_B_L_OFT	-30 - LEG_TIBIA_ANGLE_OFFSET
#define LEG_TIBIA_B_L_OFT	980

//leg upper right
#define LEG_COCYX_U_R_OFT	-120
#define LEG_FEMUR_U_R_OFT	-40 + LEG_TIBIA_ANGLE_OFFSET
#define LEG_TIBIA_U_R_OFT	-800

//leg middle right
#define LEG_COCYX_M_R_OFT	-80
#define LEG_FEMUR_M_R_OFT	-30 + LEG_TIBIA_ANGLE_OFFSET
#define LEG_TIBIA_M_R_OFT	-760

//leg bottom right
#define LEG_COCYX_B_R_OFT	-160
#define LEG_FEMUR_B_R_OFT	-40 + LEG_TIBIA_ANGLE_OFFSET
#define LEG_TIBIA_B_R_OFT	-860
/////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////////

//enum of each leg of the hexapod
typedef enum
{
	E_LEG_F_L,
	E_LEG_M_L,
	E_LEG_R_L,
	E_LEG_F_R,
	E_LEG_M_R,
	E_LEG_R_R,
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
	Boolean newPosition;
	SServo *coxaAngle;
	SServo *femurAngle;
	SServo *tibiaAngle;
	AxisFloat startPosition;
	AxisFloat targetPosition;
	Int32U startTime;
	Int32U speed;
}SLeg;

//struct of legs, 
typedef struct
{
	Boolean isInitialized;
	SLeg leg [E_NB_LEGS];
}SLegs;

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean DrvLegInit( void );
Boolean DrvLegUpdate( void );
SLegs* DrvLegGetLegs( void );
Boolean DrvLegIsInitialized( void );
SLeg* DrvLegGetStruct( ELeg indexLeg );
Boolean DrvLegSetSpeed( ELeg indexLeg, Int16U speed  );
AxisFloat DrvLegGetXYZ( ELeg indexLeg );
Boolean DrvLegSetTarget( ELeg indexLeg, AxisFloat posotion, Int16U speed );
Boolean DrvLegReachPosition( ELeg indexLeg );
float DrvLegGetXFromCoxaAngle( float angle , float y);

#endif /* DRVLEG_H_ */