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

#include "SrvImuSimple.h"
#include "SrvUltrason.h"
#include "SrvComm.h"
#include "SrvWalk.h"
#include "SrvDisplay.h"
#include "SrvBodyMove.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
//#define HC_05_MODE_AT
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
//enum used for parsing the incoming frame
typedef enum {
	COMM_START,
	COMM_HEADER,
	COMM_CLUSTER_0,
	COMM_CLUSTER_1,
	COMM_COMMAND_0,
	COMM_COMMAND_1,
	COMM_SIZE_0,
	COMM_SIZE_1,
	COMM_PARAMS,
} commMessageParsingState;

////////////////////////////////////////PRIVATE FONCTIONS/////////////////////////////////////////
//permits to decode the incoming data
static Boolean SrvCommDecodeMessage( void );
//permits to execute the incoming data
static Boolean SrvCommExecuteMessage( void );
#ifdef HC_05_MODE_AT
static Boolean SrvCommExecuteMessageHC05( void );
#endif
//send message
static Boolean SrvCommWriteMessage(uint8_t len );
//Prepare Message
static int SrvCommPrepareMessage(ECluster cluster, uint8_t cmd,  uint8_t size);

//permits to execute the incoming data in cluster
static Boolean SrvCommExecuteClusterGeneral( void );
static Boolean SrvCommExecuteClusterBehavior( void );
static Boolean SrvCommExecuteClusterIMU( void );
static Boolean SrvCommExecuteClusterUltrason( void );
static Boolean SrvCommExecuteClusterServo( void );
static Boolean SrvCommExecuteClusterLeg( void );
static Boolean SrvCommExecuteClusterDisplay( void );

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
volatile Int16U nbDataAvailable = 0U;
volatile commMessageParsingState status = COMM_START;
volatile commMessage inMessage;
volatile char response [100U] = { 0U };
uint32_t prevMillisPositionUpdate = 0U;

EUART uartUsing = E_UART_0;
/************************************************************************/
/*init de la communication                                              */
/************************************************************************/
Boolean SrvCommInit (void) 
{
	status = COMM_START;
	inMessage.header = 0U;
	inMessage.cluster = COMM_CLUSTER_UNDEFINED;
	inMessage.command = COMM_CLUSTER_COMMAND_UNDEFINED;
	inMessage.size = 0U;
	for(Int16U loop = 0U; loop < 50U ; loop++)
	{
		inMessage.data[loop] = 0U;
	}
	#ifdef HC_05_MODE_AT
		uartUsing = uartUsing;
		//DrvUartSetBaudRate(uartUsing, 38400);
		DrvUartFillTxBuffer(uartUsing,'A');
		DrvUartFillTxBuffer(uartUsing,'T');
		DrvUartFillTxBuffer(uartUsing,'\r');
		DrvUartFillTxBuffer(uartUsing,'\n');
		DrvUartSendData(uartUsing);
	#endif
	
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
		//inMessage.cluster = COMM_CLUSTER_IMU;
		//inMessage.command = COMM_CLUSTER_IMU_COMMAND_GET_RPY;
		//inMessage.size = 0U;
		//SrvCommExecuteMessage();
		//for next time
		prevMillisPositionUpdate = DrvTickGetTimeMs();
	}
	
	#ifdef HC_05_MODE_AT
		SrvCommExecuteMessageHC05();
	#endif
}


