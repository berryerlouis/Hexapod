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

Boolean CmpNRF24L01WriteRegister( Int8U reg, Int8U data );
Boolean CmpNRF24L01WriteBufferRegister( Int8U reg, Int8U *data, Int8U len );
Boolean CmpNRF24L01ReadBufferRegister( Int8U reg, Int8U *data, Int8U len );
Boolean CmpNRF24L01ReadRegister( Int8U reg, Int8U data );
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int8U NRF24L01status = 0U;
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean CmpNRF24L01Init( EIoPin CS )
{
	Boolean oSuccess = TRUE;
	
	// chip select
	DrvSpiSetCS(CS);
	NRF24L01status = CmpNRF24L01GetStatus();
	if(NRF24L01status)
	{
		BIT_HIGH(SPCR,SPE);				 // Enable SPI
	}
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

Int8U CmpNRF24L01GetStatus( void )
{
	return DrvSpiSendData(NRF24L01_NOP);
}

Boolean CmpNRF24L01WriteRegister( Int8U reg, Int8U data )
{
	return DrvSpiWriteReg(NRF24L01_W_REGISTER | (NRF24L01_REGISTER_MASK & reg), data);
}
Boolean CmpNRF24L01WriteBufferRegister( Int8U reg, Int8U *data, Int8U len )
{
	return DrvSpiWriteRegBuf(NRF24L01_W_REGISTER | (NRF24L01_REGISTER_MASK & reg), data, len);
}
Boolean CmpNRF24L01ReadRegister( Int8U reg, Int8U data )
{
	return DrvSpiReadReg(NRF24L01_R_REGISTER | (NRF24L01_REGISTER_MASK & reg), &data);
}
Boolean CmpNRF24L01ReadBufferRegister( Int8U reg, Int8U *data, Int8U len )
{
	return DrvSpiReadRegBuf(NRF24L01_R_REGISTER | (NRF24L01_REGISTER_MASK & reg), data, len);
}