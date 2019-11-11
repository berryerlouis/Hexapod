/*
 * SrvComm.c
 *
 * Created: 31/05/2012 16:04:33
 *  Author: berryer
 */ 

#include "Conf/ConfHard.h"
#include "Drv/DrvUart.h"
#include "Drv/DrvServo.h"
#include "Drv/DrvLeg.h"
#include "Drv/DrvTick.h"
#include "SrvComm.h"
#include "SrvWalk.h"
#include "SrvDisplay.h"


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

uint32_t prevMillisPositionUpdate = 0U;
/************************************************************************/
/*init de la communication                                              */
/************************************************************************/
Boolean SrvCommInit (void) 
{
	status = COMM_START;
	inMessage.header = 0;
	inMessage.command = 0;
	inMessage.size = 0;
	return TRUE;
}	

/************************************************************************/
/*dispatcher d'evenements                                               */
/************************************************************************/
void SrvCommUpdate ( void ) 
{
	SrvCommDecodeMessage();
	if ((DrvTickGetTimeMs() - prevMillisPositionUpdate) > 250U)
	{
		inMessage.command = COMM_COMMAND_SERVO_READ;
		inMessage.data[ 0U ] = 1;
		inMessage.data[ 1U ] = 8;
		SrvCommExecuteMessage();
		//for next time
		prevMillisPositionUpdate = DrvTickGetTimeMs();
	}
}

//permits to execute the incoming data
static Boolean SrvCommExecuteMessage( void )
{
	Boolean oSuccess = FALSE;
	uint8_t servoId = 0xFFU;
	uint8_t legId = 0xFFU;
	SServo *servo;
	uint8_t response [100U] = { 0U };
		
	//check command 
	if( inMessage.command ==  COMM_COMMAND_VERSION)
	{
		//prepare output string
		uint8_t responseVersion [ 4U ] = { COMM_HEADER_MESSAGE ,COMM_COMMAND_VERSION, VERSION_SOFTWARE, VERSION_HARDWARE };
		SrvCommWriteMessage(responseVersion, 4U);
				
		//can send data
		oSuccess = TRUE;
	}
	else if( inMessage.command ==  COMM_COMMAND_ENABLE_SERVO)
	{
		servoId = (uint8_t)((inMessage.data [ 0U ] * 10U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			Boolean activate = inMessage.data[ 2U ];
			//can send data
			DrvServoActivate(servoId, activate);
			oSuccess = TRUE;
		}
	}
	else if( inMessage.command ==  COMM_COMMAND_GET_SERVO_ENABLE)
	{	
		servoId = (uint8_t)((inMessage.data [ 0U ] * 10U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			//can send data
			servo = DrvServoGetStruture(servoId); 
			
			response[ 0U ] = '0'; //header
			response[ 1U ] = COMM_COMMAND_GET_SERVO_ENABLE; //command
			if( servoId >= 10)
			{
				itoa(servoId,(char*)&response[ 2U ],10); //servo id
			}
			else if( servoId >= 0)
			{
				response[ 2U ] = '0';
				itoa(servoId,(char*)&response[ 3U ],10); //servo id
			}
			response[ 4U ] = servo->enable ? '1' : '0';
			SrvCommWriteMessage(response, 5U);
			oSuccess = TRUE;
		}	
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
			
			formatMessage(servoId, servo->targetPosition - servo->mid + 90, servo->movingTime, response);
			SrvCommWriteMessage(response, 11U);
			//can send data
			oSuccess = TRUE;
		}
		else if(servoId == NB_LEGS * NB_SERVOS_PER_LEG)
		{
				
			response[ 0U ] = '0'; //header
			response[ 1U ] = COMM_COMMAND_SERVO_READ; //command
			response[ 2U ] = '1'; //servoId
			response[ 3U ] = '8'; //command
			
			for( uint8_t servoId = 0U; servoId < NB_LEGS * NB_SERVOS_PER_LEG ; servoId++ )
			{ 
				servo = DrvServoGetStruture(servoId);  
				if( servo->targetPosition - servo->mid + 90 >= 100)
				{
					itoa(servo->targetPosition - servo->mid + 90,(char*)&response[ (3U * servoId) + 4U ],10);
				}
				else if( servo->targetPosition - servo->mid + 90 >= 10)
				{
					response[ (3U * servoId) + 4U ] = '0';
					itoa(servo->targetPosition - servo->mid + 90,(char*)&response[ (3U * servoId) + 5U ],10);
				}
				else if( servo->targetPosition - servo->mid + 90 >= 0)
				{
					response[ (3U * servoId) + 4U ] = '0';
					response[ (3U * servoId) + 5U ] = '0';
					itoa(servo->targetPosition - servo->mid + 90,(char*)&response[ (3U * servoId) + 6U ],10);
				}
			}
			SrvCommWriteMessage(response, 4U + 18U * 3U);
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
			servo = DrvServoGetStruture(servoId);  
			int16_t servoPos =  inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
			uint16_t delay = inMessage.data[ 5U ] * 1000 + inMessage.data[ 6U ] * 100 + inMessage.data[ 7U ] * 10 + inMessage.data[ 8U ];
			//can send data
			oSuccess = DrvServoSetTarget(servoId,servoPos + servo->mid - 90 ,delay);
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
			formatMessageMinMax(servoId, servo->min , response);
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
			formatMessageMinMax(servoId, servo->max , response);
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
		//uint16_t delay = inMessage.data[ 1U ] * 1000 + inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
		//oSuccess = SrvWalkSetPosition((E_BODY_POSITION)inMessage.data[ 0U ], delay);
	}
	else if( inMessage.command ==  COMM_COMMAND_SET_WALK)
	{
		uint16_t delay = inMessage.data[ 1U ] * 1000 + inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
		E_WALK walk = (E_WALK)inMessage.data[ 0U ];
		oSuccess = SrvWalkSetWalk(walk, delay);
		
		if(walk == E_WALK_FW)		SrvDisplaySetDirection(ARROW_UP);
		else if(walk == E_WALK_RV)		SrvDisplaySetDirection(ARROW_DOWN);
		else SrvDisplaySetDirection(ARROW_CENTER);
	}
	else if( inMessage.command ==  COMM_COMMAND_SET_MOVE)
	{
		uint16_t delay = inMessage.data[ 1U ] * 1000 + inMessage.data[ 2U ] * 100 + inMessage.data[ 3U ] * 10 + inMessage.data[ 4U ];
		oSuccess = SrvWalkSetGait((E_GAIT)inMessage.data[ 0U ], delay);
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