//permits to execute the incoming data
static Boolean SrvCommExecuteMessage( void )
{
	Boolean oSuccess = FALSE;
	//check command 
	if( inMessage.cluster == COMM_CLUSTER_GENARAL)
	{
		oSuccess = SrvCommExecuteClusterGeneral();
	}
	else if( inMessage.cluster == COMM_CLUSTER_BEHAVIOR)
	{
		oSuccess = SrvCommExecuteClusterBehavior();
	}
	else if( inMessage.cluster == COMM_CLUSTER_IMU)
	{
		oSuccess = SrvCommExecuteClusterIMU();
	}
	else if( inMessage.cluster == COMM_CLUSTER_ULTRASON)
	{
		oSuccess = SrvCommExecuteClusterUltrason();
	}
	else if( inMessage.cluster == COMM_CLUSTER_SERVOS)
	{	
		oSuccess = SrvCommExecuteClusterServo();
	}
	else if( inMessage.cluster == COMM_CLUSTER_LEG)
	{	
		oSuccess = SrvCommExecuteClusterLeg();
	}
	else if( inMessage.cluster == COMM_CLUSTER_DISPLAY)
	{
		oSuccess = SrvCommExecuteClusterDisplay();
	}
	
	return oSuccess;
}
//create message
static int SrvCommPrepareMessage(ECluster cluster, uint8_t cmd,  uint8_t size)
{
	return sprintf((char *)response,"%d%02X%02X%02X",
	 COMM_HEADER_MESSAGE,
	 cluster,
	 cmd,
	 size);
}
//send message
static Boolean SrvCommWriteMessage(uint8_t len )
{
	DrvUartFillTxBuffer(uartUsing,'<');
	//fill buffer
	for(Int16U loop = 0U; loop < len ; loop++)
	{
		//get pointer and loop on each useful data
		DrvUartFillTxBuffer(uartUsing,((Int8U *)response)[loop]);
	}
	DrvUartFillTxBuffer(uartUsing,'>');
	
	//can send over uart
	return DrvUartSendData(uartUsing);
}
//permits to decode the incoming data
uint8_t countData = 0U;
static Boolean SrvCommDecodeMessage( void )
{
	nbDataAvailable = DrvUartDataAvailable(uartUsing);
	
	//loop until the data is over
	for(Int16U loop = 0U; loop < nbDataAvailable ; loop++)
	{
		//get the datum
		volatile Int8U datum = DrvUartReadData(uartUsing);
		#ifdef HC_05_MODE_AT
			inMessage.data[ inMessage.size ] = datum ;
			inMessage.size++;
		//switch
		#else
		switch((Int8U) status)
		{
			case COMM_START:
				//wait for start and end
				if( datum == '<' )
				{
					//new frame is coming
					status = COMM_HEADER;
					//reset pointer
					countData = 0U;
				}
			break;
			case COMM_HEADER:
				//get header
				inMessage.header = asciiHexToInt(datum) ;
				status = COMM_CLUSTER_0;
			break;
			case COMM_CLUSTER_0:
				//get cluster id
				inMessage.cluster = asciiHexToInt(datum) * 16U;
				status = COMM_CLUSTER_1;
			break;
			case COMM_CLUSTER_1:
				//get cluster id
				inMessage.cluster += asciiHexToInt(datum) ;
				status = COMM_COMMAND_0;
			break;
			
			case COMM_COMMAND_0:
				//get command id
				inMessage.command = asciiHexToInt(datum) * 16U;
				status = COMM_COMMAND_1;
			break;
			case COMM_COMMAND_1:
				//get command id
				inMessage.command += asciiHexToInt(datum) ;
				status = COMM_SIZE_0;
			break;
			
			case COMM_SIZE_0:
				//get size
				inMessage.size = asciiHexToInt(datum) * 16U;
				status = COMM_SIZE_1;
			break;
			case COMM_SIZE_1:
				//get size
				inMessage.size += asciiHexToInt(datum) ;
				status = COMM_PARAMS;
			break;
			
			case COMM_PARAMS:
				//if end of frame
				if( datum == '>' )
				{
					//in all case return to wait a start
					status = COMM_START;
					//check frame integrity
					if(countData == inMessage.size)
					{
						//execute incoming message
						if(SrvCommExecuteMessage())
						{
							response[0U] = 'O';
							response[1U] = 'K';
							SrvCommWriteMessage(2U);
							return TRUE;
						}
						else
						{
							response[0U] = 'K';
							response[1U] = 'O';
							SrvCommWriteMessage(2U);
							return FALSE;
						}
					}
				}
				else
				{
					//fill buffer
					inMessage.data[ countData ] = asciiHexToInt(datum) ;
					countData++;
				}
			break;
		}
		#endif
	}	
	return FALSE;
}


