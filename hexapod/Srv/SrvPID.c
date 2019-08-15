/*
 * SrvPID.c
 *
 * Created: 24/09/2012 14:48:34
 *  Author: berryer
 */ 
/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

#include "Srv/SrvPID.h"
#include "Srv/SrvImu.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES///////////////////////////////////////

////////////////////////////////////////PRIVATE FONCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static Pid pids[ E_NB_PIDS ] ;

//variables de timming
Int32U lastread_pid = 0U;
float interval_pid = 0.0F;

//Init des valeurs du pid
void SrvPIDInit( void )
{
	float p = 0.1F;
	float i = 0.0F;
	float d = 0.0F;
	Boolean eep_config = DrvEepromIsConfigured();
	
	for(Int8U loop_pid = 0U ; loop_pid < E_NB_PIDS ; loop_pid++ )
	{
		if( eep_config == TRUE )
		{
			//on lit les valeurs enregistrés
			DrvEepromReadPID( loop_pid, &p, &i, &d );
		}
		else
		{
			//on ecrit les valeurs nulles
			DrvEepromWritePID( loop_pid, p, i, d );
		}
		//set actual values
		SrvPIDSetValues( loop_pid, p, i, d );
	}
	SrvPIDResetValues( ) ;
}

/************************************************************************/
/*Dispatcher d'evenements                                               */
/************************************************************************/
void SrvPIDDispatcher (void)
{	
	// ********************* Calcul du temps de cycle *************************
	Int32U now_pid = DrvTickGetTimeUs();
	interval_pid = (float)(now_pid - lastread_pid) / 1000000.0F;
	lastread_pid = now_pid;
	
	#ifdef 	BI
		imu.pid_error.roulis	= SrvPIDCompute( 0U , imu.angles.roulis				, 0U/*imu.angles.roulis	*/			, interval_pid);
	#else
		// ********************* PID **********************************************
		imu.pid_error.roulis	= SrvPIDCompute( 0U , imu.angles.roulis				, 0U/*imu.angles.roulis	*/			, interval_pid);
		imu.pid_error.tangage	= SrvPIDCompute( 1U , imu.angles.tangage			, 0U/*imu.angles.tangage*/			, interval_pid);
		imu.pid_error.lacet		= 0U;//SrvPIDCompute( 2U , imu.angles.lacet				, imu.angles.lacet				, interval_pid);
		imu.pid_error.altitude	= SrvPIDCompute( 3U , imu.sensors->bar->altitude	, imu.sensors->bar->altitude		, interval_pid);
	#endif
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
	//on ecrit les valeurs
	DrvEepromWritePID( index, p, i, d );
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
	
	//calcul de l'erreur intégré
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

