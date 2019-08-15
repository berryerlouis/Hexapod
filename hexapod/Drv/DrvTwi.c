/*
 * drv_twi.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 
#include "../Conf/ConfHard.h"
#include <util/twi.h>
#include "DrvTwi.h"
#include "DrvLed.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define I2C_SCL_PIN	0U
#define I2C_SDA_PIN	1U

////////////////////////////////////////PRIVATE STRUCTURES/////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//permits to get the number of errors count
static Int16U errorsCount = 0U;
EIoPin i2cPins[ E_NB_I2CS ][2U] = {I2C_PINS} ;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//On attend la transmission   
static void DrvTwiWaitTransmission( void );
  
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

/************************************************************************/
/*Init du Drv Twi                                                       */
/************************************************************************/
Boolean DrvTwiInit( Int32U speed ) 
{
	Boolean o_success = TRUE;
	
	//set gpio to output for I2C
	DrvIoSetPinOutput(i2cPins[ E_I2C_0 ][I2C_SCL_PIN]);
	DrvIoSetPinOutput(i2cPins[ E_I2C_0 ][I2C_SDA_PIN]);
	
	TWBR =  (Int8U)((Int16U)((Int32U)(CONF_FOSC_HZ / speed ) - 16U) / 2U);	// change the I2C clock rate
	TWSR =  0U;	                                        	// Not used. Driver presumes prescaler to be 00.
	TWCR =	(1U<<TWEN) |                                	// Enable TWI-interface and release TWI pins.
			(0U<<TWIE) | (0U<<TWINT) |                    	// Disable Interupt.
			(0U<<TWEA) | (0U<<TWSTA) | (0U<<TWSTO) |      	// No Signal requests.
			(0U<<TWWC);  
	errorsCount = 0U;
	return o_success;		 
}


/************************************************************************/
/*Read register                                                         */
/************************************************************************/
Boolean DrvTwiReadReg( Int8U slave_address , Int8U slave_register, Int8U *data )
{	
	Boolean read_no_error = FALSE;
	
	//Send start
	TWCR = (1U<<TWINT)|(1U<<TWSTA)|(1U<<TWEN);
	DrvTwiWaitTransmission();
	if (((TWSR==TW_START) || (TWSR==TW_REP_START)) )
	{
		//send slave address write
		TWDR =  (slave_address << 1) | TW_WRITE;
		TWCR = (1U<<TWINT) | (1U<<TWEN);
		DrvTwiWaitTransmission();
		if( (TWSR == TW_MT_SLA_ACK) )
		{
			//send register address
			TWDR = slave_register;
			TWCR = (1U<<TWINT) | (1U<<TWEN);
			DrvTwiWaitTransmission();
			if ( (TWSR == TW_MT_DATA_ACK) )
			{
				//repeat start
				TWCR = (1U<<TWINT)|(1U<<TWSTA)|(1U<<TWEN);
				DrvTwiWaitTransmission();
				if ( (TWSR == TW_REP_START) )
				{
					//send slave address read
					TWDR =  (slave_address << 1 ) | TW_READ;
					TWCR = (1U<<TWINT) | (1U<<TWEN);
					DrvTwiWaitTransmission();					
					if ( (TWSR==TW_MR_SLA_ACK) )
					{
						//send clock
						TWCR = (1U<<TWINT) | (1U<<TWEN);
						DrvTwiWaitTransmission();
						if ( ( TWSR==TW_MR_DATA_NACK) )
						{
							//record data
							data[ 0U ] = TWDR;
							
							//send stop
							TWCR = (1U<<TWINT) | (1U<<TWEN) | (1U<<TWSTO);
							if (TWSR==TW_NO_INFO)
							{
								//no error
								read_no_error = TRUE;
							}
							else
							{
								errorsCount++;
							}
						}
					}
				}
			}
		}
	}
	
	return read_no_error;
}

