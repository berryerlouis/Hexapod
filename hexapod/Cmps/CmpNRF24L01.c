/*
 * CmpPCA9685.c
 *
 * Created: 11/02/2014 12:00:01
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Cmps/CmpNRF24L01.h"

#include "Drv/DrvSpi.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean CmpNRF24L01Init( Int8U addr, EIoPin CS )
{
	Boolean oSuccess = TRUE;
	
	// init SPI
	DrvSpiInit();
	// chip select
	DrvIoSetPinOutput(CS);
	
	return oSuccess;
}

void CmpNRF24L01PowerDown( void )
{
	//uint8_t reg = BIT_LOW(DrvSpiReadData(NRF24L01_CONFIG) ,NRF24L01_PWR_UP);
	//DrvSpiSendData(NRF24L01_CONFIG, reg);
}

void CmpNRF24L01PowerUp( void )
{
	//uint8_t reg = BIT_HIGH(DrvSpiReadData(NRF24L01_CONFIG) ,NRF24L01_PWR_UP);
	//DrvSpiSendData(NRF24L01_CONFIG, reg);
}


void CmpNRF24L01GetPALevel( void )
{
}


Boolean CmpNRF24L01SetDataRate( Int8U speed )
{
	Boolean oSuccess = FALSE;
	
	return oSuccess;
}

Boolean CmpNRF24L01Reset( Int8U addr )
{
	Boolean oSuccess = FALSE;
	
	return oSuccess;
}

