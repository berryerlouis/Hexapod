/*
 * SrvBattery.h
 *
 * Created: 27/07/2017 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVDETECTION_H_
#define SRVDETECTION_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"
#include "Cmps/CmpSRF04.h"
#include "Cmps/CmpVL53L0X.h"


////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUM///////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
#define US_THRESHOLD_DISTANCE		30U		// cm
#define LAZER_THRESHOLD_DISTANCE	300U	// mm

typedef void (*SrvDetectionNotification) (Boolean reachThreshold);
////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
//struct of detection, 
typedef struct
{
	Int16U usThreshold;
	Int16U lazerThreshold;
	Int16U distance [E_NB_DIST_SENSORS];
	Boolean detect[E_NB_DIST_SENSORS];
	SrvDetectionNotification notification;
}SDetection;


////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvDetectionInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvDetectionUpdate (void) ;

SDetection* SrvDetectionGetStruct( void );

Int16U SrvDetectionGetThresholdUS( void );
void SrvDetectionSetThresholdUS( Int16U usThreshold );
Int16U SrvDetectionGetDistanceUS(E_US us);


Int16U SrvDetectionGetThresholdLazer( void );
void SrvDetectionSetThresholdLazer( Int16U lazerThreshold );
Int16U SrvDetectionGetDistanceLazer(E_LAZER lazer);


void SrvDetectionSetNotification (SrvDetectionNotification cb) ;

#endif //SRVDETECTION_H_