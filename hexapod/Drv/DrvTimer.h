/*
 * DrvTimer.h
 *
 * Created: 06/07/2012 16:35:15
 *  Author: berryer
 */ 

#ifndef DrvTIMER_H_
#define DrvTIMER_H_

#include "../Conf/ConfHard.h"
#include "../Tools/tools_typedefs.h"



//////////////////////////////////////////PUBLIC PTR FCTS//////////////////////////////////////////
typedef void (*ptrfct_Isr_Callback_Timer)( void);

////////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////
//mode fonctionnement du timer
typedef enum
{
  E_TIMER_MODE_ONE_SHOT,
  E_TIMER_MODE_PERIODIC,
  E_TIMER_MODE_NONE,
}ETimerMode;

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Timer 
Boolean DrvTimerInit( void ) ;

//fct qui parametre le timer
void DrvTimerAddTimer( Int8U index_timer, Int16U delay_ms, ETimerMode mode, ptrfct_Isr_Callback_Timer ptrfct ) ;

//fct qui met en pause le timer
void DrvTimerPauseTimer( Int8U index_timer ) ;

//fct qui remet a zero les parametres du timer
void DrvTimerStopTimer( Int8U index_timer ) ;

//fct qui reseter le timer
void DrvTimerResetTimer( Int8U index_timer ) ;

//fct qui fixe un delay au timer
void DrvTimerDelayTimer( Int8U index_timer , Int16U delay) ;

//on reset tt les timers
void DrvTimerTickReset(void) ;
#endif /* DrvTIMER_H_ */