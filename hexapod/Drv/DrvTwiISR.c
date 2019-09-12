/*
 * drv_twi.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 
#include "../Conf/ConfHard.h"
#include <util/twi.h>
#include <util/delay.h>
#include "DrvTwi.h"
#include "DrvLed.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define I2C_SCL_PIN	0U
#define I2C_SDA_PIN	1U


// TWI Status Codes
#define TWI_START_SENT			0x08 // Start sent
#define TWI_REP_START_SENT		0x10 // Repeated Start sent
// Master Transmitter Mode
#define TWI_MT_SLAW_ACK			0x18 // SLA+W sent and ACK received
#define TWI_MT_SLAW_NACK		0x20 // SLA+W sent and NACK received
#define TWI_MT_DATA_ACK			0x28 // DATA sent and ACK received
#define TWI_MT_DATA_NACK		0x30 // DATA sent and NACK received
// Master Receiver Mode
#define TWI_MR_SLAR_ACK			0x40 // SLA+R sent, ACK received
#define TWI_MR_SLAR_NACK		0x48 // SLA+R sent, NACK received
#define TWI_MR_DATA_ACK			0x50 // Data received, ACK returned
#define TWI_MR_DATA_NACK		0x58 // Data received, NACK returned

// Miscellaneous States
#define TWI_LOST_ARBIT			0x38 // Arbitration has been lost
#define TWI_NO_RELEVANT_INFO	0xF8 // No relevant information available
#define TWI_ILLEGAL_START_STOP	0x00 // Illegal START or STOP condition has been detected
#define TWI_SUCCESS				0xFF // Successful transfer, this state is impossible from TWSR as bit2 is 0 and read only


#define DrvTwiSendStart()		(TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE)) // Send the START signal, enable interrupts and TWI, clear TWINT flag to resume transfer.
#define DrvTwiSendStop()		(TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE)) // Send the STOP signal, enable interrupts and TWI, clear TWINT flag.
#define DrvTwiSendTransmit()	(TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)) // Used to resume a transfer, clear TWINT and ensure that TWI and interrupts are enabled.
#define DrvTwiSendACK()			(TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA)) // FOR MR mode. Resume a transfer, ensure that TWI and interrupts are enabled and respond with an ACK if the device is addressed as a slave or after it receives a byte.
#define DrvTwiSendNACK()		(TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)) // FOR MR mode. Resume a transfer, ensure that TWI and interrupts are enabled but DO NOT respond with an ACK if the device is addressed as a slave or after it receives a byte.

// Get TWI status
#define TWI_STATUS	(TWSR & 0xF8)
// Transmit buffer length
#define TXMAXBUFLEN 50
// Receive buffer length
#define RXMAXBUFLEN 50

////////////////////////////////////////PRIVATE STRUCTURES/////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
// Global transmit buffer
uint8_t TwiTransmitBuffer[TXMAXBUFLEN];
// Global receive buffer
volatile uint8_t TwiReceiveBuffer[RXMAXBUFLEN];
// Buffer indexes
volatile int TwiTXBuffIndex; // Index of the transmit buffer. Is volatile, can change at any time.
int TwiRXBuffIndex; // Current index in the receive buffer
// Buffer lengths
int TwiTXBuffLen; // The total length of the transmit buffer
int TwiRXBuffLen; // The total number of bytes to read (should be less than RXMAXBUFFLEN)

EIoPin i2cPins[ E_NB_I2CS ][2U] = {I2C_PINS} ;

DrvTwiInfoStruct twiInfo;
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
  
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

/************************************************************************/
/*Init du Drv Twi                                                       */
/************************************************************************/
Boolean DrvTwiInit( Int32U speed ) 
{
	Boolean o_success = FALSE;
	
	if(twiInfo.initialized == FALSE )
	{
		//set gpio to output for I2C
		DrvIoSetPinOutput(i2cPins[ E_I2C_0 ][I2C_SCL_PIN]);
		DrvIoSetPinOutput(i2cPins[ E_I2C_0 ][I2C_SDA_PIN]);
	
	
		twiInfo.mode = Ready;
		twiInfo.errorCode = 0xFFU;
		twiInfo.repStart = 0U;
		// Set pre-scalers (no pre-scaling)
		TWSR = 0;
		// Set bit rate
		TWBR = ((CONF_FOSC_HZ / speed) - 16) / 2;
		// Enable TWI and interrupt
		TWCR = (1 << TWIE) | (1 << TWEN);
		twiInfo.initialized = TRUE;
		o_success = TRUE;
	}
	return o_success;
}


uint8_t DrvTwiReady()
{
	if ( (twiInfo.mode == Ready) | (twiInfo.mode == RepeatedStartSent) )
	{
		return TRUE;
	}
	return FALSE;
}

