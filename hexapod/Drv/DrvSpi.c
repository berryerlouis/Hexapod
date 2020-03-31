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
EIoPin spiPins[ E_NB_SPIS ][3U] = {SPI_PINS} ;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

// Init du Drv Uart 
Boolean DrvSpiInit( void )
{
	Boolean oSuccess = TRUE;
			 
	//set gpio to output for SPI
	DrvIoSetPinOutput(spiPins[ E_I2C_0 ][SPI_CLK_PIN]);
	DrvIoSetPinOutput(spiPins[ E_SPI_0 ][SPI_MOSI_PIN]);
	DrvIoSetPinInput(spiPins[ E_SPI_0 ][SPI_MISO_PIN]);
	
	BIT_HIGH(SPCR,MSTR);			 // Set as Master
	BIT_HIGH(SPCR,SPR0);			 // divided clock by 128
	BIT_HIGH(SPCR,SPR1);
	//BIT_HIGH(SPCR,SPIE);			 // Enable SPI Interrupt
	BIT_HIGH(SPCR,SPE);				 // Enable SPI
	
	return oSuccess;
}

//set CS pin
Boolean DrvSpiSetCS( EIoPin pin )
{
	DrvIoSetPinOutput(pin);
	DrvIoSetPinState(pin,IO_LEVEL_LOW);
	return TRUE;
}


//read register
Boolean DrvSpiReadReg( Int8U slave_register, Int8U *data )
{    
	Boolean status;
    status = DrvSpiSendData(slave_register);
    data[0U] = DrvSpiSendData(0xFFU);
    return status;
}
//write register
Boolean DrvSpiWriteReg( Int8U slave_register, Int8U data )
{
	Boolean status;
    status = DrvSpiSendData(slave_register);
    DrvSpiSendData(data);
    return status;
}
//read many register
Boolean DrvSpiReadRegBuf(Int8U slave_register, Int8U *buffer, Int8U buffer_size)
{
    Boolean status;
    status = DrvSpiSendData(slave_register);
    while (buffer_size--) {
        *buffer++ = DrvSpiSendData(0xFFU);
    }
    return status;
}

//write many register
Boolean DrvSpiWriteRegBuf(Int8U slave_register, Int8U *buffer, Int8U buffer_size)
{
    Boolean status;
    status = DrvSpiSendData(slave_register);
    while (buffer_size--) {
        DrvSpiSendData(*buffer++);
    }
    return status;
}

//envoie des données sur la liaison série
Int8U DrvSpiSendData( Int8U dataToSend )
{
	SPDR = dataToSend;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}




////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////

ISR(SPI_STC_vect)
{
	SPDR = data;
}

