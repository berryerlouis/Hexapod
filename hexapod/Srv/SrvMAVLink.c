////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "SrvMAVLink.h"
#include "SrvIhm.h"
#include "SrvImu.h"

#include "Drv/DrvUart.h"

////////////////////////////////////////PRIVATE MACROS////////////////////////////////////////////
#define CHECK_CRC( MavlinkCmd ) SrvMAVLinkComputeCRC((Int8U *)&inMessage.msg,MAVLINK_CORE_HEADER_LEN + inMessage.msg.len, MavlinkCmd##_MAGIC ) \
										== *(Int16U*)&inMessage.msg.payload[ incommingMessageSize - MAVLINK_LEN_WO_STX_AND_PAYLOAD - 1U ]

#define MAVLINK_SENT_MSG( MavlinkCmd ) {	MavlinkCmd,MavlinkCmd##_LEN,MavlinkCmd##_MAGIC}
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
const MavLinkCommonMsgId commonMsgId [] =
{
	/*	0	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_HEARTBEAT),	
	/*	1	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_PARAM_REQUEST_READ),
	/*	2	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_PARAM_REQUEST_LIST),
	/*	3	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_PARAM_VALUE),		
	/*	4	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_PARAM_SET	),		
	/*	5	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_MISSION_COUNT),		
	/*	6	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_MISSION_REQUEST_LIST) ,
	/*	7	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_REQUEST_DATA_STREAM),
	/*	8	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_COMMAND_LONG)	,
	/*	9	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_COMMAND_ACK),	
	/*	10	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_BATTERY_STATUS),
	/*	11	*/	MAVLINK_SENT_MSG(MAVLINK_MSG_ID_SCALED_IMU),
};


