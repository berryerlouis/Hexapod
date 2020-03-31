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
#define LEG_COCYX_LENGTH		30U		//horizontal
#define LEG_FEMUR_LENGTH		85U		//vertical
#define LEG_TIBIA_LENGTH		125U	//knee

//leg upper left
#define LEG_COCYX_U_L_MIN	6 //
#define LEG_COCYX_U_L_MID	21
#define LEG_COCYX_U_L_MAX	36
#define LEG_FEMUR_U_L_MIN	-80
#define LEG_FEMUR_U_L_MID	-35
#define LEG_FEMUR_U_L_MAX	10 //
#define LEG_TIBIA_U_L_MIN	-68
#define LEG_TIBIA_U_L_MID	2
#define LEG_TIBIA_U_L_MAX	72 //

//leg middle left
#define LEG_COCYX_M_L_MIN	-3
#define LEG_COCYX_M_L_MID	12 //
#define LEG_COCYX_M_L_MAX	27
#define LEG_FEMUR_M_L_MIN	-87
#define LEG_FEMUR_M_L_MID	-42
#define LEG_FEMUR_M_L_MAX	3 //
#define LEG_TIBIA_M_L_MIN	-70
#define LEG_TIBIA_M_L_MID	0
#define LEG_TIBIA_M_L_MAX	70 //

//leg bottom letf
#define LEG_COCYX_B_L_MIN	-20
#define LEG_COCYX_B_L_MID	-5
#define LEG_COCYX_B_L_MAX	10 //
#define LEG_FEMUR_B_L_MIN	83
#define LEG_FEMUR_B_L_MID	-38
#define LEG_FEMUR_B_L_MAX	7 //
#define LEG_TIBIA_B_L_MIN	-71
#define LEG_TIBIA_B_L_MID	-1
#define LEG_TIBIA_B_L_MAX	69 //

//leg upper right
#define LEG_COCYX_U_R_MIN	-25
#define LEG_COCYX_U_R_MID	-10
#define LEG_COCYX_U_R_MAX	5//
#define LEG_FEMUR_U_R_MIN	9//
#define LEG_FEMUR_U_R_MID	54
#define LEG_FEMUR_U_R_MAX	99
#define LEG_TIBIA_U_R_MIN	-59//
#define LEG_TIBIA_U_R_MID	11
#define LEG_TIBIA_U_R_MAX	81

//leg middle right
#define LEG_COCYX_M_R_MIN	-8
#define LEG_COCYX_M_R_MID	8//
#define LEG_COCYX_M_R_MAX	23
#define LEG_FEMUR_M_R_MIN	10//
#define LEG_FEMUR_M_R_MID	55
#define LEG_FEMUR_M_R_MAX	100
#define LEG_TIBIA_M_R_MIN	-55//
#define LEG_TIBIA_M_R_MID	15
#define LEG_TIBIA_M_R_MAX	85

//leg bottom right
#define LEG_COCYX_B_R_MIN	3//
#define LEG_COCYX_B_R_MID	18
#define LEG_COCYX_B_R_MAX	23
#define LEG_FEMUR_B_R_MIN	11//
#define LEG_FEMUR_B_R_MID	56
#define LEG_FEMUR_B_R_MAX	101
#define LEG_TIBIA_B_R_MIN	-60//
#define LEG_TIBIA_B_R_MID	10
#define LEG_TIBIA_B_R_MAX	80
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
	int16_t x;
	int16_t y;
	int16_t z;
}SLeg;

//struct of legs, 
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
SLeg* DrvLegGetLeg( ELeg indexLeg );
Boolean DrvLegSetXYZ( ELeg indexLeg, Int16S x, Int16S y, Int16S z, Int16U speed );
Boolean DrvLegGetXYZ( ELeg indexLeg, Int16S *x, Int16S *y, Int16S *z );
Boolean DrvLegSetPosition( ELeg indexLeg, Int16S coxaAngle, Int16S femurAngle, Int16S tibiaAngle, Int16U speed );
Boolean DrvLegCheckTarget( ELeg indexLeg );


#endif /* DRVLEG_H_ */