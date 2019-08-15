/*
 * SrvOpenPilot.h
 *
 * Created: 03/07/2012 13:49:01
 *  Author: berryer
 */ 

#include "Conf/conf_hard.h"
#include "Drv/DrvEvent.h"
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////



#define HEADER_LEN                           10U

#define RESPOND_OBJ_LEN                      HEADER_LEN
#define REQUEST_OBJ_LEN                      HEADER_LEN

#define UAVTALK_SYNC_VAL                     0x3CU

#define UAVTALK_TYPE_MASK                    0xF8U
#define UAVTALK_TYPE_VER                     0x20U

#define UAVTALK_TYPE_OBJ                     (UAVTALK_TYPE_VER | 0x00U)
#define UAVTALK_TYPE_OBJ_REQ                 (UAVTALK_TYPE_VER | 0x01U)
#define UAVTALK_TYPE_OBJ_ACK                 (UAVTALK_TYPE_VER | 0x02U)
#define UAVTALK_TYPE_ACK                     (UAVTALK_TYPE_VER | 0x03U)
#define UAVTALK_TYPE_NACK                    (UAVTALK_TYPE_VER | 0x04U)

typedef enum {
	UAVTALK_PARSE_STATE_WAIT_SYNC = 0U,
	UAVTALK_PARSE_STATE_GOT_SYNC,
	UAVTALK_PARSE_STATE_GOT_MSG_TYPE,
	UAVTALK_PARSE_STATE_GOT_LENGTH,
	UAVTALK_PARSE_STATE_GOT_OBJID,
	UAVTALK_PARSE_STATE_GOT_INSTID,
	UAVTALK_PARSE_STATE_GOT_DATA,
	UAVTALK_PARSE_STATE_GOT_CRC
} uavtalk_parse_state_t;

typedef enum {
	TELEMETRYSTATS_STATE_DISCONNECTED = 0,
	TELEMETRYSTATS_STATE_HANDSHAKEREQ,
	TELEMETRYSTATS_STATE_HANDSHAKEACK,
	TELEMETRYSTATS_STATE_CONNECTED
} telemetrystats_state_t;

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int8U sync;
	Int8U msgType;
	Int16U length;
	Int32U objID;
	Int16U instID;
	Int8U data[ 255U ];
	Int8U crc;
} uavTalkMessage;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
Boolean SrvOpenPilotInit ( void ) ;
//Fonction de dispatching d'evenements
void SrvOpenPilotDispatcher ( Event_t in_event ) ;