mavlinkParameter params[] =
{
	//sysID
	{"SYSID_SW_MREV"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},
	{"SYSID_SW_TYPE"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	10U		},
	{"SYSID_THISMAV"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	1U		},
	{"SYSID_MYGCS"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	255U	},
	//pilot
	{"PILOT_THR_FILT"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},
	{"PILOT_TKOFF_ALT"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},
	{"PILOT_TKOFF_DZ"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	100U	},
	{"PILOT_THR_BHV"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},
	{"PILOT_ACCEL_Z"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	1U		},//Pilot vertical acceleration
	//serial
	{"SERIAL0_BAUD"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	115U	},//115200
	{"SERIAL1_PROTOCOL"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	1U		},//MAVLINK
	//throttle
	{"THR_MIN"			,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	130U	},//Throttle Minimum
	{"THR_MID"			,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	500U	},//Throttle medium
	//tune
	{"TUNE"				,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//Tune PID
	{"TUNE_LOW"			,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	0U		},//Tuning minimum
	{"TUNE_HIGH"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	0U		},//Tuning maximum
	//FRAME
	{"FRAME"			,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	1U		},//X_FRAME
	//arm
	{"ARMING_CHECK"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	1U		},//enabling or disabling of pre-arming checks of receiver, accelerometer, barometer, compass and GPS
	{"DISARM_DELAY"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	10U		},//Disarm delay
	//angle
	{"ANGLE_MAX"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	2000U	},//Maximum lean angle in all flight modes
	//battery
	{"BATT_MONITOR"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	3U		},//Battery monitoring  0:Disabled,3:Analog Voltage Only,4:Analog Voltage and Current,5:SMBus,6:Bebop
	{"BATT_VOLT_PIN"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0xFFU	},//Battery Voltage sensing pin
	{"BATT_CURR_PIN"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0xFFU	},//Battery Current sensing pin
	{"BATT_VOLT_MULT"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//Voltage Multiplier
	{"BATT_AMP_PERVOLT"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//Amps per volt
	{"BATT_AMP_OFFSET"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//Voltage offset at zero current on current sensor
	{"BATT_CAPACITY"	,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	3300U	},//Capacity of the battery in mAh when full
	//motor
	{"MOT_SV_SPEED"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	100U	},//servo speed 100Hz
	//servo1
	{"MOT_SV1_MIN"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1100U	},//RC min PWM
	{"MOT_SV1_TRIM"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1500U	},//RC trim PWM
	{"MOT_SV1_MAX"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1900U	},//RC max PWM
	{"MOT_SV1_REV"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC reverse -1:Reversed,1:Normal
	{"MOT_SV1_DZ"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	200U	},//RC dead-zone
	//servo2
	{"MOT_SV2_MIN"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1100U	},//RC min PWM
	{"MOT_SV2_TRIM"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1500U	},//RC trim PWM
	{"MOT_SV2_MAX"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1900U	},//RC max PWM
	{"MOT_SV2_REV"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC reverse -1:Reversed,1:Normal
	{"MOT_SV2_DZ"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	200U	},//RC dead-zone
	//servo3
	{"MOT_SV3_MIN"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1100U	},//RC min PWM
	{"MOT_SV3_TRIM"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1500U	},//RC trim PWM
	{"MOT_SV3_MAX"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1900U	},//RC max PWM
	{"MOT_SV3_REV"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC reverse -1:Reversed,1:Normal
	{"MOT_SV3_DZ"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	200U	},//RC dead-zone
	//servo4
	{"MOT_SV4_MIN"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1100U	},//RC min PWM
	{"MOT_SV4_TRIM"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1500U	},//RC trim PWM
	{"MOT_SV4_MAX"		,	0U	,		E_MAV_PARAM_TYPE_UINT16		,	1900U	},//RC max PWM
	{"MOT_SV4_REV"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC reverse -1:Reversed,1:Normal
	{"MOT_SV4_DZ"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	200U	},//RC dead-zone
	//RSSI
	{"RSSI_TYPE"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RSSI Type  0:Disabled,1:AnalogPin,2:RCChannelPwmValue
	{"RSSI_ANA_PIN"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//Receiver RSSI analog sensing pin
	{"RSSI_PIN_LOW"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC max PWM
	{"RSSI_PIN_HIGH"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC reverse -1:Reversed,1:Normal
	{"RSSI_CHANNEL"		,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC dead-zone
	{"RSSI_CHAN_LOW"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC dead-zone
	{"RSSI_CHAN_HIGHT"	,	0U	,		E_MAV_PARAM_TYPE_UINT8		,	0U		},//RC dead-zone
	//PID
	//{"PID_ROLLKP"		,	0U	,		E_MAV_PARAM_TYPE_REAL32		,	0U		},//
		
		/*"PID_ROLLKP"
		"PID_ROLLKD"
		"PID_YAWKPAIL"
		"PID_YAWKDAIL"
		"PID_PITCHGAIN"
		"PID_PITCHKD"
		"PID_ROLLKPRUD"
		"PID_YAWKPRUD"
		"PID_YAWKDRUD"
		"PID_ROLLKPRUD"
		"PID_ROLLKDRUD"*/
};
#define NB_MAV_LINK_PARAMS sizeof(params)/sizeof(mavlinkParameter)


////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////



////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////////
//callback fire when data is received
static void SrvMAVLinkReceiveDataISR (Int8U data);

////////////////////////////////////////INLINE FUNCTIONS///////////////////////////////////////////
//compute CRC fct
static inline Int16U SrvMAVLinkComputeCRC( const Int8U* pBuffer, Int16U length, Int8U extraCRC );
static inline void SrvMAVLinkComputeDataCRC( Int16U* crc, const Int8U data);

////////////////////////////////////////PRIVATE FUNCTIONS//////////////////////////////////////////
static void SrvMAVLinkDispatcherIncommingMessages ( void ) ;
static void SrvMAVLinkSendMavlinkMessage( mavlinkMessage *mavlinkMsg, const MavLinkCommonMsgId *MsgId ) ;
static void SrvMAVLinkSendHeartBeat( void );
static void SrvMAVLinkSendParamValue( Int16U idParam );
static void SrvMAVLinkSendMissionCount( void );
static void SrvMAVLinkSendBatteryStatus( void );
static void SrvMAVLinkSentCommandLong( void );
static void SrvMAVLinkSendACK( Int16U command, mavlinkResultType result );

static void SrvMAVLinkReceiveRequestDataStream( void );
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//MAVLINK UART configuration
UartConfig confUartMavLink;

mavlinkMessage inMessage = { 0U };
mavlinkMessage outMessage = { 0U };
	
volatile Int8U mavLinkSeq = 0U;

Int8U bufferUartRx[ BUFFER_IN_OUT_MAX ] = {0U};
volatile Int8U incommingMessageSize = 0U;
volatile Int8U lastSeq = 0U;
volatile Boolean messageIncomming = FALSE;
Int16U idParam = 0U;


Int32U intervalMAVLink = 0UL;
Int32U lastreadMAVLink  = 0UL;
Int8U secondMAVLink  = 0U;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvMAVLinkInit ( void )
{
	Boolean o_success = FALSE;
	DrvUartInit( UART_0, UART_SPEED_115200 );
	
	for(Int16U loop = 0U ; loop < NB_MAV_LINK_PARAMS ; loop++)
	{
		params[ loop ].id = loop;
	}
	
	mavLinkSeq = 0U;
	messageIncomming = FALSE;
	idParam = 0xFFFFU;
	
	confUartMavLink.inBuffer.buffer = (Int8U*)&inMessage;
	confUartMavLink.inBuffer.maxSize = BUFFER_IN_OUT_MAX;
	confUartMavLink.inBuffer.index = 0U;
	confUartMavLink.inBuffer.nbData = 0U;
	
	confUartMavLink.outBuffer.buffer = (Int8U*)&bufferUartRx;
	confUartMavLink.outBuffer.maxSize = BUFFER_IN_OUT_MAX;
	confUartMavLink.outBuffer.index = 0U;
	confUartMavLink.outBuffer.nbData = 0U;
	
	confUartMavLink.expectedData.enable = FALSE;
	
	if( TRUE == DrvUartSetBufferConfiguration(UART_0,&confUartMavLink))
	{
		if( TRUE == DrvUartSetCallbackConfiguration(UART_0,SrvMAVLinkReceiveDataISR) )
		{
			o_success = TRUE;
		}
	}
	
   return o_success;
}

//Fonction de dispatching d'evenements
void SrvMAVLinkDispatcher ( Event_t iEvent )
{
	//Period
	intervalMAVLink += DrvTickGetTimeUs() - lastreadMAVLink;
	lastreadMAVLink = DrvTickGetTimeUs();
	
	//every 10 milliseconds call dispatcher
	if( lastreadMAVLink > 10000U )
	{
		intervalMAVLink = 0U;
		SrvMAVLinkDispatcherIncommingMessages();
		
		//get the second
		if(secondMAVLink < 100U)
		{
			secondMAVLink++;
		}
		else
		{
			secondMAVLink = 0;
			SrvMAVLinkSendHeartBeat();
		}
	}
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//Fonction de dispatching des messages entrant
static void SrvMAVLinkDispatcherIncommingMessages ( void )
{
	if( messageIncomming == TRUE )
	{
		if(inMessage.msg.msgId == MAVLINK_MSG_ID_HEARTBEAT)
		{
			if( CHECK_CRC( MAVLINK_MSG_ID_HEARTBEAT ) )
			{
				idParam = 0xFFFFU;
			}
		}
		else if(inMessage.msg.msgId == MAVLINK_MSG_ID_PARAM_REQUEST_LIST)
		{
			if( CHECK_CRC( MAVLINK_MSG_ID_PARAM_REQUEST_LIST ) )
			{
				idParam = 0;
			}
		}
		else if(inMessage.msg.msgId == MAVLINK_MSG_ID_PARAM_REQUEST_READ)
		{
			if( CHECK_CRC( MAVLINK_MSG_ID_PARAM_REQUEST_READ ) )
			{
				idParam = 0xFFFFU;
				SrvMAVLinkSendParamValue((Int16U)inMessage.msg.payload[ 0U ]);
			}
		}
		else if(inMessage.msg.msgId == MAVLINK_MSG_ID_MISSION_REQUEST_LIST)
		{
			if( CHECK_CRC( MAVLINK_MSG_ID_MISSION_REQUEST_LIST ) )
			{
				SrvMAVLinkSendMissionCount();
			}
		}
		else if(inMessage.msg.msgId == MAVLINK_MSG_ID_BATTERY_STATUS)
		{
			if( CHECK_CRC( MAVLINK_MSG_ID_BATTERY_STATUS ) )
			{
				SrvMAVLinkSendBatteryStatus();
			}
		}
		else if(inMessage.msg.msgId == MAVLINK_MSG_ID_REQUEST_DATA_STREAM )
		{
			if( CHECK_CRC( MAVLINK_MSG_ID_REQUEST_DATA_STREAM ) )
			{
				SrvMAVLinkReceiveRequestDataStream();
			}
		}
		else if(inMessage.msg.msgId == MAVLINK_MSG_ID_COMMAND_LONG )
		{
			if( CHECK_CRC( MAVLINK_MSG_ID_COMMAND_LONG ) )
			{
				SrvMAVLinkSentCommandLong();
			}
		}
		else
		{
			idParam = 0xFFFFU;
		}
		
		incommingMessageSize = 0U;
		inMessage.header = 0U;
		inMessage.msg.len = 0U;
		inMessage.msg.seq = 0U;
		inMessage.msg.sysId = 0U;
		inMessage.msg.compId = 0U;
		inMessage.msg.msgId = 0U;
		inMessage.cksA = 0U;
		inMessage.cksB = 0U;
		
		messageIncomming = FALSE;
	}
	else if(idParam != 0xFFFFU)
	{
		SrvMAVLinkSendParamValue(idParam);
		if(++idParam == NB_MAV_LINK_PARAMS)
		{
			idParam = 0xFFFFU;
		}
	}
}



static Int16U reqMessageRate = 0U;
static Int8U targetSystem = 0U;
static Int8U targetComponent = 0U;
static Int8U reqStreamId = 0U;
static Int8U startStop = 0U;

static void SrvMAVLinkReceiveRequestDataStream( void )
{
	//Int16U req_message_rate; ///< The requested interval between two messages of this type
	//Int8U target_system; ///< The target requested to send the message stream.
	//Int8U target_component; ///< The target requested to send the message stream.
	//Int8U req_stream_id; ///< The ID of the requested data stream
	//Int8U start_stop; ///< 1 to start sending, 0 to stop sending.
	
	reqMessageRate = *(Int16U *) &inMessage.msg.payload[ 0U ];
	targetSystem = inMessage.msg.payload[ 2U ];
	targetComponent = inMessage.msg.payload[ 3U ];
	reqStreamId = inMessage.msg.payload[ 4U ];
	startStop = inMessage.msg.payload[ 5U ];
}

static void SrvMAVLinkSentCommandLong( void )
{
	typedef struct 
	{
		float param1; ///< Parameter 1, as defined by MAV_CMD enum.
		float param2; ///< Parameter 2, as defined by MAV_CMD enum.
		float param3; ///< Parameter 3, as defined by MAV_CMD enum.
		float param4; ///< Parameter 4, as defined by MAV_CMD enum.
		float param5; ///< Parameter 5, as defined by MAV_CMD enum.
		float param6; ///< Parameter 6, as defined by MAV_CMD enum.
		float param7; ///< Parameter 7, as defined by MAV_CMD enum.
		Int16U command; ///< Command ID, as defined by MAV_CMD enum.
		Int8U target_system; ///< System which should execute the command
		Int8U target_component; ///< Component which should execute the command, 0 for all components
		Int8U confirmation; ///< 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
	} MavlinkCommandLong;
	
	MavlinkCommandLong cmd = *(MavlinkCommandLong*)&inMessage.msg.payload[0U];
	
	if(cmd.command == 400U)//	Arms / Disarms a component
	{
		if(cmd.param1 == 0U )//0 to disarm
		{
			SrvIhmPlatformDisarm();
			SrvMAVLinkSendACK(cmd.command,E_MAV_RESULT_ACCEPTED);
		}
		else if(cmd.param1 == 1U )//1 to arm
		{
			SrvIhmPlatformArm();
			SrvMAVLinkSendACK(cmd.command,E_MAV_RESULT_ACCEPTED);
		}
		else
		{
			SrvMAVLinkSendACK(cmd.command,E_MAV_RESULT_UNSUPPORTED);
		}
	}
}

static void SrvMAVLinkSendACK( Int16U command, mavlinkResultType result )
{
	//command			Int16U		Command ID, as defined by MAV_CMD enum.
	//result			Int8U		See MAV_RESULT enum
	*(Int16U*)&outMessage.msg.payload[0U] = command;
	outMessage.msg.payload[ 2U ] = result; // base_mode
	SrvMAVLinkSendMavlinkMessage(&outMessage, &commonMsgId[ 9U ]);
}

static void SrvMAVLinkSendScaledIMU( void )
{	
	//time_boot_ms	uint32_t	Timestamp (milliseconds since system boot)
    //xacc			int16_t		X acceleration (mg)
    //yacc			int16_t		Y acceleration (mg)
    //zacc			int16_t		Z acceleration (mg)
    //xgyro			int16_t		Angular speed around X axis (millirad /sec)
    //ygyro			int16_t		Angular speed around Y axis (millirad /sec)
    //zgyro			int16_t		Angular speed around Z axis (millirad /sec)
    //xmag			int16_t		X Magnetic field (milli tesla)
    //ymag			int16_t		Y Magnetic field (milli tesla)
    //zmag			int16_t		Z Magnetic field (milli tesla)
			   
	Int32U boot_time = DrvTickGetTimeUs();
	outMessage.msg.payload[ 0U ] = boot_time;
	outMessage.msg.payload[ 1U ] = (Int8U)(boot_time >> 8U);
	outMessage.msg.payload[ 2U ] = (Int8U)(boot_time >> 16U);
	outMessage.msg.payload[ 3U ] = (Int8U)(boot_time >> 24U);
	
	outMessage.msg.payload[ 4U ] = 0U;
	outMessage.msg.payload[ 5U ] = 0U;
	outMessage.msg.payload[ 6U ] = 0U;
	outMessage.msg.payload[ 7U ] = 0U;
	outMessage.msg.payload[ 8U ] = (Int8U)imu.sensors->bar->temperature; // temperature
	outMessage.msg.payload[ 9U ] = (Int8U)(imu.sensors->bar->temperature >> 8U); // temperature		
	for(Int8U loop = 0U ; loop < 20U ; loop += 2U)
	{
		outMessage.msg.payload[ 10U + loop ] = (Int8U)0xE4U; // voltages
		outMessage.msg.payload[ 11U + loop + 1U ] = (Int8U)(0x0CU >> 8U); // voltages
	}		
	outMessage.msg.payload[ 30U ] = (Int8U)0xDCU; // current_battery
	outMessage.msg.payload[ 31U ] = (Int8U)(0U >> 8U); // current_battery	
	outMessage.msg.payload[ 32U ] = (Int8U)0x00U; // id	
	outMessage.msg.payload[ 33U ] = (Int8U)0x01U; // battery_function	
	outMessage.msg.payload[ 34U ] = (Int8U)0x01U; // type	
	outMessage.msg.payload[ 35U ] = (Int8U)0x64U; // battery_remaining
	
	SrvMAVLinkSendMavlinkMessage(&outMessage, &commonMsgId[ 10U ]);
}

static void SrvMAVLinkSendBatteryStatus( void )
{	
	//id				uint8_t			Battery ID
	//battery_function	uint8_t			Function of the battery
	//type				uint8_t			Type (chemistry) of the battery
	//temperature		int16_t			Temperature of the battery in centi-degrees celsius. INT16_MAX for unknown temperature.
	//voltages			uint16_t[10]	Battery voltage of cells, in millivolts (1 = 1 millivolt). Cells above the valid cell count for this battery should have the UINT16_MAX value.
	//current_battery	int16_t			Battery current, in 10*milliamperes (1 = 10 milliampere), -1: autopilot does not measure the current
	//current_consumed	int32_t			Consumed charge, in milliampere hours (1 = 1 mAh), -1: autopilot does not provide mAh consumption estimate
	//energy_consumed	int32_t			Consumed energy, in 100*Joules (intergrated U*I*dt) (1 = 100 Joule), -1: autopilot does not provide energy consumption estimate
	//battery_remaining	int8_t			Remaining battery energy: (0%: 0, 100%: 100), -1: autopilot does not estimate the remaining battery
	outMessage.msg.payload[ 0U ] = (Int8U)1U; // current_consumed
	outMessage.msg.payload[ 1U ] = 0U;//(Int8U)(0U >> 8U); // current_consumed
	outMessage.msg.payload[ 2U ] = 0U;//(Int8U)(0U >> 16U); // current_consumed
	outMessage.msg.payload[ 3U ] = 0U;//(Int8U)(0U >> 24U); // current_consumed		
	outMessage.msg.payload[ 4U ] = 0U;//(Int8U)1U; // energy_consumed
	outMessage.msg.payload[ 5U ] = 0U;//(Int8U)(0U >> 8U); // energy_consumed
	outMessage.msg.payload[ 6U ] = 0U;//(Int8U)(0U >> 16U); // energy_consumed
	outMessage.msg.payload[ 7U ] = 0U;//(Int8U)(0U >> 24U); // energy_consumed	
	outMessage.msg.payload[ 8U ] = (Int8U)imu.sensors->bar->temperature; // temperature
	outMessage.msg.payload[ 9U ] = (Int8U)(imu.sensors->bar->temperature >> 8U); // temperature		
	for(Int8U loop = 0U ; loop < 20U ; loop += 2U)
	{
		outMessage.msg.payload[ 10U + loop ] = (Int8U)0xE4U; // voltages
		outMessage.msg.payload[ 11U + loop + 1U ] = (Int8U)(0x0CU >> 8U); // voltages
	}		
	outMessage.msg.payload[ 30U ] = (Int8U)0xDCU; // current_battery
	outMessage.msg.payload[ 31U ] = (Int8U)(0U >> 8U); // current_battery	
	outMessage.msg.payload[ 32U ] = (Int8U)0x00U; // id	
	outMessage.msg.payload[ 33U ] = (Int8U)0x01U; // battery_function	
	outMessage.msg.payload[ 34U ] = (Int8U)0x01U; // type	
	outMessage.msg.payload[ 35U ] = (Int8U)0x64U; // battery_remaining
	
	SrvMAVLinkSendMavlinkMessage(&outMessage, &commonMsgId[ 9U ]);
}

static void SrvMAVLinkSendMissionCount( void )
{
	//int16_t count; ///< Number of mission items in the sequence
	//uint8_t target_system; ///< System ID
	//uint8_t target_component; ///< Component ID	
	outMessage.msg.payload[ 0U ] = (Int8U)0U; // param_index
	outMessage.msg.payload[ 1U ] = (Int8U)(0U >> 8U); // param_index
	outMessage.msg.payload[ 2U ] = 200;//target_system
	outMessage.msg.payload[ 3U ] = 100;//target_component
	
	SrvMAVLinkSendMavlinkMessage(&outMessage, &commonMsgId[ 6U ]);
}

static void SrvMAVLinkSendParamValue( Int16U id )
{
	//param_id		char[16]	Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string
	//param_value	float		Onboard parameter value
	//param_type	uint8_t		Onboard parameter type: see the MAV_PARAM_TYPE enum for supported data types.
	//param_count	uint16_t	Total number of onboard parameters
	//param_index	uint16_t	Index of this onboard parameter	
	outMessage.msg.payload[ 3U ] = (Int8U)(params[id].defaultValue >> 24U); // param_type
	outMessage.msg.payload[ 2U ] = (Int8U)(params[id].defaultValue >> 16U); // param_type
	outMessage.msg.payload[ 1U ] = (Int8U)(params[id].defaultValue >> 8U); // param_type
	outMessage.msg.payload[ 0U ] = (Int8U)params[id].defaultValue; // param_type
	outMessage.msg.payload[ 4U ] = (Int8U)NB_MAV_LINK_PARAMS; // param_index
	outMessage.msg.payload[ 5U ] = (Int8U)(NB_MAV_LINK_PARAMS >> 8U); // param_count
	outMessage.msg.payload[ 6U ] = (Int8U)params[id].id; // param_index
	outMessage.msg.payload[ 7U ] = (Int8U)(params[id].id >> 8U); // param_index
	for(Int8U loop = 8U ; loop < 16U + 8U ; loop++)
	{
		outMessage.msg.payload[ loop ] = 0U;//param_id
	}
	memcpy(&outMessage.msg.payload[ 8U ], params[id].name, 16U); //param_id
	outMessage.msg.payload[ 24U ] = params[id].type; // param_type
	
	SrvMAVLinkSendMavlinkMessage(&outMessage, &commonMsgId[ 3U ]);
}

static void SrvMAVLinkSendHeartBeat( void )
{
	//type				Int8U		Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
	//autopilot			Int8U		Autopilot type / class. defined in MAV_AUTOPILOT ENUM
	//base_mode			Int8U		System mode bitfield, see MAV_MODE_FLAG ENUM in mavlink/include/mavlink_types.h
	//custom_mode		Int32U		A bitfield for use for autopilot-specific flags.
	//system_status		Int8U		System status flag, see MAV_STATE ENUM
	//mavlink_version	Int8U		MAVLink version, not writable by user, gets added by protocol because of magic data type: uint8_t_mavlink_version
	
	outMessage.msg.payload[ 0U ] = 2U; // type
	outMessage.msg.payload[ 1U ] = 0U; // autopilot
	outMessage.msg.payload[ 2U ] = 0U; // base_mode
	outMessage.msg.payload[ 3U ] = 0U; // custom_mode 0
	outMessage.msg.payload[ 4U ] = 0U; // custom_mode 1
	outMessage.msg.payload[ 5U ] = 0U; // custom_mode 2
	outMessage.msg.payload[ 6U ] = 0U; // custom_mode 3
	outMessage.msg.payload[ 7U ] = 3U; // system_status
	outMessage.msg.payload[ 8U ] = 3U; // mavlink_version
	 
	SrvMAVLinkSendMavlinkMessage(&outMessage, &commonMsgId[ 0U ]);
}




static void SrvMAVLinkSendMavlinkMessage( mavlinkMessage *mavlinkMsg,const MavLinkCommonMsgId *MsgId )
{
	//si il reste de la place
	if( confUartMavLink.outBuffer.index + MAVLINK_HEADER_BYTES_LEN + MsgId->length + MAVLINK_CHECKSUM_BYTES_LEN <= BUFFER_IN_OUT_MAX )
	{
		mavlinkMsg->header = MAVLINK_STX;
		mavlinkMsg->msg.len = MsgId->length;
		mavlinkMsg->msg.seq = mavLinkSeq;
		mavlinkMsg->msg.sysId = 100U;
		mavlinkMsg->msg.compId = 200U;
		mavlinkMsg->msg.msgId = MsgId->msgId;
		//fill the mavlink header
		memcpy( &bufferUartRx[ confUartMavLink.outBuffer.index ], (Int8U*)mavlinkMsg, MAVLINK_HEADER_BYTES_LEN);
		
		//fill the mavlink payload
		memcpy( &bufferUartRx[ confUartMavLink.outBuffer.index + MAVLINK_HEADER_BYTES_LEN ], (Int8U*)mavlinkMsg->msg.payload, MsgId->length);
		
		Int16U crc = SrvMAVLinkComputeCRC((Int8U *)&outMessage.msg,
		MAVLINK_CORE_HEADER_LEN + MsgId->length,
		MsgId->magic );
		
		outMessage.cksA = (Int8U)crc;
		outMessage.cksB = (Int8U)(crc >> 8U);
		
		//fill the mavlink header
		memcpy( &bufferUartRx[ confUartMavLink.outBuffer.index + MAVLINK_HEADER_BYTES_LEN + MsgId->length ], (Int8U*)&outMessage.cksA, MAVLINK_CHECKSUM_BYTES_LEN);
		
		confUartMavLink.outBuffer.nbData = MAVLINK_LEN_WO_PAYLOAD + MsgId->length ;
		DrvUartSendData(UART_0);
		mavLinkSeq++;
	}
	else
	{
		SrvIhmPlatformError();
	}
}


////////////////////////////////////////INLINE FUNCTIONS///////////////////////////////////////////
//compute CRC fct for buffer
static inline Int16U SrvMAVLinkComputeCRC( const Int8U* buffer, Int16U length, Int8U extraCRC )
{		
	Int16U crc = 0xFFFFU;
	while (length--) 
	{
		SrvMAVLinkComputeDataCRC(&crc, *buffer++);
	}
	SrvMAVLinkComputeDataCRC(&crc, extraCRC);
	return crc;
}
//compute CRC fct for 1 datum
static inline void SrvMAVLinkComputeDataCRC( Int16U* crc, const Int8U datum)
{
	Int8U tmp;
	tmp  = datum ^ (Int8U)(*crc &0xff);
	tmp ^= (tmp<<4);
	*crc = (*crc>>8) ^ (tmp<<8) ^ (tmp <<3) ^ (tmp>>4);
}

////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////////
static void SrvMAVLinkReceiveDataISR (Int8U data)
{
	if(messageIncomming == FALSE)
	{
		if((inMessage.header == 0 ) && (incommingMessageSize == 0U))
		{
			if( data == MAVLINK_STX )
			{
				inMessage.header = MAVLINK_STX;
			}
		}
		else if( ( inMessage.header == MAVLINK_STX ) &&
		( inMessage.msg.len == 0 ))
		{
			if( data < MAVLINK_MAX_PAYLOAD_LEN )
			{
				inMessage.msg.len = data;
			}
			else
			{
				inMessage.header = 0;
			}
		}
		else
		{
			((Int8U*)&inMessage.msg.seq)[ incommingMessageSize++ ] = data;
			
			lastSeq = inMessage.msg.seq;
			if( incommingMessageSize == inMessage.msg.len + MAVLINK_LEN_WO_PAYLOAD )
			{
				messageIncomming = TRUE;
			}
		}
	}
}



























/*
typedef struct tag_mavlink_parameter
{
	const char name[15];                // Name that will be displayed in the GCS
	uint8_t readonly;                   // Parameter is readonly (true) or Read / Write (false)
	uint8_t* pparam;                    // Reference to variable
	uint16_t param_size;                // parameter size in ram
} mavlinkParameter;

	
	
const mavlinkParameter mavlinkParametersList[] =
{
	{"PID_ROLLKP", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkp, sizeof(rollkp) },
	{"PID_ROLLKD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*)&rollkd, sizeof(rollkd) },
	{"PID_YAWKPAIL", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&yawkpail, sizeof(yawkpail) },
	{"PID_YAWKDAIL", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*)&yawkdail, sizeof(yawkdail) },
	{"PID_PITCHGAIN", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&pitchgain, sizeof(pitchgain) },
	{"PID_PITCHKD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&pitchkd, sizeof(pitchkd) },
	{"PID_ROLLKPRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkprud, sizeof(rollkprud) },
	{"PID_YAWKPRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&yawkprud, sizeof(yawkprud) },
	{"PID_YAWKDRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*)&yawkdrud, sizeof(yawkdrud) },
	{"PID_ROLLKPRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkprud, sizeof(rollkprud) },
	{"PID_ROLLKDRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkdrud, sizeof(rollkdrud) },

	{"MAG_CAL_RAW0", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rawMagCalib[0], sizeof(rawMagCalib[0]) },
	{"MAG_CAL_RAW1", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rawMagCalib[1], sizeof(rawMagCalib[1]) },
	{"MAG_CAL_RAW2", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rawMagCalib[2], sizeof(rawMagCalib[2]) },
	{"MAG_GAIN0", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&magGain[0], sizeof(magGain[0]) },
	{"MAG_GAIN1", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&magGain[1], sizeof(magGain[1]) },
	{"MAG_GAIN2", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&magGain[2], sizeof(magGain[2]) },
	{"MAG_OFFSET0", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_magOffset[0], sizeof(udb_magOffset[0]) },
	{"MAG_OFFSET1", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_magOffset[1], sizeof(udb_magOffset[1]) },
	{"MAG_OFFSET2", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_magOffset[2], sizeof(udb_magOffset[2]) },
	{"MAG_DECLINATION", {.param_int32=-180}, {.param_int32=180}, UDB_TYPE_INT_CIRCULAR, PARAMETER_READWRITE, (void*)&dcm_declination_angle.BB, sizeof(dcm_declination_angle.BB) },

	{"PWTRIM_AILERON", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[AILERON_INPUT_CHANNEL], sizeof(udb_pwTrim[AILERON_INPUT_CHANNEL]) },
	{"PWTRIM_ELEVATOR", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[ELEVATOR_INPUT_CHANNEL], sizeof(udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) },
	{"PWTRIM_RUDDER", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[RUDDER_INPUT_CHANNEL], sizeof(udb_pwTrim[RUDDER_INPUT_CHANNEL]) },
	{"PWTRIM_AILERON2", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[AILERON_SECONDARY_INPUT_CHANNEL], sizeof(udb_pwTrim[AILERON_SECONDARY_INPUT_CHANNEL]) },
	{"PWTRIM_ROLL", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[ROLL_INPUT_CHANNEL], sizeof(udb_pwTrim[ROLL_INPUT_CHANNEL]) },
	{"PWTRIM_PITCH", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[PITCH_INPUT_CHANNEL], sizeof(udb_pwTrim[PITCH_INPUT_CHANNEL]) },
	{"PWTRIM_THROTTLE", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[THROTTLE_INPUT_CHANNEL], sizeof(udb_pwTrim[THROTTLE_INPUT_CHANNEL]) },
	{"PWTRIM_YAW", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[YAW_INPUT_CHANNEL], sizeof(udb_pwTrim[YAW_INPUT_CHANNEL]) },
	{"PWTRIM_FLAP", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[FLAP_INPUT_CHANNEL], sizeof(udb_pwTrim[FLAP_INPUT_CHANNEL]) },
	{"PWTRIM_AIRBRAKE", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[BRAKE_INPUT_CHANNEL], sizeof(udb_pwTrim[BRAKE_INPUT_CHANNEL]) },
	{"PWTRIM_SPOILER", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[SPOILER_INPUT_CHANNEL], sizeof(udb_pwTrim[SPOILER_INPUT_CHANNEL]) },
	{"PWTRIM_CAMBER", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CAMBER_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMBER_INPUT_CHANNEL]) },
	{"PWTRIM_CROW", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CROW_INPUT_CHANNEL], sizeof(udb_pwTrim[CROW_INPUT_CHANNEL]) },
	{"PWTRIM_CAMPITCH", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL]) },
	{"PWTRIM_CAM_YAW", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL]) },

	{"IMU_XACCEL_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_xaccel.offset, sizeof(udb_xaccel.offset) },
	{"IMU_YACCEL_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_yaccel.offset, sizeof(udb_yaccel.offset) },
	{"IMU_ZACCEL_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_zaccel.offset, sizeof(udb_zaccel.offset) },
	{"IMU_XGYRO_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_xrate.offset, sizeof(udb_xrate.offset) },
	{"IMU_YGYRO_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_yrate.offset, sizeof(udb_yrate.offset) },
	{"IMU_ZGYRO_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_zrate.offset, sizeof(udb_zrate.offset) },
	{"IMU_VREF_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_vref.offset, sizeof(udb_vref.offset) },

	{"TH_H_TARGET_MIN", {.param_int32=0}, {.param_int32=5000}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&height_target_min, sizeof(height_target_min) },
	{"TH_H_TARGET_MAX", {.param_int32=0}, {.param_int32=5000}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&height_target_max, sizeof(height_target_max) },
	{"TH_H_MARGIN", {.param_int32=1}, {.param_int32=500}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&height_margin, sizeof(height_margin) },
	{"TH_T_HOLD_MIN", {.param_float=0}, {.param_float=1}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&alt_hold_throttle_min, sizeof(alt_hold_throttle_min) },
	{"TH_T_HOLD_MAX", {.param_float=0}, {.param_float=1}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&alt_hold_throttle_max, sizeof(alt_hold_throttle_max) },
	{"TH_P_HOLD_MIN", {.param_int32=-89}, {.param_int32=0}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&alt_hold_pitch_min, sizeof(alt_hold_pitch_min) },
	{"TH_P_HOLD_MAX", {.param_int32=0}, {.param_int32=89}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&alt_hold_pitch_max, sizeof(alt_hold_pitch_max) },
	{"TH_P_HIGH", {.param_int32=0}, {.param_int32=89}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&alt_hold_pitch_high, sizeof(alt_hold_pitch_high) },
	{"TH_P_RTL_DOWN", {.param_int32=0}, {.param_int32=89}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rtl_pitch_down, sizeof(rtl_pitch_down) },

	{"ASPD_DESIRED", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_DM, PARAMETER_READWRITE, (void*)&desiredSpeed, sizeof(desiredSpeed) },
	{"ASPD_MIN_GSPD", {.param_float=0}, {.param_float=20000}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&minimum_groundspeed, sizeof(minimum_groundspeed) },
	{"ASPD_MIN", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&minimum_airspeed, sizeof(minimum_airspeed) },
	{"ASPD_MAX", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&maximum_airspeed, sizeof(maximum_airspeed) },
	{"ASPD_CRUISE", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&cruise_airspeed, sizeof(cruise_airspeed) },
	{"ASPD_P_MIN_ASPD", {.param_int32=-90}, {.param_int32=90.0}, UDB_TYPE_DCM_ANGLE, PARAMETER_READWRITE, (void*)&airspeed_pitch_min_aspd, sizeof(airspeed_pitch_min_aspd) },
	{"ASPD_P_MAX_ASPD", {.param_int32=-90}, {.param_int32=90.0}, UDB_TYPE_DCM_ANGLE, PARAMETER_READWRITE, (void*)&airspeed_pitch_max_aspd, sizeof(airspeed_pitch_max_aspd) },
	{"ASPD_P_RATE_LIM", {.param_int32=1.0}, {.param_int32=720.0}, UDB_TYPE_FRAME_ANGLERATE, PARAMETER_READWRITE, (void*)&airspeed_pitch_adjust_rate, sizeof(airspeed_pitch_adjust_rate) },
	{"ASPD_P_KI", {.param_float=0.0}, {.param_float=1.0}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&airspeed_pitch_ki, sizeof(airspeed_pitch_ki) },
	{"ASPD_P_KI_LIMIT", {.param_int32=0.0}, {.param_int32=45.0}, UDB_TYPE_DCM_ANGLE, PARAMETER_READWRITE, (void*)&airspeed_pitch_ki_limit, sizeof(airspeed_pitch_ki_limit) },

	{"TURN_ELE_TR_NRM", {.param_float=-1.0}, {.param_float=1.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.ElevatorTrimNormal, sizeof(turns.ElevatorTrimNormal) },
	{"TURN_ELE_TR_INV", {.param_float=-1.0}, {.param_float=1.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.ElevatorTrimInverted, sizeof(turns.ElevatorTrimInverted) },
	{"TURN_CRUISE_SPD", {.param_float=0.0}, {.param_float=999.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.CruiseSpeed, sizeof(turns.CruiseSpeed) },
	{"TURN_AOA_NORMAL", {.param_float=-90.0}, {.param_float=90.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.AngleOfAttackNormal, sizeof(turns.AngleOfAttackNormal) },
	{"TURN_AOA_INV", {.param_float=-90.0}, {.param_float=90.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.AngleOfAttackInverted, sizeof(turns.AngleOfAttackInverted) },
	{"TURN_FEED_FWD", {.param_float=0.0}, {.param_float=100.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.FeedForward, sizeof(turns.FeedForward) },
	{"TURN_RATE_NAV", {.param_float=0.0}, {.param_float=100.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.TurnRateNav, sizeof(turns.TurnRateNav) },
	{"TURN_RATE_FBW", {.param_float=0.0}, {.param_float=100.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.TurnRateFBW, sizeof(turns.TurnRateFBW) },
};

const uint16_t count_of_parameters_list = sizeof(mavlink_parameters_list) / sizeof(mavlink_parameter);
*/