//permits to execute the incoming data in cluster
static Boolean SrvCommExecuteClusterGeneral( void )
{
	if(( inMessage.command == COMM_CLUSTER_GENARAL_COMMAND_VERSION) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage( COMM_CLUSTER_GENARAL,COMM_CLUSTER_GENARAL_COMMAND_VERSION, 2U);
		nbData += sprintf((char*)&response[ nbData ],"%01X%01X", VERSION_SOFTWARE, VERSION_HARDWARE);
		return SrvCommWriteMessage(nbData);
	}
	return FALSE;
}
static Boolean SrvCommExecuteClusterBehavior( void )
{
	if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_GET_BEHAVIOR) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_BEHAVIOR,COMM_CLUSTER_BEHAVIOR_COMMAND_GET_BEHAVIOR, 1U);
		nbData += sprintf((char*)&response[ nbData ],"%01X", SrvBodyMoveGetBehavior());
		return SrvCommWriteMessage(nbData);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_BEHAVIOR) && (inMessage.size == 5U))
	{
		//prepare output string
		uint16_t delay = inMessage.data[ 1U ] * 4096 + inMessage.data[ 2U ] * 256 + inMessage.data[ 3U ] * 16 + inMessage.data[ 4U ];
		return SrvBodyMoveSetBehavior(inMessage.data[ 0U ], delay);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_TRANSLATION_XYZ) && (inMessage.size == 10U))
	{
		//prepare output string
		Int8S x = (Int8S)(inMessage.data[ 0U ] * 16 + inMessage.data[ 1U ] );
		Int8S y = (Int8S)(inMessage.data[ 2U ] * 16 + inMessage.data[ 3U ] );
		Int8S z = (Int8S)(inMessage.data[ 4U ] * 16 + inMessage.data[ 5U ] );
		uint16_t delay = inMessage.data[ 6U ] * 4096 + inMessage.data[ 7U ] * 256 + inMessage.data[ 8U ] * 16 + inMessage.data[ 9U ];
		return SrvBodyMoveSetTranslation(x,y,z,delay);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_POSITION_XYZ) && (inMessage.size == 10U))
	{
		//prepare output string
		Int8S roll = (Int8S)(inMessage.data[ 0U ] * 16 + inMessage.data[ 1U ] );
		Int8S pitch = (Int8S)(inMessage.data[ 2U ] * 16 + inMessage.data[ 3U ] );
		Int8S yaw = (Int8S)(inMessage.data[ 4U ] * 16 + inMessage.data[ 5U ] );
		uint16_t delay = inMessage.data[ 6U ] * 4096 + inMessage.data[ 7U ] * 256 + inMessage.data[ 8U ] * 16 + inMessage.data[ 9U ];
		return SrvBodyMoveSetRollPitchYaw(roll,pitch,yaw,delay);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_WALK) && (inMessage.size == 5U))
	{
		//prepare output string
		E_WALK walk = (E_WALK)inMessage.data[ 0U ];
		uint16_t delay = inMessage.data[ 1U ] * 4096 + inMessage.data[ 2U ] * 256 + inMessage.data[ 3U ] * 16 + inMessage.data[ 4U ];
		return SrvWalkSetWalk(walk,delay);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_GAIT) && (inMessage.size == 5U))
	{
		//prepare output string
		E_GAIT gait = (E_WALK)inMessage.data[ 0U ];
		uint16_t delay = inMessage.data[ 1U ] * 4096 + inMessage.data[ 2U ] * 256 + inMessage.data[ 3U ] * 16 + inMessage.data[ 4U ];
		return SrvWalkSetGait(gait,delay);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_AMPLITUDE) && (inMessage.size == 2U))
	{
		//prepare output string
		Int8U amplitude = (Int8U)(inMessage.data[ 0U ] * 16 + inMessage.data[ 1U ] ) * 1.0;
		return SrvWalkSetAmplitude(amplitude);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_GROUND_SIZE) && (inMessage.size == 6U))
	{
		//prepare output string
		uint16_t delay = inMessage.data[ 2U ] * 4096 + inMessage.data[ 3U ] * 256 + inMessage.data[ 4U ] * 16 + inMessage.data[ 5U ];
		return SrvBodyMoveSetGroundSize((inMessage.data[ 0U ] * 16 + inMessage.data[ 1U ] ) * 1.0,delay);
	}
	else if(( inMessage.command == COMM_CLUSTER_BEHAVIOR_COMMAND_SET_ELEVATION) && (inMessage.size == 6U))
	{
		//prepare output string
		uint16_t delay = inMessage.data[ 2U ] * 4096 + inMessage.data[ 3U ] * 256 + inMessage.data[ 4U ] * 16 + inMessage.data[ 5U ];
		return SrvBodyMoveSetElevation((inMessage.data[ 0U ] * 16 + inMessage.data[ 1U ] ) * 1.0,delay);
	}
	
	
	return FALSE;
}
static Boolean SrvCommExecuteClusterIMU( void )
{
	if(( inMessage.command == COMM_CLUSTER_IMU_COMMAND_GET_THRESHOLD) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_IMU,COMM_CLUSTER_IMU_COMMAND_GET_THRESHOLD, 2U);
		nbData += sprintf((char*)&response[ nbData ],"%02X", SrvImuSimpleGetThreshold());
		return SrvCommWriteMessage(nbData);
	}
	else if(( inMessage.command == COMM_CLUSTER_IMU_COMMAND_SET_THRESHOLD) && (inMessage.size == 2U))
	{
		//prepare output string
		SrvImuSimpleSetThreshold((uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]));
		//can send data
		return TRUE;
	}
	else if(( inMessage.command == COMM_CLUSTER_IMU_COMMAND_GET_ROLL_PITCH_YAW) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_IMU,COMM_CLUSTER_IMU_COMMAND_GET_ROLL_PITCH_YAW, 12U);
		nbData += sprintf((char*)&response[ nbData ],"%04X", SrvImuSimpleGetRoll());
		nbData += sprintf((char*)&response[ nbData ],"%04X", SrvImuSimpleGetPitch());
		nbData += sprintf((char*)&response[ nbData ],"%04X", SrvImuSimpleGetYaw());
		return SrvCommWriteMessage(nbData);
	}
	else if(( inMessage.command == COMM_CLUSTER_IMU_COMMAND_GET_ROLL) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_IMU,COMM_CLUSTER_IMU_COMMAND_GET_ROLL, 4U);
		nbData += sprintf((char*)&response[ nbData ],"%04X", SrvImuSimpleGetRoll());
		return SrvCommWriteMessage(nbData);
	}
	else if(( inMessage.command == COMM_CLUSTER_IMU_COMMAND_GET_PITCH) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_IMU,COMM_CLUSTER_IMU_COMMAND_GET_PITCH, 4U);
		nbData += sprintf((char*)&response[ nbData ],"%04X", SrvImuSimpleGetPitch());
		return SrvCommWriteMessage(nbData);
	}
	else if(( inMessage.command == COMM_CLUSTER_IMU_COMMAND_GET_YAW) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_IMU,COMM_CLUSTER_IMU_COMMAND_GET_YAW, 4U);
		nbData += sprintf((char*)&response[ nbData ],"%04X", SrvImuSimpleGetYaw());
		return SrvCommWriteMessage(nbData);
	}
	return FALSE;
}
static Boolean SrvCommExecuteClusterUltrason( void )
{
	if(( inMessage.command == COMM_CLUSTER_ULTRASON_COMMAND_GET_THRESHOLD) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_ULTRASON,COMM_CLUSTER_ULTRASON_COMMAND_GET_THRESHOLD, 2U);
		nbData += sprintf((char*)&response[ nbData ],"%02X", SrvUltrasonGetThreshold());
		return SrvCommWriteMessage(nbData);
	}
	else if(( inMessage.command == COMM_CLUSTER_ULTRASON_COMMAND_SET_THRESHOLD) && (inMessage.size == 2U))
	{
		//prepare output string
		SrvUltrasonSetThreshold((uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]));
		//can send data
		return TRUE;
	}
	else if(( inMessage.command == COMM_CLUSTER_ULTRASON_COMMAND_GET_DISTANCE) && (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_ULTRASON,COMM_CLUSTER_ULTRASON_COMMAND_GET_DISTANCE, 4U);
		nbData += sprintf((char*)&response[ nbData ],"%02X", SrvUltrasonGetDistance(E_US_0));
		nbData += sprintf((char*)&response[ nbData ],"%02X", SrvUltrasonGetDistance(E_US_1));
		return SrvCommWriteMessage(nbData);
	}
	return FALSE;
}
static Boolean SrvCommExecuteClusterServo( void )
{
	uint8_t servoId = 0xFFU;
	SServo *servo;
	if(( inMessage.command ==  COMM_CLUSTER_SERVOS_COMMAND_STATUS_SERVO) && (inMessage.size == 2U))
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			//prepare output string
			uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_SERVOS,COMM_CLUSTER_SERVOS_COMMAND_STATUS_SERVO, 3U);
			servo = DrvServoGetStruture(servoId);
			nbData += sprintf((char*)&response[ nbData ],"%02X%01X",servoId,servo->enable);
			return SrvCommWriteMessage(nbData);
		}
	}
	else if(( inMessage.command ==  COMM_CLUSTER_SERVOS_COMMAND_ENABLE_SERVO) && (inMessage.size == 3U))
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]);
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			Boolean activate = inMessage.data[ 2U ];
			//can send data
			DrvServoActivate(servoId, activate);
			return TRUE;
		}
	}
	else if(( inMessage.command ==  COMM_CLUSTER_SERVOS_COMMAND_SERVO_READ) && (inMessage.size == 2U))
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			//prepare output string
			servo = DrvServoGetStruture(servoId);
			uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_SERVOS,COMM_CLUSTER_SERVOS_COMMAND_SERVO_READ, 4U);
			Int8U servoPos = (Int8U)(servo->targetPosition - servo->offset);
			nbData += sprintf((char*)&response[ nbData ],"%02X%02X",servoId,servoPos);
			return SrvCommWriteMessage(nbData);
		}
	}
	else if(( inMessage.command ==  COMM_CLUSTER_SERVOS_COMMAND_SERVO_WRITE) && (inMessage.size == 8U))
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			servo = DrvServoGetStruture(servoId);
			Int8S servoPos = servo->offset + (Int8S)(inMessage.data[ 2U ] * 16 + inMessage.data[ 3U ] );
			uint16_t delay = inMessage.data[ 4U ] * 4096 + inMessage.data[ 5U ] * 256 + inMessage.data[ 6U ] * 16 + inMessage.data[ 7U ];
			//can send data
			return DrvServoSetTarget(servoId,servoPos, delay);
		}
	}
	else if(( inMessage.command ==  COMM_CLUSTER_SERVOS_COMMAND_SERVO_MIN_READ) && (inMessage.size == 2U))
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			//prepare output string
			servo = DrvServoGetStruture(servoId);
			uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_SERVOS,COMM_CLUSTER_SERVOS_COMMAND_SERVO_MIN_READ, 4U);
			Int8U min = (Int8U)(servo->min - servo->offset);
			nbData += sprintf((char*)&response[ nbData ],"%02X%02X",servoId,min);
			return SrvCommWriteMessage(nbData);
		}
	}
	else if(( inMessage.command ==  COMM_CLUSTER_SERVOS_COMMAND_SERVO_MAX_READ) && (inMessage.size == 2U))
	{
		servoId = (uint8_t)((inMessage.data[ 0U ] * 16U) + inMessage.data[ 1U ]);
		//if legs
		if(servoId < NB_LEGS * NB_SERVOS_PER_LEG)
		{
			//prepare output string
			servo = DrvServoGetStruture(servoId);
			uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_SERVOS,COMM_CLUSTER_SERVOS_COMMAND_SERVO_MAX_READ, 4U);
			Int8U max = (Int8U)(servo->max - servo->offset);
			nbData += sprintf((char*)&response[ nbData ],"%02X%02X",servoId,max);
			return SrvCommWriteMessage(nbData);
		}
	}
	else if(( inMessage.command ==  COMM_CLUSTER_SERVOS_COMMAND_SERVO_READ_ALL)&& (inMessage.size == 0U))
	{
		//prepare output string
		uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_SERVOS,COMM_CLUSTER_SERVOS_COMMAND_SERVO_READ_ALL, 18U * 2U);
		for( uint8_t servoId = 0U; servoId < NB_LEGS * NB_SERVOS_PER_LEG ; servoId++ )
		{
			servo = DrvServoGetStruture(servoId);
			Int8U mid = (Int8U)(servo->targetPosition - servo->offset);
			nbData += sprintf((char*)&response[ nbData ],"%02X",mid);
		}
		return SrvCommWriteMessage(nbData);
	}
	return FALSE;
}
static Boolean SrvCommExecuteClusterLeg( void )
{
	uint8_t legId = NB_LEGS;
	SLeg *leg;
	if(( inMessage.command == COMM_CLUSTER_LEG_COMMAND_SET_LEG_XYZ) && (inMessage.size == 17U))
	{
		legId = inMessage.data[ 0U ];
		if(legId < NB_LEGS)
		{
			Int16S x = inMessage.data[ 1U ] * 4096 + inMessage.data[ 2U ] * 256 + inMessage.data[ 3U ] * 16 + inMessage.data[ 4U ];
			Int16S y = inMessage.data[ 5U ] * 4096 + inMessage.data[ 6U ] * 256 + inMessage.data[ 7U ] * 16 + inMessage.data[ 8U ];
			Int16S z = inMessage.data[ 9U ] * 4096 + inMessage.data[ 10U ] * 256 + inMessage.data[ 11U ] * 16 + inMessage.data[ 12U ];
			uint32_t delay = inMessage.data[ 13U ] * 4096 + inMessage.data[ 14U ] * 256 + inMessage.data[ 15U ] * 16 + inMessage.data[ 16U ];
			return DrvLegSetTarget(legId,x,y,z, delay);
		}
	}
	else if(( inMessage.command == COMM_CLUSTER_LEG_COMMAND_GET_LEG_XYZ) && (inMessage.size == 1U))
	{
		legId = inMessage.data[ 0U ];
		if(legId < NB_LEGS)
		{
			//prepare output string
			leg = DrvLegGetStruct(legId);
			uint8_t nbData = SrvCommPrepareMessage(COMM_CLUSTER_LEG,COMM_CLUSTER_LEG_COMMAND_GET_LEG_XYZ, 13U);
			Int16S x = (Int16S)leg->currentPositionX;
			Int16S y = (Int16S)leg->currentPositionY;
			Int16S z = (Int16S)leg->currentPositionZ;
			nbData += sprintf((char*)&response[ nbData ],"%01X%04X%04X%04X",legId,x,y,z);
			return SrvCommWriteMessage(nbData);
		}
	}
	return FALSE;
}
static Boolean SrvCommExecuteClusterDisplay( void )
{
	return FALSE;
}




