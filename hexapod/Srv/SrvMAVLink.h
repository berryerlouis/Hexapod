/*
 * SrvMAVLink.h
 *
 * Created: 03/07/2012 13:49:01
 *  Author: berryer
 */ 

#include "Conf/conf_hard.h"
#include "Drv/DrvEvent.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////


////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
							//		0		1		2		3		4		5		6		7		8		9
//Int8U MAVLINK_MESSAGE_CRCS[] = {	50,		124,	137,	0,		237,	217,	104,	119,	0,		0, 
							//10 //	0,		89,		0,		0,		0,		0,		0,		0,		0,		0,
							//20 //	214,	159,	220,	168,	24,		23,		170,	144,	67,		115,
							//30 //	39,		246,	185,	104,	237,	244,	222,	212,	9,		254,
							//40 // 230,	28,		28,		132,	221,	232,	11,		153,	41,		39,
							//50 // 214,	223,	141,	33,		15,		3,		100,	24,		239,	238,
							//60 // 30,		240,	183,	130,	130,	118,	148,	21,		0,		243, 
							//70 //	124,	0,		0,		0,		20,		0,		152,	143,	0,		0, 
							//80 // 127,	106,	0,		0,		0,		0,		0,		0,		0,		231,
							//90 // 183,	63,		54,		0,		0,		0,		0,		0,		0,		0, 
							//100// 175,	102,	158,	208,	56,		93,		211,	108,	32,		185,
							//110// 235,	93,		124,	124,	119,	4,		76,		128,	56,		116,
							//120// 134,	237,	203,	250,	87,		203,	220,	0,		0,		0, 
							//120// 29,		223,	0,		0,		0,		0,		0,		0,		0,		0, 
							//130// 0,		0,		0,		0,		0,		0,		0,		177,	241,	15,
							//150// 134,	219,	208,	188,	84,		22,		19,		21,		134,	0, 
							//160// 78,		68,		189,	127,	111,	21,		21,		144,	1,		234,
							//170// 73,		181,	22,		0,		0,		0,		0,		0,		0,		0,
							//180// 0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
							//190// 0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
							//200// 0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
							//210// 0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
							//220// 0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
							//230// 0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
							//240// 0,		0,		0,		0,		0,		0,		0,		0,		0,		204,
							//250// 49,		170,	44,		83,		46,		0};



#define MAVLINK_STX						0xFEU ///< Packet start sign
#define MAVLINK_STX_LEN					1U ///< Length of start sign
#define MAVLINK_CORE_HEADER_LEN			5U ///< Length of core header (of the comm. layer): message length (1 byte) + message sequence (1 byte) + message system id (1 byte) + message component id (1 byte) + message type id (1 byte)
#define MAVLINK_HEADER_BYTES_LEN		(MAVLINK_CORE_HEADER_LEN + MAVLINK_STX_LEN) ///< Length of all header bytes, including core and checksum
#define MAVLINK_CHECKSUM_BYTES_LEN		2U
#define MAVLINK_LEN_WO_PAYLOAD			(MAVLINK_HEADER_BYTES_LEN + MAVLINK_CHECKSUM_BYTES_LEN)
#define MAVLINK_LEN_WO_STX_AND_PAYLOAD	(MAVLINK_LEN_WO_PAYLOAD - MAVLINK_STX_LEN)
#define MAVLINK_MAGIC_LEN				1U ///< Length of magic number
#define MAVLINK_MAX_PAYLOAD_LEN			55U



//definition des messages
#define MAVLINK_MSG_ID_HEARTBEAT					0U
#define MAVLINK_MSG_ID_HEARTBEAT_MAGIC				50U
#define MAVLINK_MSG_ID_HEARTBEAT_LEN				9U

#define MAVLINK_MSG_ID_PARAM_REQUEST_READ			20U
#define MAVLINK_MSG_ID_PARAM_REQUEST_READ_MAGIC		214U
#define MAVLINK_MSG_ID_PARAM_REQUEST_READ_LEN		9U

#define MAVLINK_MSG_ID_PARAM_REQUEST_LIST			21U
#define MAVLINK_MSG_ID_PARAM_REQUEST_LIST_MAGIC		159U
#define MAVLINK_MSG_ID_PARAM_REQUEST_LIST_LEN		2U

#define MAVLINK_MSG_ID_PARAM_VALUE					22U
#define MAVLINK_MSG_ID_PARAM_VALUE_MAGIC			220U
#define MAVLINK_MSG_ID_PARAM_VALUE_LEN				25U

#define MAVLINK_MSG_ID_PARAM_SET					23U
#define MAVLINK_MSG_ID_PARAM_SET_MAGIC				168U
#define MAVLINK_MSG_ID_PARAM_SET_LEN				23U

