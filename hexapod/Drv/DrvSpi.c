/*
 * DrvSpi.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 

#include "../Conf/ConfHard.h"

#include "DrvSpi.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
volatile uint8_t data = 0U;

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init du Drv Uart 
Boolean DrvSpiInit( void )
{
	Boolean oSuccess = TRUE;
			 
	DrvIoSetPinOutput(EIO_PIN_B_5);
	DrvIoSetPinInput(EIO_PIN_B_6);
	DrvIoSetPinOutput(EIO_PIN_B_7);
	
	BIT_HIGH(SPCR,MSTR);			 // Set as Master
	BIT_HIGH(SPCR,SPR0);			 // divided clock by 128
	BIT_HIGH(SPCR,SPR1);
	//BIT_HIGH(SPCR,SPIE);			 // Enable SPI Interrupt
	BIT_HIGH(SPCR,SPE);				 // Enable SPI
	
	return oSuccess;
}


//envoie des données sur la liaison série
Boolean DrvSpiSendData( uint8_t dataToSend )
{
	SPDR = dataToSend;
	while(!(SPSR & (1<<SPIF)));
	
	return SPDR;
}

//recoit une donnée de la liaison série
uint8_t DrvSpiReadData( void )
{
	while(!(SPSR & (1<<SPIF) ));
	return(SPDR);
}



////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////

ISR(SPI_STC_vect)
{
	SPDR = data;
}

