////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "SrvOpenPilot.h"
#include "SrvIhm.h"
#include "SrvImu.h"

#include "Drv/DrvUart.h"
#include "Drv/DrvTick.h"
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
// CRC lookup table
static const Int8U crcTable[ 256U ] = {
	0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
	0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
	0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
	0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
	0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
	0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
	0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
	0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
	0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
	0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
	0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
	0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
	0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
	0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
	0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
	0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////////
//callback fire when data is received
static void SrvOpenPilotReceiveDataISR (Int8U data);
static void SrvOpenPilotSendMsg(uavTalkMessage *msg);
static void SrvOpenPilotSendObject(uavTalkMessage *msg_to_respond, Int8U type);
static void SrvOpenPilotSendGCSTelemetryStats(void);
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//MAVLINK UART configuration
UartConfig confUartOpenPilot;
volatile Int8U bufferUartTx[ BUFFER_IN_OUT_MAX ] = {0U};	
volatile uavTalkMessage inMessage = { 0U };
volatile uavTalkMessage outMessage = { 0U };	
volatile Boolean incommingMessage = FALSE;

telemetrystats_state_t gcstelemetrystatus = TELEMETRYSTATS_STATE_DISCONNECTED;
static Int32U lastGCSTelemetryStatsSend = 0U;


float interval_1s = 0.0F;
Int32U lastread_1s = 0U;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvOpenPilotInit ( void )
{
	Boolean o_success = FALSE;
	DrvUartInit( UART_0, UART_SPEED_115200 );

	confUartOpenPilot.inBuffer.buffer = (Int8U*)&inMessage;
	confUartOpenPilot.inBuffer.maxSize = BUFFER_IN_OUT_MAX;
	confUartOpenPilot.inBuffer.index = 0U;
	confUartOpenPilot.inBuffer.nbData = 0U;
	
	confUartOpenPilot.outBuffer.buffer = (Int8U*)&bufferUartTx;
	confUartOpenPilot.outBuffer.maxSize = BUFFER_IN_OUT_MAX;
	confUartOpenPilot.outBuffer.index = 0U;
	confUartOpenPilot.outBuffer.nbData = 0U;

	confUartOpenPilot.expectedData.enable = FALSE;

	if( TRUE == DrvUartSetBufferConfiguration(UART_0, &confUartOpenPilot))
	{
		if( TRUE == DrvUartSetCallbackConfiguration(UART_0, SrvOpenPilotReceiveDataISR) )
		{
			o_success = TRUE;
		}
	}

	return o_success;
}

//Fonction de dispatching d'evenements
void SrvOpenPilotDispatcher ( Event_t in_event )
{
	interval_1s += DrvTickGetTimeUs() - lastread_1s;
	lastread_1s = DrvTickGetTimeUs();
	//every 100ms
	if( interval_1s > (1000000U - lastGCSTelemetryStatsSend) )
	{
		interval_1s = 0U;
		//envoie que si connecté
		if(gcstelemetrystatus == TELEMETRYSTATS_STATE_CONNECTED)
		{
			SrvOpenPilotSendGCSTelemetryStats();
		}
	}
	
	//nouveau message
	if(incommingMessage == TRUE)
	{
		//aquitement du nouveau message
		incommingMessage = FALSE;
		
		switch (inMessage.objID )
		{
			case 0xcad1dc0a :
				switch (inMessage.data[36U]) 
				{
					case TELEMETRYSTATS_STATE_DISCONNECTED:
						gcstelemetrystatus = TELEMETRYSTATS_STATE_HANDSHAKEREQ;
						SrvOpenPilotSendGCSTelemetryStats();
						break;
					case TELEMETRYSTATS_STATE_HANDSHAKEREQ:
						gcstelemetrystatus = TELEMETRYSTATS_STATE_HANDSHAKEACK;
						SrvOpenPilotSendGCSTelemetryStats();
						break;
					case TELEMETRYSTATS_STATE_HANDSHAKEACK:
						gcstelemetrystatus = TELEMETRYSTATS_STATE_CONNECTED;
						SrvOpenPilotSendGCSTelemetryStats();
						break;
					case TELEMETRYSTATS_STATE_CONNECTED:
						gcstelemetrystatus = TELEMETRYSTATS_STATE_CONNECTED;
						lastGCSTelemetryStatsSend = DrvTickGetTimeUs();
						break;
				}
				
			//gcstelemetrystatus = TELEMETRYSTATS_STATE_CONNECTED;
			//SrvOpenPilotSendGCSTelemetryStats();
			break;
		}
	}

}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


static void SrvOpenPilotSendGCSTelemetryStats(void)
{
    Int8U *d;
    Int8U i;
    uavTalkMessage msg;

    msg.sync    = UAVTALK_SYNC_VAL;
    msg.msgType = UAVTALK_TYPE_OBJ_ACK;
    msg.length  = 37 + HEADER_LEN;//gcstelemetrystats_obj_len + HEADER_LEN;
    msg.objID   = 0xcad1dc0a;

    d = msg.data;
    for (i = 0; i < 37U; i++) 
	{
        *d++ = 0;
    }

    msg.data[36] = gcstelemetrystatus;
    // remaining data unused and unset

    SrvOpenPilotSendMsg(&msg);
    lastGCSTelemetryStatsSend = DrvTickGetTimeUs()/1000U;
}

static void SrvOpenPilotSendObject(uavTalkMessage *msg_to_respond, Int8U type)
{
	uavTalkMessage msg;

	msg.sync    = UAVTALK_SYNC_VAL;
	msg.msgType = type;
	msg.length  = RESPOND_OBJ_LEN;
	msg.objID   = msg_to_respond->objID;

	SrvOpenPilotSendMsg(&msg);
}

static void SrvOpenPilotSendMsg(uavTalkMessage *msg)
{
	Int8U *d;
	Int8U i;
	Int8U c;

	c = (Int8U)(msg->sync);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[0 ^ c];
	c = (Int8U)(msg->msgType);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = (Int8U)(msg->length & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = (Int8U)((msg->length >> 8) & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = (Int8U)(msg->objID & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = (Int8U)((msg->objID >> 8) & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = (Int8U)((msg->objID >> 16) & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = (Int8U)((msg->objID >> 24) & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = 0; // (Int8U) (msg->InstID & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];
	c = 0; // (Int8U) ((msg->InstID >> 8) & 0xff);
	DrvUartSendDatum(UART_0,c);
	msg->crc = crcTable[msg->crc ^ c];

	if (msg->length > HEADER_LEN) {
		d = msg->data;
		for (i = 0; i < msg->length - HEADER_LEN; i++) {
			c = *d++;
			DrvUartSendDatum(UART_0,c);
			msg->crc = crcTable[msg->crc ^ c];
		}
	}
	DrvUartSendDatum(UART_0,msg->crc);
}




////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////////
//callback fire when data is received
static void SrvOpenPilotReceiveDataISR (Int8U data)
{
	static Int8U status = UAVTALK_PARSE_STATE_WAIT_SYNC;
	static Int8U crc    = 0;
	static Int8U cnt    = 0;

	switch (status) 
	{
		case UAVTALK_PARSE_STATE_WAIT_SYNC:
			if (data == UAVTALK_SYNC_VAL)
			{
				status    = UAVTALK_PARSE_STATE_GOT_SYNC;
				inMessage.sync = data;
				crc = crcTable[0 ^ data];
			}
			break;
		case UAVTALK_PARSE_STATE_GOT_SYNC:
			crc = crcTable[crc ^ data];
			if ((data & UAVTALK_TYPE_MASK) == UAVTALK_TYPE_VER)
			{
				status = UAVTALK_PARSE_STATE_GOT_MSG_TYPE;
				inMessage.msgType = data;
				cnt    = 0;
			}
			else
			{
				status = UAVTALK_PARSE_STATE_WAIT_SYNC;
			}
			break;
		case UAVTALK_PARSE_STATE_GOT_MSG_TYPE:
			crc = crcTable[crc ^ data];
			cnt++;
			if (cnt < 2) 
			{
				inMessage.length = ((Int16U)data);
			} 
			else
			{
				inMessage.length += ((Int16U)data) << 8;
				if ((inMessage.length < HEADER_LEN) || (inMessage.length > 255 + HEADER_LEN))
				{
					// Drop corrupted messages:
					// Minimal length is HEADER_LEN
					// Maximum is HEADER_LEN + 255 (Data) + 2 (Optional Instance Id)
					// As we are not parsing Instance Id, 255 is a hard maximum.
					status = UAVTALK_PARSE_STATE_WAIT_SYNC;
				} 
				else
				{
					status = UAVTALK_PARSE_STATE_GOT_LENGTH;
					cnt    = 0;
				}
			}
			break;
		case UAVTALK_PARSE_STATE_GOT_LENGTH:
			crc = crcTable[crc ^ data];
			cnt++;
			switch (cnt)
			{
				case 1:
					inMessage.objID  = ((Int32U)data);
					break;
				case 2:
					inMessage.objID += ((Int32U)data) << 8;
					break;
				case 3:
					inMessage.objID += ((Int32U)data) << 16;
					break;
				case 4:
					inMessage.objID += ((Int32U)data) << 24;
					status = UAVTALK_PARSE_STATE_GOT_OBJID;
				cnt    = 0;
				break;
			}
			break;
		case UAVTALK_PARSE_STATE_GOT_OBJID:
			crc = crcTable[crc ^ data];
			cnt++;
			switch (cnt) 
			{
				case 1:
					inMessage.instID  = ((Int32U)data);
					break;
				case 2:
					inMessage.instID += ((Int32U)data) << 8;
					if (inMessage.length == HEADER_LEN)
					{ // no data exists
						status = UAVTALK_PARSE_STATE_GOT_DATA;
					} 
					else
					{
						status = UAVTALK_PARSE_STATE_GOT_INSTID;
					}
					cnt = 0;
					break;
			}
		break;
		case UAVTALK_PARSE_STATE_GOT_INSTID:
			crc = crcTable[crc ^ data];
			cnt++;
			inMessage.data[cnt - 1] = data;
			if (cnt >= inMessage.length - HEADER_LEN) 
			{
				status = UAVTALK_PARSE_STATE_GOT_DATA;
				cnt    = 0;
			}
			break;
		case UAVTALK_PARSE_STATE_GOT_DATA:
			inMessage.crc = data;
			status   = UAVTALK_PARSE_STATE_GOT_CRC;
			break;
	}

	if (status == UAVTALK_PARSE_STATE_GOT_CRC)
	{
		status = UAVTALK_PARSE_STATE_WAIT_SYNC;
		if (crc == inMessage.crc)
		{
			incommingMessage = TRUE;
		}
	} 
}