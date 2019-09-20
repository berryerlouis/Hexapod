/*
 * DrvTwi.h
 *
 * Created: 03/07/2012 10:23:09
 *  Author: berryer
 */

 #ifndef DRVTWI_H_
 #define DRVTWI_H_

 #include "../Tools/tools_typedefs.h"


////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

//on init le driver
Boolean DrvTwiInit( void );
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
//permits to get the number of errors count
Int16U DrvTwiGetErrorsCount( void );
#endif /* DRVTWI_H_ */