#define MAVLINK_MSG_ID_SCALED_IMU					26U
#define MAVLINK_MSG_ID_SCALED_IMU_MAGIC				170U
#define MAVLINK_MSG_ID_SCALED_IMU_LEN				22U

#define MAVLINK_MSG_ID_MISSION_REQUEST_LIST 		43U
#define MAVLINK_MSG_ID_MISSION_REQUEST_LIST_MAGIC	132U
#define MAVLINK_MSG_ID_MISSION_REQUEST_LIST_LEN		2U

#define MAVLINK_MSG_ID_MISSION_COUNT 				44U
#define MAVLINK_MSG_ID_MISSION_COUNT_MAGIC			221U
#define MAVLINK_MSG_ID_MISSION_COUNT_LEN			4U

#define MAVLINK_MSG_ID_REQUEST_DATA_STREAM 			66U
#define MAVLINK_MSG_ID_REQUEST_DATA_STREAM_MAGIC	148U
#define MAVLINK_MSG_ID_REQUEST_DATA_STREAM_LEN		6U

#define MAVLINK_MSG_ID_COMMAND_LONG  				76U
#define MAVLINK_MSG_ID_COMMAND_LONG_MAGIC			152U
#define MAVLINK_MSG_ID_COMMAND_LONG_LEN				33U

#define MAVLINK_MSG_ID_COMMAND_ACK	 				77U
#define MAVLINK_MSG_ID_COMMAND_ACK_MAGIC			143U
#define MAVLINK_MSG_ID_COMMAND_ACK_LEN				3U

#define MAVLINK_MSG_ID_BATTERY_STATUS 				147U
#define MAVLINK_MSG_ID_BATTERY_STATUS_MAGIC			154U
#define MAVLINK_MSG_ID_BATTERY_STATUS_LEN			36U

#define MAVLINK_MSG_ID_AUTOPILOT_VERSION 			148U
#define MAVLINK_MSG_ID_AUTOPILOT_VERSION_MAGIC		178U
#define MAVLINK_MSG_ID_AUTOPILOT_VERSION_LEN		60U

#define MAVLINK_MSG_ID_AUTOPILOT_VERSION_REQUEST 			183U
#define MAVLINK_MSG_ID_AUTOPILOT_VERSION_REQUEST_MAGIC		85U
#define MAVLINK_MSG_ID_AUTOPILOT_VERSION_REQUEST_LEN		2U

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef enum
{
	E_MAV_PARAM_TYPE_UINT8 = 1,
	E_MAV_PARAM_TYPE_INT8	,
	E_MAV_PARAM_TYPE_UINT16	,
	E_MAV_PARAM_TYPE_INT16	,
	E_MAV_PARAM_TYPE_UINT32	,
	E_MAV_PARAM_TYPE_INT32	,
	E_MAV_PARAM_TYPE_UINT64	,
	E_MAV_PARAM_TYPE_INT64	,
	E_MAV_PARAM_TYPE_REAL32	,
	E_MAV_PARAM_TYPE_REAL64 ,
}mavlinkParamType;

typedef enum
{
	E_MAV_RESULT_ACCEPTED	,
	E_MAV_RESULT_TEMPORARILY_REJECTED  ,
	E_MAV_RESULT_DENIED  	,
	E_MAV_RESULT_UNSUPPORTED,
	E_MAV_RESULT_FAILED  	,
}mavlinkResultType;

typedef struct 
{
	Int8U len;     ///< Length of payload
	Int8U seq;     ///< Sequence of packet
	Int8U sysId;   ///< ID of message sender system/aircraft
	Int8U compId;  ///< ID of the message sender component
	Int8U msgId;   ///< ID of message in payload
	Int8U payload[MAVLINK_MAX_PAYLOAD_LEN]; ///< Payload data, ALIGNMENT IMPORTANT ON MCU
} mavlinkCoreMessage;

typedef struct
{
	Int8U header;  ///< header of the frame
	mavlinkCoreMessage msg;
	Int8U cksA;    ///< Checksum high byte
	Int8U cksB;    ///< Checksum low byte
} mavlinkMessage;

typedef struct
{
	Char name[ 16U ];	//name of this param
	Int16U id;			// ID of this param
	mavlinkParamType type; //type of this param
	Int32U defaultValue;// valeur par defaut
} mavlinkParameter;


typedef struct
{
	Int8U msgId;
	Int8U length;
	Int8U magic;
}MavLinkCommonMsgId;




////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvMAVLinkInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvMAVLinkDispatcher ( Event_t in_event ) ;