/************************************************************************/
/*Write register                                                        */
/************************************************************************/
Boolean DrvTwiWriteReg( Int8U slave_address , Int8U slave_register, Int8U data )
{
	Boolean write_no_error = FALSE;
	
	TWCR = (1U<<TWINT)|(1U<<TWSTA)|(1U<<TWEN);           // SEND START
	DrvTwiWaitTransmission();

	if  ( ((TWSR==TW_START) || (TWSR==TW_REP_START)) )
	{
		//send slave address
		TWDR =  (slave_address << 1 ) | TW_WRITE;
		TWCR = (1U << TWINT) | (1U << TWEN);
		DrvTwiWaitTransmission();
		if ( (TWSR==TW_MT_SLA_ACK) )
		{
			//send register address
			TWDR = slave_register;
			TWCR = (1U<<TWINT) | (1U<<TWEN);
			DrvTwiWaitTransmission();
			if ( (TWSR==TW_MT_DATA_ACK) )
			{
				//write data to register
				TWDR = data;
				TWCR = (1U<<TWINT) | (1U<<TWEN);
				DrvTwiWaitTransmission();
				if ( (TWSR==TW_MT_DATA_ACK) )
				{
					//send stop
					TWCR = (1U<<TWINT)|(1U<<TWEN)| (1U<<TWSTO);
					if (TWSR==TW_NO_INFO)
					{
						write_no_error = TRUE;
					}
					else
					{
						errorsCount++;
					}
				}
			}
		}
	}
	
	return write_no_error;
}


/************************************************************************/
/*Read many registers                                                   */
/************************************************************************/
Boolean DrvTwiReadRegBuf(Int8U slave_address, Int8U slave_register, Int8U *buffer, Int8U buffer_size) 
{
	Boolean read_no_error = FALSE;
	
	//Send start
	TWCR = (1U<<TWINT)|(1U<<TWSTA)|(1U<<TWEN);
	DrvTwiWaitTransmission();
	if (((TWSR==TW_START) || (TWSR==TW_REP_START)) )
	{
		//send slave address write
		TWDR =  (slave_address << 1) | TW_WRITE;
		TWCR = (1U<<TWINT) | (1U<<TWEN);
		DrvTwiWaitTransmission();
		if( (TWSR == TW_MT_SLA_ACK) )
		{
			//send register address
			TWDR = slave_register;
			TWCR = (1U<<TWINT) | (1U<<TWEN);
			DrvTwiWaitTransmission();
			if ( (TWSR == TW_MT_DATA_ACK) )
			{
				//repeat start
				TWCR = (1U<<TWINT) | (1U<<TWSTA) | (1U<<TWEN);
				DrvTwiWaitTransmission();
				if ( (TWSR == TW_REP_START) )
				{
					//send slave address read
					TWDR =  (slave_address << 1 ) | TW_READ;
					TWCR = (1U<<TWINT) | (1U<<TWEN);
					DrvTwiWaitTransmission();
					if ( (TWSR == TW_MR_SLA_ACK) )
					{
						//for each data to read
						for ( Int8U loop_read_twi = 0 ; loop_read_twi < buffer_size; loop_read_twi++ )
						{
							//send ack for each data to read
							if(  loop_read_twi != ( buffer_size - 1U ) )
							{
								TWCR = (1U<<TWINT) | (1U<<TWEA) | (1U<<TWEN); 
							}
							//last data send nack
							else
							{
								TWCR = (1U<<TWINT) | (1U<<TWEN);
							}
							DrvTwiWaitTransmission();
							*buffer++ = TWDR;
						}
													
						//send stop
						TWCR = (1U<<TWINT) | (1U<<TWEN) | (1U<<TWSTO);
						if (TWSR == TW_NO_INFO)
						{
							//no error
							read_no_error = TRUE;
						}
						else
						{
							errorsCount++;
						}
					}
				}
			}
		}
	}
	return read_no_error; 
}	

