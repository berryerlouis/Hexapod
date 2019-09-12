/*
 * SrvComm.c
 *
 * Created: 31/05/2012 16:04:33
 *  Author: berryer
 */ 

#include "../Conf/ConfHard.h"
#include "../Drv/DrvUart.h"
#include "../Drv/DrvServo.h"
#include "../Drv/DrvLeg.h"
#include "SrvComm.h"
#include "SrvBody.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define WRITE_OK "OK"
#define WRITE_KO "KO"
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
//enum used for parsing the incoming frame
typedef enum {
	COMM_START,
	COMM_HEADER,
	COMM_COMMAND,
	COMM_PARAMS,
} commMessageParsingState;

////////////////////////////////////////PRIVATE FONCTIONS/////////////////////////////////////////
//permits to decode the incoming data
static Boolean SrvCommDecodeMessage( void );
//permits to execute the incoming data
static Boolean SrvCommExecuteMessage( void );
//send message
static Boolean SrvCommWriteMessage( uint8_t *outcomFrame, uint8_t len );
static void formatMessage ( uint8_t servoId, uint8_t servoPos, uint8_t servoDelay, uint8_t *message);
static void formatMessageMinMax ( uint8_t servoId, uint8_t servoPos, uint8_t *message);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int16U nbDataAvailable = 0U;
commMessageParsingState status = COMM_START;
commMessage inMessage;

/************************************************************************/
/*init de la communication                                              */
/************************************************************************/
Boolean SrvCommInit (void) 
{
	status = COMM_START;
	inMessage.header = 0;
	inMessage.command = 0;
	inMessage.size = 0;
	return DrvUartInit( E_UART_0, UART_SPEED_115200 );
}	

/************************************************************************/
/*dispatcher d'evenements                                               */
/************************************************************************/
void SrvCommUpdate ( void ) 
{
	SrvCommDecodeMessage();
}

//permits to execute the incoming data
static Boolean SrvCommExecuteMessage( void )
{
	Boolean oSuccess = FALSE;
	uint8_t servoId = 0xFFU;
	uint8_t legId = 0xFFU;
	SServo *servo;
	uint8_t response [20U] = { 0U };
		
	//check command 
	if( inMessage.command ==  COMM_COMMAND_VERSION)
	{
		//prepare output string
		uint8_t responseVersion [ 4U ] = { COMM_HEADER_MESSAGE ,COMM_COMMAND_VERSION, VERSION_SOFTWARE, VERSION_HARDWARE };
		SrvCommWriteMessage(responseVersion, 4U);
				
		//can send data
		oSuccess = TRUE;
	}
	else if( inMessage.command ==  COMM_COMMAND_SERVO_READ)
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 10U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			servo = DrvServoGetStruture(servoId);  
				
			response[ 0U ] = '0'; //header
			response[ 1U ] = COMM_COMMAND_SERVO_READ; //command
			formatMessage(servoId, servo->targetPosition, servo->movingTime, response);
			SrvCommWriteMessage(response, 11U);
			//can send data
			oSuccess = TRUE;
		}
	}
	else if( inMessage.command ==  COMM_COMMAND_SERVO_WRITE)
	{
		servoId = (uint8_t)((inMessage.data [ 0U ] * 10U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			int16_t servoPos =  inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
			uint16_t delay = inMessage.data[ 5U ] * 1000 + inMessage.data[ 6U ] * 100 + inMessage.data[ 7U ] * 10 + inMessage.data[ 8U ];
			
			DrvServoSetTarget(servoId,servoPos,delay);
			//can send data
			oSuccess = TRUE;
		}
	}
	else if( inMessage.command ==  COMM_COMMAND_SERVO_MIN_READ)
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 10U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			servo = DrvServoGetStruture(servoId);  
				
			response[ 0U ] = '0'; //header
			response[ 1U ] = COMM_COMMAND_SERVO_MIN_READ; //command
			formatMessageMinMax(servoId, servo->min, response);
			SrvCommWriteMessage(response, 7U);
			//can send data
			oSuccess = TRUE;
		}
	}
	else if( inMessage.command ==  COMM_COMMAND_SERVO_MAX_READ)
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 10U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			servo = DrvServoGetStruture(servoId);  
				
			response[ 0U ] = '0'; //header
			response[ 1U ] = COMM_COMMAND_SERVO_MAX_READ; //command
			formatMessageMinMax(servoId, servo->max, response);
			SrvCommWriteMessage(response, 7U);
			//can send data
			oSuccess = TRUE;
		}
	}
	else if( inMessage.command ==  COMM_COMMAND_LEG_WRITE)
	{		 
		legId  = (uint8_t)(inMessage.data[ 0U ]) ;
		
		DrvLegSetPosition(legId,
		inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ],
		inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ],
		inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ],
		inMessage.data[ 1U ] * 1000 + inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ]);
	}
	else if( inMessage.command ==  COMM_COMMAND_SET_POSITION)
	{
		uint16_t delay = inMessage.data[ 1U ] * 1000 + inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
		oSuccess = SrvBodySetPosition((E_BODY_POSITION)inMessage.data[ 0U ], delay);
	}
	else if( inMessage.command ==  COMM_COMMAND_SET_WALK)
	{
		uint16_t delay = inMessage.data[ 1U ] * 1000 + inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
		oSuccess = SrvBodySetWalk((E_WALK)inMessage.data[ 0U ], delay);
	}
	else if( inMessage.command ==  COMM_COMMAND_SET_MOVE)
	{
		uint16_t delay = inMessage.data[ 1U ] * 1000 + inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
		oSuccess = SrvBodySetGait((E_GAIT)inMessage.data[ 0U ], delay);
	}

	return oSuccess;
}

