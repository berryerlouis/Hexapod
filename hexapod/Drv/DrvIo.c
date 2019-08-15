////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"

#include "DrvIo.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
static const SIoPin ioTable[ EIO_NB_PIN ] =
{
	#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega1284__)
		{ &PORTA, &DDRA, &PINA, 0U},
		{ &PORTA, &DDRA, &PINA, 1U},
		{ &PORTA, &DDRA, &PINA, 2U},
		{ &PORTA, &DDRA, &PINA, 3U},
		{ &PORTA, &DDRA, &PINA, 4U},
		{ &PORTA, &DDRA, &PINA, 5U},
		{ &PORTA, &DDRA, &PINA, 6U},
		{ &PORTA, &DDRA, &PINA, 7U},
		{ &PORTB, &DDRB, &PINB, 0U},
		{ &PORTB, &DDRB, &PINB, 1U},
		{ &PORTB, &DDRB, &PINB, 2U},
		{ &PORTB, &DDRB, &PINB, 3U},
		{ &PORTB, &DDRB, &PINB, 4U},
		{ &PORTB, &DDRB, &PINB, 5U},
		{ &PORTB, &DDRB, &PINB, 6U},
		{ &PORTB, &DDRB, &PINB, 7U},
		{ &PORTC, &DDRC, &PINC, 0U},
		{ &PORTC, &DDRC, &PINC, 1U},
		{ &PORTC, &DDRC, &PINC, 2U},
		{ &PORTC, &DDRC, &PINC, 3U},
		{ &PORTC, &DDRC, &PINC, 4U},
		{ &PORTC, &DDRC, &PINC, 5U},
		{ &PORTC, &DDRC, &PINC, 6U},
		{ &PORTC, &DDRC, &PINC, 7U},
		{ &PORTD, &DDRD, &PIND, 0U},
		{ &PORTD, &DDRD, &PIND, 1U},
		{ &PORTD, &DDRD, &PIND, 2U},
		{ &PORTD, &DDRD, &PIND, 3U},
		{ &PORTD, &DDRD, &PIND, 4U},
		{ &PORTD, &DDRD, &PIND, 5U},
		{ &PORTD, &DDRD, &PIND, 6U},
		{ &PORTD, &DDRD, &PIND, 7U},
	#endif
};
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////


////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean DrvIoSetPinOutput ( EIoPin pin ) 
{
	Boolean o_success = FALSE;
	
	if( pin < EIO_NB_PIN )
	{
		*(ioTable[ pin ].ddr) |= (1 << ioTable[ pin ].bit);
		o_success = TRUE;
	}
	return o_success;
}

Boolean DrvIoSetPinInput ( EIoPin pin ) 
{
	Boolean o_success = FALSE;
	if( pin < EIO_NB_PIN )
	{
		*(ioTable[ pin ].ddr) &=~ (1 << ioTable[ pin ].bit);
		*(ioTable[ pin ].port) &=~ (1 << ioTable[ pin ].bit);
		o_success = TRUE;
	}
	return o_success;
}

EIOLevel DrvIoGetPinState ( EIoPin pin )
{
	EIOLevel pin_value = IO_LEVEL_LOW;

	pin_value = *(ioTable[pin].pin);
	pin_value = pin_value >> ioTable[pin].bit;
	pin_value &= 0x01U;

	return pin_value ;
}


void DrvIoSetPinState ( EIoPin pin , EIOLevel level)
{
	EIOLevel not_level = ((EIOLevel)(~level)) & 0x01U ;

	*(ioTable[pin].port) &= (Int8U)(~(Int8U)(not_level << (Int8U)ioTable[pin].bit));
	*(ioTable[pin].port) |= (Int8U)( level << (Int8U)ioTable[pin].bit );
}

void DrvIoTogglePinState( EIoPin pin )
{
	*(ioTable[pin].port) ^= (1U << ioTable[pin].bit);
}

void DrvIoSetInterruptPin ( EIoPin pin )
{
	if(pin < EIO_PORT_A)
	{
		BIT_HIGH(PCICR,PCIE0);
		BIT_HIGH(PCMSK0,ioTable[pin].bit);
	}
	else if(pin < EIO_PORT_B)
	{
		BIT_HIGH(PCICR,PCIE1);
		BIT_HIGH(PCMSK1,ioTable[pin].bit);	
	}
	else if(pin < EIO_PORT_C)
	{
		BIT_HIGH(PCICR,PCIE2);
		BIT_HIGH(PCMSK2,ioTable[pin].bit);	
	}
	else if(pin < EIO_PORT_D)
	{
		#ifdef PCIE3
		BIT_HIGH(PCICR,PCIE3);
		BIT_HIGH(PCMSK3,ioTable[pin].bit);	
		#endif
	}
}
void DrvIoResetInterruptPin ( EIoPin pin )
{	
	if(pin < EIO_PORT_A)
	{
		BIT_LOW(PCICR,PCIE0);
		BIT_LOW(PCMSK0,ioTable[pin].bit);
	}
	else if(pin < EIO_PORT_B)
	{
		BIT_LOW(PCICR,PCIE1);
		BIT_LOW(PCMSK1,ioTable[pin].bit);	
	}
	else if(pin < EIO_PORT_C)
	{
		BIT_LOW(PCICR,PCIE2);
		BIT_LOW(PCMSK2,ioTable[pin].bit);	
	}
	else if(pin < EIO_PORT_D)
	{
		#ifdef PCIE3
		BIT_HIGH(PCICR,PCIE3);
		BIT_LOW(PCMSK3,ioTable[pin].bit);
		#endif
	}
}