#ifdef HC_05_MODE_AT
static Int8U hc05 = 0;
static Boolean SrvCommExecuteMessageHC05( void )
{
	if((inMessage.data[ inMessage.size - 2U ] == '\r' ) && (inMessage.data[ inMessage.size - 1U ] == '\n'))
	{
		if((hc05 == 0) &&(inMessage.size == 4)) // receive OK
		{
			hc05 = 1;
			DrvUartFillTxBuffer(uartUsing,'A');
			DrvUartFillTxBuffer(uartUsing,'T');
			
			DrvUartFillTxBuffer(uartUsing,'+');
			
			DrvUartFillTxBuffer(uartUsing,'N');
			DrvUartFillTxBuffer(uartUsing,'A');
			DrvUartFillTxBuffer(uartUsing,'M');
			DrvUartFillTxBuffer(uartUsing,'E');
			
			DrvUartFillTxBuffer(uartUsing,'=');
			
			DrvUartFillTxBuffer(uartUsing,'H');
			DrvUartFillTxBuffer(uartUsing,'e');
			DrvUartFillTxBuffer(uartUsing,'x');
			DrvUartFillTxBuffer(uartUsing,'a');
			DrvUartFillTxBuffer(uartUsing,'p');
			DrvUartFillTxBuffer(uartUsing,'o');
			DrvUartFillTxBuffer(uartUsing,'d');
			
			DrvUartFillTxBuffer(uartUsing,'\r');
			DrvUartFillTxBuffer(uartUsing,'\n');
			DrvUartSendData(uartUsing);
		}
		else if((hc05 == 1) &&(inMessage.size == 4)) // receive OK
		{
			hc05 = 2;
			DrvUartFillTxBuffer(uartUsing,'A');
			DrvUartFillTxBuffer(uartUsing,'T');
			
			DrvUartFillTxBuffer(uartUsing,'+');
			
			DrvUartFillTxBuffer(uartUsing,'U');
			DrvUartFillTxBuffer(uartUsing,'A');
			DrvUartFillTxBuffer(uartUsing,'R');
			DrvUartFillTxBuffer(uartUsing,'T');
			
			DrvUartFillTxBuffer(uartUsing,'=');
			
			DrvUartFillTxBuffer(uartUsing,'1');
			DrvUartFillTxBuffer(uartUsing,'1');
			DrvUartFillTxBuffer(uartUsing,'5');
			DrvUartFillTxBuffer(uartUsing,'2');
			DrvUartFillTxBuffer(uartUsing,'0');
			DrvUartFillTxBuffer(uartUsing,'0');
			DrvUartFillTxBuffer(uartUsing,',');
			DrvUartFillTxBuffer(uartUsing,'0');
			DrvUartFillTxBuffer(uartUsing,',');
			DrvUartFillTxBuffer(uartUsing,'0');
			
			DrvUartFillTxBuffer(uartUsing,'\r');
			DrvUartFillTxBuffer(uartUsing,'\n');
			DrvUartSendData(uartUsing);
		}
		else if((hc05 == 2) &&(inMessage.size == 4)) // receive OK
		{
			hc05 = 3;
			DrvUartFillTxBuffer(uartUsing,'A');
			DrvUartFillTxBuffer(uartUsing,'T');
			
			DrvUartFillTxBuffer(uartUsing,'+');
			
			DrvUartFillTxBuffer(uartUsing,'P');
			DrvUartFillTxBuffer(uartUsing,'S');
			DrvUartFillTxBuffer(uartUsing,'W');
			DrvUartFillTxBuffer(uartUsing,'D');
			
			DrvUartFillTxBuffer(uartUsing,'=');
			
			DrvUartFillTxBuffer(uartUsing,'4');
			DrvUartFillTxBuffer(uartUsing,'8');
			DrvUartFillTxBuffer(uartUsing,'5');
			DrvUartFillTxBuffer(uartUsing,'0');
			
			DrvUartFillTxBuffer(uartUsing,'\r');
			DrvUartFillTxBuffer(uartUsing,'\n');
			DrvUartSendData(uartUsing);
		}
		inMessage.size = 0;
	}
	return TRUE;
}
#endif