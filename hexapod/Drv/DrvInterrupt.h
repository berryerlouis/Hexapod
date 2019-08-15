/*
 * drv_interrupt.h
 *
 * Created: 28/06/2011 15:53:09
 *  Author: berryer
 */ 


#ifndef DRV_INTERRUPT_H_
#define DRV_INTERRUPT_H_

#include "../Tools/tools_typedefs.h"



////////////////////////////////////////////PUBLIC ENUMS///////////////////////////////////////////


/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// On clear toutes les ITs 
#define DrvInterruptClearAllInterrupts() cli()

// On clear toutes les ITs 
#define DrvInterruptSetAllInterrupts() sei()

#endif /* DRV_INTERRUPT_H_ */