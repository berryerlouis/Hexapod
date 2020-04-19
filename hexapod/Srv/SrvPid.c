/*
 * SrvPID.c
 *
 * Created: 24/09/2012 14:48:34
 *  Author: berryer
 */ 
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/ConfHard.h"
#include "Srv/SrvPID.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define E_NB_PIDS 2U
////////////////////////////////////////PRIVATE STRUCTURES///////////////////////////////////////

////////////////////////////////////////PRIVATE FONCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static Pid pids[ E_NB_PIDS ] ;


//Init des valeurs du pid
void SrvPIDInit( void )
{	
	SrvPIDResetValues( ) ;
}

//get the struct of the using PID
Pid **SrvPIDGetStruct ( Int8U index )
{
	return (Pid **)&pids[index];
}

//Set des valeurs du pid
void SrvPIDSetValues( Int8U index, float p, float i, float d )
{
	pids[index].p = p ;
	pids[index].i = i ;
	pids[index].d = d ;
}


//get des valeurs du PID
void SrvPIDGetValues( Int8U index, float *p, float *i, float *d ) 
{
	*p = pids[index].p ;
	*i = pids[index].i ;
	*d = pids[index].d ;
}


//Reset des valeurs du pid
void SrvPIDResetValues( void )
{
	for(Int8U loop_pid = 0 ; loop_pid < E_NB_PIDS ; loop_pid++ )
	{
		pids[ loop_pid ].integratedError = 0.0F ;
		pids[ loop_pid ].lastPosition = 0.0F ;
	}
}

static float pid_error = 0.0F;
static float p_term = 0.0F;
static float i_term = 0.0F;
static float d_term = 0.0F;
Int16S SrvPIDCompute(Int8U index, Int16S currentPosition, Int16S targetPosition, float delta_time )
{	
	pid_error = 0.0F;
	p_term = 0.0F;
	i_term = 0.0F;
	d_term = 0.0F;
	//determine l'erreur
	pid_error = (targetPosition - currentPosition );
	
	//Calcul du terme P
	p_term = (float)( pids[index].p * pid_error );
	
	//calcul de l'erreur int?gr?
	pids[index].integratedError += pid_error * delta_time ;

	//limitation de l'erreur
	float windupguard = 500.0F / pids[index].i ;
	
	if(pids[index].integratedError > windupguard)
	{
		pids[index].integratedError = windupguard;
	}
	else if(pids[index].integratedError < -windupguard)
	{
		pids[index].integratedError = -windupguard;
	}
	
	//Calcul du terme I
	i_term = (float)( pids[index].i * pids[index].integratedError );

	//Calcul du terme D
	d_term = (float)((float)( pids[index].d * (float)( currentPosition - pids[index].lastPosition ) ) / delta_time );
	
	//on conserve la position actuel
	pids[index].lastPosition = currentPosition;
	
	//retourne le calcul PID
	return (Int16S)(float)(p_term + i_term + d_term);
}
