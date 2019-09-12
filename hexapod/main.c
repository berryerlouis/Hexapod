/*
 * main.c
 *
 * Created: 03/07/2012 10:06:16
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Srv/SrvCore.h"

////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES////////////////////////////////////////


//fonction principale
int main(void)
{	
	// ********************* Reset Cause **********************************************
	//imu.status.lastResetCause = MCUSR;
	//reset status register
	MCUSR = 0U;
	
	// ********************* Interrupt Vectors ****************************************
	// Enable change of interrupt vectors
	MCUCR |= (1<<IVCE);
	// Move interrupts to app flash section
	MCUCR &= ~(1<<IVSEL);

	// ********************* Interrupt Disable ****************************************
	cli();
	
	// ********************* WatchDog Enable ******************************************
	wdt_enable(WDTO_1S);
	
	// ********************* Interrupt Enable *****************************************
	sei();

	// ********************* Core init ****************************************
	SrvCoreInit();
		
	while(TRUE)
	{
		// ********************* Watchdog *********************************************
		wdt_enable(WDTO_1S);
								
		// ********************* Call services******************
		SrvCoreLoop();
	}
}



















/*FUSES = 
    {
        .low = LFUSE_DEFAULT,
        .high = (FUSE_SPIEN & FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE  & FUSE_JTAGEN)
        .extended = EFUSE_DEFAULT,
    };*/

