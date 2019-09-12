/*
 * DrvTwi.h
 *
 * Created: 03/07/2012 10:23:09
 *  Author: berryer
 */

 #ifndef DRVTWI_H_
 #define DRVTWI_H_

 #include "../Tools/tools_typedefs.h"


// TWI bit rate
#define TWI_SPEED_100K		100000UL
#define TWI_SPEED_400K		400000UL


typedef enum {
	Ready,
	Initializing,
	RepeatedStartSent,
	MasterTransmitter,
	MasterReceiver,
	SlaceTransmitter,
	SlaveReciever
} DrvTwiMode;

typedef struct DrvTwiInfoStruct{
	DrvTwiMode mode;
	uint8_t errorCode;
	uint8_t repStart;
	Boolean initialized;
}DrvTwiInfoStruct;


// Function declarations
Boolean DrvTwiInit(Int32U speed);

//read register
Boolean DrvTwiReadReg( Int8U slave_address , Int8U slave_register, Int8U *data ) ;
//write register
Boolean DrvTwiWriteReg( Int8U slave_address , Int8U slave_register, Int8U data ) ;
//read many register
Boolean DrvTwiReadRegBuf(Int8U slave_address, Int8U slave_register, Int8U *buffer, Int8U buffer_size) ;
//write many register
Boolean DrvTwiWriteRegBuf(Int8U slave_address, Int8U slave_register, Int8U *buffer, Int8U buffer_size) ;
//write many register with same data
Boolean DrvTwiFillRegBuf(Int8U slave_address, Int8U slave_register, Int8U data, Int8U buffer_size) ;

uint8_t DrvTwiTransmitData(void *const TXdata, uint8_t dataLen, uint8_t repStart);
uint8_t DrvTwiReadData(uint8_t TWIaddr, uint8_t bytesToRead, uint8_t repStart);
uint8_t DrvTwiReady(void);
DrvTwiInfoStruct *DrvTwiGetInfo( void );
#endif /* DRVTWI_H_ */