/************************************************************************/
/*Write many registers                                                  */
/************************************************************************/
Boolean DrvTwiWriteRegBuf(Int8U slave_address, Int8U slave_register, Int8U *buffer, Int8U buffer_size)
{
	Boolean o_success = TRUE;
	
	TWCR = (1U<<TWINT)|(1U<<TWSTA)|(1U<<TWEN);           // SEND START
	DrvTwiWaitTransmission();

	if  ( ((TWSR==TW_START) || (TWSR==TW_REP_START)) )
	{
		//send slave address
		TWDR =  (slave_address << 1 ) | TW_WRITE;
		TWCR = (1U << TWINT) | (1U << TWEN);
		DrvTwiWaitTransmission();
		if ( (TWSR==TW_MT_SLA_ACK) )
		{
			//send register address
			TWDR = slave_register;
			TWCR = (1U<<TWINT) | (1U<<TWEN);
			DrvTwiWaitTransmission();
			if ( (TWSR==TW_MT_DATA_ACK) )
			{
				for ( Int8U loop_write_twi = 0 ; loop_write_twi < buffer_size; loop_write_twi++ )
				{
					TWDR =  *buffer++;                                     // DATA TO WRITE
					TWCR = (1U<<TWINT) | (1U<<TWEN);                       // send clock
					DrvTwiWaitTransmission();
					
					if ( (TWSR!=TW_MT_DATA_ACK) )  
					{
						o_success = FALSE;
						errorsCount++;
						break;
					}
				}
				TWCR = (1U<<TWINT) | (1U<<TWEN)| (1U<<TWSTO);       // send stop
			}
		}
	}
	return o_success;
}


/************************************************************************/
/*write many register with same data                                    */
/************************************************************************/
Boolean DrvTwiFillRegBuf(Int8U slave_address, Int8U slave_register, Int8U data, Int8U buffer_size) 
{
	Boolean o_success = TRUE;
	
	TWCR = (1U<<TWINT)|(1U<<TWSTA)|(1U<<TWEN);           // SEND START
	DrvTwiWaitTransmission();

	if  ( ((TWSR==TW_START) || (TWSR==TW_REP_START)) )
	{
		//send slave address
		TWDR =  (slave_address << 1 ) | TW_WRITE;
		TWCR = (1U << TWINT) | (1U << TWEN);
		DrvTwiWaitTransmission();
		if ( (TWSR==TW_MT_SLA_ACK) )
		{
			//send register address
			TWDR = slave_register;
			TWCR = (1U<<TWINT) | (1U<<TWEN);
			DrvTwiWaitTransmission();
			if ( (TWSR==TW_MT_DATA_ACK) )
			{
				for ( Int8U loop_write_twi = 0 ; loop_write_twi < buffer_size; loop_write_twi++ )
				{
					TWDR =  data;                                     // DATA TO WRITE
					TWCR = (1U<<TWINT) | (1U<<TWEN);                       // send clock
					DrvTwiWaitTransmission();
					
					if ( (TWSR!=TW_MT_DATA_ACK) )
					{
						o_success = FALSE;
						errorsCount++;
						break;
					}
				}
				TWCR = (1U<<TWINT) | (1U<<TWEN)| (1U<<TWSTO);       // send stop
			}
		}
	}
	return o_success;
}


/************************************************************************/
/*permits to get the number of errors count                             */
/************************************************************************/
Int16U DrvTwiGetErrorsCount( void )
{
	return errorsCount;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

/************************************************************************/
/*On attend la transmission                                             */
/************************************************************************/
static void DrvTwiWaitTransmission( void )
{
	Int8U timeout_i2c = 0xFFU;
	while ( ! (TWCR & (1<<TWINT) ) ) 
	{
		timeout_i2c--;
		if ( timeout_i2c == 0) 
		{
			errorsCount++;
			TWCR = 0;
			break;
		}
	}
}