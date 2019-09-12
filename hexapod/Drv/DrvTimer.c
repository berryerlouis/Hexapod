/*
 * Drv_timer.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 

#include "DrvTimer.h"

////////////////////////////////////////PRIVATE STRUCTURES///////////////////////////////////////
//structure configuration initial des timers
typedef struct SSTimer
{
	Boolean enable;
	Int16U delay;
	Int16U cpt_delay;
	ETimerMode mode;
	Boolean reload;
	ptrfct_Isr_Callback_Timer ptrfct;
} STimer ;

////////////////////////////////////////PRIVATE DEFINES/////////////////////////////////////////
#define TIMER2_OFFSET_COMPA		125U
#define MACRO_NULL_TIMER {\
							.enable = FALSE,\
							.delay = 0U,\
							.cpt_delay = 0U,\
							.mode = E_TIMER_MODE_NONE,\
							.ptrfct = NULL,\
						}\

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//configuration initial des leds
volatile STimer MesTimers[ E_NB_TIMER ];

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
// fait tourner le timer 0 compA a 1 ms
void DrvTimerInitSystemTimer( void ) ;
  
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Timer 
Boolean DrvTimerInit( void )
{
	//if timer is needed
	if( E_NB_TIMER > 0U )
	{
		//on configure les timers autre que le timer event
		for(Int8U loop_index = 0U; loop_index < E_NB_TIMER ; loop_index++ )
		{
			MesTimers[ loop_index ].enable = FALSE;
			MesTimers[ loop_index ].ptrfct = NULL;
		}
			
		//on init le timer
		TCCR2A	= 0U;
		TCNT2	= 0U;
		//prescaler Clk/128 => 0,000008 s => 8µs
		BIT_HIGH(TCCR2B,CS20);
		BIT_HIGH(TCCR2B,CS22);
		//1ms
		OCR2A   = TIMER2_OFFSET_COMPA;
		BIT_HIGH(TIFR2,OCF2A);
		BIT_HIGH(TIMSK2,OCIE2A);
		return TRUE;
	}
	return FALSE;
}

	
//fct qui parametre le timer
void DrvTimerAddTimer( Int8U index_timer, Int16U delay_ms, ETimerMode mode, ptrfct_Isr_Callback_Timer ptrfct )
{
	MesTimers[ index_timer ].delay = delay_ms;
	MesTimers[ index_timer ].cpt_delay = 0U;
	MesTimers[ index_timer ].mode = mode;
	MesTimers[ index_timer ].ptrfct = ptrfct;
	MesTimers[ index_timer ].enable = TRUE;
}

//fct qui met en pause le timer
void DrvTimerPauseTimer( Int8U index_timer )
{	
	MesTimers[ index_timer ].enable = FALSE;
}

//fct qui remet a zero les parametres du timer
void DrvTimerStopTimer( Int8U index_timer )
{	
	MesTimers[ index_timer ].enable    = FALSE;
	MesTimers[ index_timer ].delay     = 0U ;
	MesTimers[ index_timer ].cpt_delay = 0U;
	MesTimers[ index_timer ].mode      = E_TIMER_MODE_NONE;
	MesTimers[ index_timer ].ptrfct    = NULL;
}

//fct qui reseter le timer
void DrvTimerResetTimer( Int8U index_timer )
{
	MesTimers[ index_timer ].cpt_delay = 0U;	
}

//fct qui fixe un delay au timer
void DrvTimerDelayTimer( Int8U index_timer , Int16U delay)
{
	MesTimers[ index_timer ].cpt_delay = 0U;	
	MesTimers[ index_timer ].delay	  = delay;
}

//on reset tt les timers
void DrvTimerTickReset(void)
{
	//stop IT
	BIT_LOW(TIMSK2,OCIE2A);
	//set the threshold
	OCR2A   = TIMER2_OFFSET_COMPA;
	//on configure les timers autre que le timer event
	for(Int8U loop_index = 0U; loop_index < E_NB_TIMER ; loop_index++ )
	{
		MesTimers[ loop_index ].cpt_delay = 0U;	
	}
	//reset the counter
	TCNT2	= 0U;
	//enable the IT
	BIT_HIGH(TIMSK2,OCIE2A);
}


/////////////////////////////////////ISR PRIVATE FUNCTIONS////////////////////////////////////////
//ISR timer event 1ms
ISR(TIMER2_COMPA_vect)
{
	//on prend le temps de debut d'interruption
	Int8U tcnt2_start = TCNT2;
	for(Int8U loop_index = 0U; loop_index < E_NB_TIMER ; loop_index++ )
	{
		//si le timer est activé
		if( MesTimers[ loop_index ].enable == TRUE )
		{
			//on incremente le compteur
			MesTimers[ loop_index ].cpt_delay++;
			
			//on test par rapport a la valeur utilisateur
			if(MesTimers[ loop_index ].cpt_delay == MesTimers[ loop_index ].delay )
			{
				if( MesTimers[ loop_index ].mode != E_TIMER_MODE_NONE )
				{
					//on remet a zero le compteur
					MesTimers[ loop_index ].cpt_delay = 0U;
					
					//si on est en mode ONE SHOT 
					//on atteind le temp, on supprime le timer
					if (MesTimers[ loop_index ].mode == E_TIMER_MODE_ONE_SHOT)
					{
						//on garde la fonction appelé avant de tt effacer
						ptrfct_Isr_Callback_Timer ptrfct = MesTimers[ loop_index ].ptrfct;
						//on supprime le timer One Shot
						DrvTimerStopTimer( loop_index );
						//on appelle la fonction
						if( ptrfct != NULL )
						{
							ptrfct();
						}
					}
					else
					{
						//on appelle la fonction
						if( MesTimers[ loop_index ].ptrfct != NULL )
						{
							MesTimers[ loop_index ].ptrfct();
						}
					}
				}
			}
		}			
	}
	//on prend le temps de fin d'interruption
	Int8U tcnt2_end = TCNT2;
	//reenclanche le timer dans 1ms
	OCR2A = tcnt2_end + TIMER2_OFFSET_COMPA - (tcnt2_end - tcnt2_start) - 1U ;
}