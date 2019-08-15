/*
 * SrvPID.h
 *
 * Created: 24/09/2012 14:48:50
 *  Author: berryer
 */ 


#ifndef SRVPID_H_
#define SRVPID_H_



#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	float p, i, d;
	float lastPosition;
	float integratedError;
}Pid;



////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
//init des valeurs du PID
void SrvPIDInit( void ) ;

//Dispatcher d'evenements 
void SrvPIDDispatcher (void) ;

//get the struct of the using PID
Pid **SrvPIDGetStruct ( Int8U index );

//set des valeurs du PID
void SrvPIDSetValues( Int8U index, float p, float i, float d ) ;

//get des valeurs du PID
void SrvPIDGetValues( Int8U index, float *p, float *i, float *d ) ;

//Reset des valeurs du pid
void SrvPIDResetValues( void ) ;

//calcul des valeurs PID
Int16S SrvPIDCompute(Int8U index, Int16S currentPosition, Int16S targetPosition, float delta_time ) ;


#endif /* SRVPID_H_ */