/*
 * ConfBoard.h
 *
 * Created: 20/12/2016 12:18:08
 *  Author: berryer
 */ 


#ifndef CONF_BOARD_H_
#define CONF_BOARD_H_


////////////////////////////////////////PUBLIC VERSION CODE/////////////////////////////////////////
//version 1.0
#define VERSION_SOFTWARE				0x01U 
#define VERSION_HARDWARE				0x01U

////////////////////////////////////////PUBLIC PROCESSOR BOARD////////////////////////////////////
//set processor 
#define ATMEGA_1284P
//#include "avr/iom1284p.h"
//#define ATMEGA_3284P

//Set oscillation frequency
#define	CONF_FOSC_MHZ					20UL
#define	CONF_FOSC_HZ					CONF_FOSC_MHZ * 1000000UL
#define	F_CPU							CONF_FOSC_HZ
//enum which permits to get the reset cause at startup
typedef enum
{
	RESET_CAUSE_POWER_ON	= 1U,
	RESET_CAUSE_EXTERNAL	= 2U,
	RESET_CAUSE_BROWN_OUT	= 4U,
	RESET_CAUSE_WATCHDOG	= 8U,
	RESET_CAUSE_JTAG		= 16U,
}E_ResetCause;


////////////////////////////////////////PUBLIC IHM BOARD///////////////////////////////////////////
//set LEDs used
typedef enum
{
	E_LED_0,
	E_LED_1,
	E_LED_2,
	E_LED_3,
	//Alaways at end
	E_NB_LEDS,
	E_LED_NONE
}ELed;

//set buttons used
typedef enum
{
	E_BUTTON_SETUP,
	//Alaways at end
	E_NB_BUTTONS,
	E_BUTTON_NONE
}EButton;

//nb timers used
typedef enum
{
	E_TIMER_BUTTON,
	//E_TIMER_LED,
	//----------------- Alaways at end -----------------------
	E_NB_TIMER,
	E_TIMER_NONE
}ETimer;

//set output IR used
typedef enum
{
	E_ADC_0,
	//Alaways at end
	E_NB_ADCS,
	E_ADC_NONE
}E_ADC;

//set output UltraSound used
typedef enum
{
	E_US_0,
	E_US_1,
	//Alaways at end
	E_NB_USS,
	E_US_NONE
}E_US;

//set output VLX used
typedef enum
{
	E_LAZER_0,
	//Alaways at end
	E_NB_LAZERS,
	E_LAZER_NONE
}E_LAZER;

//set output Uart used
typedef enum
{
	E_UART_0,
	E_UART_1,
	//Alaways at end
	E_NB_UARTS,
	E_UART_NONE
}EUART;

#if E_UART_0 == 0U
	#define E_UART_0_BAUD_RATE	115200UL
#endif
#if E_UART_1 == 0U
	#define E_UART_1_BAUD_RATE	115200UL
#endif
//set output I2c used
typedef enum
{
	E_I2C_0,
	//Alaways at end
	E_NB_I2CS,
	E_I2C_NONE
}EI2c;

//set output Spi used
typedef enum
{
	E_SPI_0,
	//Alaways at end
	E_NB_SPIS,
	E_SPI_NONE
}ESpi;
#if E_SPI_0 == 0U
	#define E_SPI_0_SPEED	20000000UL
#endif

////////////////////////////////////////PUBLIC SENSORS BOARD///////////////////////////////////////





#endif /* CONF_BOARD_H_ */