uint8_t DrvTwiTransmitData(void *const TXdata, uint8_t dataLen, uint8_t repStart)
{
	if (dataLen <= TXMAXBUFLEN)
	{
		// Wait until ready
		while (!DrvTwiReady()) {_delay_us(1);}
		// Set repeated start mode
		twiInfo.repStart = repStart;
		// Copy data into the transmit buffer
		uint8_t *data = (uint8_t *)TXdata;
		for (int i = 0; i < dataLen; i++)
		{
			TwiTransmitBuffer[i] = data[i];
		}
		// Copy transmit info to global variables
		TwiTXBuffLen = dataLen;
		TwiTXBuffIndex = 0;
		
		// If a repeated start has been sent, then devices are already listening for an address
		// and another start does not need to be sent.
		if (twiInfo.mode == RepeatedStartSent)
		{
			twiInfo.mode = Initializing;
			TWDR = TwiTransmitBuffer[TwiTXBuffIndex++]; // Load data to transmit buffer
			DrvTwiSendTransmit(); // Send the data
		}
		else // Otherwise, just send the normal start signal to begin transmission.
		{
			twiInfo.mode = Initializing;
			DrvTwiSendStart();
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

uint8_t DrvTwiReadData(uint8_t TWIaddr, uint8_t bytesToRead, uint8_t repStart)
{
	// Check if number of bytes to read can fit in the RXbuffer
	if (bytesToRead < RXMAXBUFLEN)
	{
		// Reset buffer index and set RXBuffLen to the number of bytes to read
		TwiRXBuffIndex = 0;
		TwiRXBuffLen = bytesToRead;
		// Create the one value array for the address to be transmitted
		uint8_t TXdata[1];
		// Shift the address and AND a 1 into the read write bit (set to write mode)
		TXdata[0] = (TWIaddr << 1) | 0x01;
		// Use the TWITransmitData function to initialize the transfer and address the slave
		DrvTwiTransmitData(TXdata, 1, repStart);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

/************************************************************************/
/*permits to get the number of errors count                             */
/************************************************************************/
DrvTwiInfoStruct *DrvTwiGetInfo( void )
{
	return &twiInfo;
}

//read register
Boolean DrvTwiReadReg( Int8U slave_address , Int8U slave_register, Int8U *data )
{
	uint8_t twiData[2U] = {	slave_address, slave_register };
	do
	{
		DrvTwiTransmitData(twiData, sizeof(twiData), 1U);
	}while (DrvTwiGetInfo()->errorCode != 0xFF);
	do
	{
		DrvTwiReadData(slave_address, 1U, 0U);
	}while (DrvTwiGetInfo()->errorCode != 0xFF);
	
	return TRUE;
}
//write register
Boolean DrvTwiWriteReg( Int8U slave_address , Int8U slave_register, Int8U data )
{
	Int8U twiData[3U] = { slave_address, slave_register, data };
	do
	{
		DrvTwiTransmitData(twiData,sizeof(twiData),0U);
	}while (DrvTwiGetInfo()->errorCode != 0xFF);
	
	return TRUE;
}
//read many register
Boolean DrvTwiReadRegBuf(Int8U slave_address, Int8U slave_register, Int8U *buffer, Int8U buffer_size)
{
	uint8_t twiData[2U] = {	slave_address, slave_register };
	do
	{
		DrvTwiTransmitData(twiData, sizeof(twiData), 1U);
	}while (DrvTwiGetInfo()->errorCode != 0xFF);
	do
	{
		DrvTwiReadData(slave_address, buffer_size, 0U);
	}while (DrvTwiGetInfo()->errorCode != 0xFF);
	
	
	for(Int8U loop = 0U; loop < buffer_size ; loop++ )
	{
		buffer[loop] = TwiReceiveBuffer[loop];
	}
	
	return TRUE;
}
//write many register
Boolean DrvTwiWriteRegBuf(Int8U slave_address, Int8U slave_register, Int8U *buffer, Int8U buffer_size)
{
	Int8U twiData[2U + buffer_size];
	twiData[0U] = slave_address;
	twiData[1U] = slave_register;
	for(Int8U loop = 0U; loop < buffer_size ; loop++ )
	{
		twiData[loop + 2U] = buffer[loop];
	}
	do
	{
		DrvTwiTransmitData(twiData,sizeof(twiData),0U);
	}while (DrvTwiGetInfo()->errorCode != 0xFF);
	
	return TRUE;
}


//write many register with same data
Boolean DrvTwiFillRegBuf(Int8U slave_address, Int8U slave_register, Int8U data, Int8U buffer_size)
{
	Int8U twiData[2U + buffer_size];
	twiData[0U] = slave_address;
	twiData[1U] = slave_register;
	for(Int8U loop = 0U; loop < buffer_size ; loop++ )
	{
		twiData[loop + 2U] = data;
	}
	do
	{
		DrvTwiTransmitData(twiData,sizeof(twiData),0U);
	}while (DrvTwiGetInfo()->errorCode != 0xFF);
	
	return TRUE;
}

ISR (TWI_vect)
{
	switch (TWI_STATUS)
	{
		// ----\/ ---- MASTER TRANSMITTER OR WRITING ADDRESS ----\/ ----  //
		case TWI_MT_SLAW_ACK: // SLA+W transmitted and ACK received
		// Set mode to Master Transmitter
		twiInfo.mode = MasterTransmitter;
		case TWI_START_SENT: // Start condition has been transmitted
		case TWI_MT_DATA_ACK: // Data byte has been transmitted, ACK received
		if (TwiTXBuffIndex < TwiTXBuffLen) // If there is more data to send
		{
			TWDR = TwiTransmitBuffer[TwiTXBuffIndex++]; // Load data to transmit buffer
			twiInfo.errorCode = TWI_NO_RELEVANT_INFO;
			DrvTwiSendTransmit(); // Send the data
		}
		// This transmission is complete however do not release bus yet
		else if (twiInfo.repStart)
		{
			twiInfo.errorCode = 0xFF;
			DrvTwiSendStart();
		}
		// All transmissions are complete, exit
		else
		{
			twiInfo.mode = Ready;
			twiInfo.errorCode = 0xFF;
			DrvTwiSendStop();
		}
		break;
		
		// ----\/ ---- MASTER RECEIVER ----\/ ----  //
		
		case TWI_MR_SLAR_ACK: // SLA+R has been transmitted, ACK has been received
		// Switch to Master Receiver mode
		twiInfo.mode = MasterReceiver;
		// If there is more than one byte to be read, receive data byte and return an ACK
		if (TwiRXBuffIndex < TwiRXBuffLen-1)
		{
			twiInfo.errorCode = TWI_NO_RELEVANT_INFO;
			DrvTwiSendACK();
		}
		// Otherwise when a data byte (the only data byte) is received, return NACK
		else
		{
			twiInfo.errorCode = TWI_NO_RELEVANT_INFO;
			DrvTwiSendNACK();
		}
		break;
		
		case TWI_MR_DATA_ACK: // Data has been received, ACK has been transmitted.
		
		/// -- HANDLE DATA BYTE --- ///
		TwiReceiveBuffer[TwiRXBuffIndex++] = TWDR;
		// If there is more than one byte to be read, receive data byte and return an ACK
		if (TwiRXBuffIndex < TwiRXBuffLen-1)
		{
			twiInfo.errorCode = TWI_NO_RELEVANT_INFO;
			DrvTwiSendACK();
		}
		// Otherwise when a data byte (the only data byte) is received, return NACK
		else
		{
			twiInfo.errorCode = TWI_NO_RELEVANT_INFO;
			DrvTwiSendNACK();
		}
		break;
		
		case TWI_MR_DATA_NACK: // Data byte has been received, NACK has been transmitted. End of transmission.
		
		/// -- HANDLE DATA BYTE --- ///
		TwiReceiveBuffer[TwiRXBuffIndex++] = TWDR;
		// This transmission is complete however do not release bus yet
		if (twiInfo.repStart)
		{
			twiInfo.errorCode = 0xFF;
			DrvTwiSendStart();
		}
		// All transmissions are complete, exit
		else
		{
			twiInfo.mode = Ready;
			twiInfo.errorCode = 0xFF;
			DrvTwiSendStop();
		}
		break;
		
		// ----\/ ---- MT and MR common ----\/ ---- //
		
		case TWI_MR_SLAR_NACK: // SLA+R transmitted, NACK received
		case TWI_MT_SLAW_NACK: // SLA+W transmitted, NACK received
		case TWI_MT_DATA_NACK: // Data byte has been transmitted, NACK received
		case TWI_LOST_ARBIT: // Arbitration has been lost
		// Return error and send stop and set mode to ready
		if (twiInfo.repStart)
		{
			twiInfo.errorCode = TWI_STATUS;
			DrvTwiSendStart();
		}
		// All transmissions are complete, exit
		else
		{
			twiInfo.mode = Ready;
			twiInfo.errorCode = TWI_STATUS;
			DrvTwiSendStop();
		}
		break;
		case TWI_REP_START_SENT: // Repeated start has been transmitted
		// Set the mode but DO NOT clear TWINT as the next data is not yet ready
		twiInfo.mode = RepeatedStartSent;
		break;
		
		// ----\/ ---- SLAVE RECEIVER ----\/ ----  //
		
		// TODO  IMPLEMENT SLAVE RECEIVER FUNCTIONALITY
		
		// ----\/ ---- SLAVE TRANSMITTER ----\/ ----  //
		
		// TODO  IMPLEMENT SLAVE TRANSMITTER FUNCTIONALITY
		
		// ----\/ ---- MISCELLANEOUS STATES ----\/ ----  //
		case TWI_NO_RELEVANT_INFO: // It is not really possible to get into this ISR on this condition
		// Rather, it is there to be manually set between operations
		break;
		case TWI_ILLEGAL_START_STOP: // Illegal START/STOP, abort and return error
		twiInfo.errorCode = TWI_ILLEGAL_START_STOP;
		twiInfo.mode = Ready;
		DrvTwiSendStop();
		break;
	}
	
}