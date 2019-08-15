/*
 * DrvAdc.h
 *
 * Created: 14/11/2011 15:18:48
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#ifndef DRV_IO_H_
#define DRV_IO_H_

#include "../Tools/tools_typedefs.h"

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////

////////////////////////////////////////PUBLIC ENUMS//////////////////////////////////////////////
typedef enum
{
  EIO_PIN_A_0 = 0U,
  EIO_PIN_A_1 = 1U,
  EIO_PIN_A_2 = 2U,
  EIO_PIN_A_3 = 3U,
  EIO_PIN_A_4 = 4U,
  EIO_PIN_A_5 = 5U,
  EIO_PIN_A_6 = 6U,
  EIO_PIN_A_7 = 7U,
  EIO_PORT_A  = 8U,

  EIO_PIN_B_0 = EIO_PORT_A,
  EIO_PIN_B_1 = 9U,
  EIO_PIN_B_2 = 10U,
  EIO_PIN_B_3 = 11U,
  EIO_PIN_B_4 = 12U,
  EIO_PIN_B_5 = 13U,
  EIO_PIN_B_6 = 14U,
  EIO_PIN_B_7 = 15U,
  EIO_PORT_B  = 16U,

  EIO_PIN_C_0 = EIO_PORT_B,
  EIO_PIN_C_1 = 17U,
  EIO_PIN_C_2 = 18U,
  EIO_PIN_C_3 = 19U,
  EIO_PIN_C_4 = 20U,
  EIO_PIN_C_5 = 21U,
  EIO_PIN_C_6 = 22U,
  EIO_PIN_C_7 = 23U,
  EIO_PORT_C  = 24U,

  EIO_PIN_D_0 = EIO_PORT_C,
  EIO_PIN_D_1 = 25U,
  EIO_PIN_D_2 = 26U,
  EIO_PIN_D_3 = 27U,
  EIO_PIN_D_4 = 28U,
  EIO_PIN_D_5 = 29U,
  EIO_PIN_D_6 = 30U,
  EIO_PIN_D_7 = 31U,
  EIO_PORT_D  = 32U,

  //--------------- Always at end ----------------------------------------------
  EIO_NB_PIN = EIO_PORT_D
} EIoPin;

typedef enum
{
	IO_LEVEL_LOW,
	IO_LEVEL_HIGH
} EIOLevel;


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int8U volatile *port;
	Int8U volatile *ddr;
	Int8U volatile *pin;
	Int8U bit;
} SIoPin;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////
Boolean DrvIoSetPinOutput ( EIoPin pin ) ;
Boolean DrvIoSetPinInput ( EIoPin pin ) ;
EIOLevel DrvIoGetPinState ( EIoPin pin );
void DrvIoSetPinState ( EIoPin pin , EIOLevel level);
void DrvIoTogglePinState( EIoPin pin );
void DrvIoSetInterruptPin ( EIoPin pin );
void DrvIoResetInterruptPin ( EIoPin pin );
#endif /* DRV_IO_H_ */