//send message
static Boolean SrvCommWriteMessage( uint8_t *outcomFrame, uint8_t len )
{
	DrvUartFillTxBuffer(E_UART_0,'<');
	//fill buffer
	for(Int16U loop = 0U; loop < len ; loop++)
	{
		//get pointer and loop on each usefull data
		DrvUartFillTxBuffer(E_UART_0,((Int8U *)outcomFrame)[loop]);
	}
	DrvUartFillTxBuffer(E_UART_0,'>');
	
	//can send over uart
	return DrvUartSendData(E_UART_0);
}

//permits to decode the incoming data
static Boolean SrvCommDecodeMessage( void )
{
	nbDataAvailable = DrvUartDataAvailable(E_UART_0);
	
	//loop until the data is over
	for(Int16U loop = 0U; loop < nbDataAvailable ; loop++)
	{
		//get the datum
		Int8U datum = DrvUartReadData(E_UART_0);
		//switch
		switch((Int8U) status)
		{
			case COMM_START:
				//wait for start and end
				if( datum == '<' )
				{
					//new frame is coming
					status = COMM_HEADER;
					//reset pointer
					inMessage.size = 0U;
				}
			break;
			case COMM_HEADER:
				//get header
				inMessage.header = datum;
				status = COMM_COMMAND;
			break;
			
			case COMM_COMMAND:
				//get command id
				inMessage.command = datum;
				status = COMM_PARAMS;
			break;
			
			case COMM_PARAMS:
				//if end of frame
				if( datum == '>' )
				{
					//reset pointer
					inMessage.size = 0U;
					status = COMM_START;
					//execute incoming message
					if(SrvCommExecuteMessage())
					{
						SrvCommWriteMessage((uint8_t*)WRITE_OK,2U);
						return TRUE;
					}
					else
					{
						SrvCommWriteMessage((uint8_t*)WRITE_KO,2U);
						return FALSE;
					}
				}
				else
				{
					//fill buffer
					inMessage.data[ inMessage.size ] = datum;
					if( (datum != '-') && (datum != '+') )
					{
						inMessage.data[ inMessage.size ] -= 0x30U;
					}
					inMessage.size++;
				}
			break;
		}
	}	
	return FALSE;
}

static void formatMessage ( uint8_t servoId, uint8_t servoPos, uint8_t servoDelay, uint8_t *message)
{
	if( servoId >= 10)
	{
		itoa(servoId,(char*)&message[ 2U ],10); //servo id
	}
	else if( servoId >= 0)
	{
		message[ 2U ] = '0';
		itoa(servoId,(char*)&message[ 3U ],10); //servo id
	}
	
	if( servoPos >= 100)
	{
		itoa(servoPos,(char*)&message[ 4U ],10);
	}
	else if( servoPos >= 10)
	{
		message[ 4U ] = '0';
		itoa(servoPos,(char*)&message[ 5U ],10);
	}
	else if( servoPos >= 0)
	{
		message[ 4U ] = '0';
		message[ 5U ] = '0';
		itoa(servoPos,(char*)&message[ 6U ],10);
	}
	
	if( servoDelay >= 1000)
	{
		itoa(servoDelay,(char*)&message[ 7U ],10);
	}
	else if( servoDelay >= 100)
	{
		message[ 7U ] = '0';
		itoa(servoDelay,(char*)&message[ 8U ],10);
	}
	else if( servoDelay >= 10)
	{
		message[ 7U ] = '0';
		message[ 8U ] = '0';
		itoa(servoDelay,(char*)&message[ 9U ],10);
	}
	else if( servoDelay >= 0)
	{
		message[ 7U ] = '0';
		message[ 8U ] = '0';
		message[ 9U ] = '0';
		itoa(servoDelay,(char*)&message[ 10U ],10);
	}
}

static void formatMessageMinMax ( uint8_t servoId, uint8_t servoPos, uint8_t *message)
{
	if( servoId >= 10)
	{
		itoa(servoId,(char*)&message[ 2U ],10); //servo id
	}
	else if( servoId >= 0)
	{
		message[ 2U ] = '0';
		itoa(servoId,(char*)&message[ 3U ],10); //servo id
	}
	
	if( servoPos >= 100)
	{
		itoa(servoPos,(char*)&message[ 4U ],10);
	}
	else if( servoPos >= 10)
	{
		message[ 4U ] = '0';
		itoa(servoPos,(char*)&message[ 5U ],10);
	}
	else if( servoPos >= 0)
	{
		message[ 4U ] = '0';
		message[ 5U ] = '0';
		itoa(servoPos,(char*)&message[ 6U ],10);
	}
}