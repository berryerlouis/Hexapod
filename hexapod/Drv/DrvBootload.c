/*
* DrvBootloader.c
*
* Created: 06/07/2012 16:34:50
*  Author: berryer
*/
#include <avr/boot.h>

#include "Drv/DrvLed.h"
#include "Drv/DrvButton.h"
#include "Drv/DrvUart.h"
#include "Drv/DrvEeprom.h"
#include "Drv/DrvTimer.h"
#include "Drv/DrvInterrupt.h"

#include "DrvBootload.h"


////////////////////////////////////////PRIVATE DEFINES/////////////////////////////////////////


////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////
//set the address of each flash part
void (*startBootloader)(void)	=	(void (*)(void))0xF000U;
void (*startApp) (void)			=	(void (*)(void))0x0000U;

////////////////////////////////////////PRIVATE VARIABLES///////////////////////////////////////
typedef enum
{
	BOOT_FRAME_START,
	BOOT_FRAME_NB_DATA,
	BOOT_FRAME_ADDRESS,
	BOOT_FRAME_TYPE,
	BOOT_FRAME_DATA,
	BOOT_FRAME_CS,
	BOOT_FRAME_END,
}BootFrameIndex;

typedef struct
{
	Int8U nbData;
	Int16U address;
	Int8U type;
	Int8U data[ 16U ];
	Int8U checksum;
	Int16U oldAddress;
}BootFrame;


static BootFrame bootFrame;
static Int16U nbDataAvailable = 0U;
static BootFrameIndex bootFrameIndex = BOOT_FRAME_START;
static Int8U bufferPage[ SPM_PAGESIZE + 16U ];
static Int8U asciiHexaIndex = 0U;
static Int8U indexData = 0U;
static volatile Char datum;
static Int16U writtenBytes = 0U;



//Init du bootload in app flash area
void DrvBootloadAppInit( void )
{
	//test the button status in order to know if user want to be in boot mode
	if(DrvButtonPushed(E_BUTTON_SETUP) == TRUE)
	{
		DrvEepromSetRunMode( VAL_EEPROM_CHECK_RUN_BOOT );
		
		//clear interrupt
		//DrvInterruptClearAllInterrupts();
		//reset wdg
		wdt_enable(WDTO_15MS);
		//go to App area
		while(1);
	}
}

//Init du bootload in boot flash area
void DrvBootloadBootInit( void )
{
	//check eeprom status
	if(DrvEepromGetRunMode() == VAL_EEPROM_CHECK_RUN_APP) 
	{		
		/* Enable change of interrupt vectors */
		MCUCR = (1<<IVCE);
		/* Move interrupts to app flash section */
		MCUCR &= ~(1<<IVSEL);
		//go to App area
		startApp();
	}
	
	/* Enable change of interrupt vectors */
	MCUCR = (1<<IVCE);
	/* Move interrupts to boot flash section */
	MCUCR = (1<<IVSEL);
	
	//reset wdg
	wdt_enable(WDTO_1S);
		
	DrvInterruptSetAllInterrupts();
	
	DrvUartInit( E_UART_1, UART_SPEED_115200 );
	DrvTimerInit();
	DrvLedInit();
	
	//IHM init
	DrvLedSetPinLed(E_LED_OK);
	DrvLedSetPinLed(E_LED_WARNING);
	DrvLedSetPinLed(E_LED_ERROR);


	//set status of leds
	DrvLedSetBlinkMode(E_LED_OK, 500U, 500U);
	DrvLedSetState(E_LED_WARNING, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_ERROR, E_LED_STATE_OFF);
}

//Bootloader disptacher
void DrvBootloadDispatcher( void )
{
	//get available data on uart buffer
	nbDataAvailable = DrvUartDataAvailable(E_UART_1);

	//parse hex line
	for( Int16U loopDatum = 0U; loopDatum < nbDataAvailable ; loopDatum++)
	{
		//get datum
		datum = DrvUartReadData( E_UART_1 );
	
		//dependent of the sequence in line
		switch(bootFrameIndex)
		{
			case BOOT_FRAME_START :
			{
				//wait for start frame
				if(datum == ':')
				{
					//init new frame
					asciiHexaIndex = 0U;
					indexData = 0U;
					//save last address
					bootFrame.oldAddress = bootFrame.address;
					bootFrameIndex = BOOT_FRAME_NB_DATA;
				}
			}
			break;
			case BOOT_FRAME_NB_DATA :
			{
				//get number of data
				datum = strtol((const char *)&datum, NULL, 16);
				if(asciiHexaIndex == 0U)
				{
					bootFrame.nbData = datum << 4U;
					asciiHexaIndex = 1U;
				}
				else
				{
					bootFrame.nbData += datum;
					asciiHexaIndex = 0U;
					bootFrameIndex = BOOT_FRAME_ADDRESS;
				}
			}
			break;
			case BOOT_FRAME_ADDRESS :
			{
				//get address
				datum = strtol((const char *)&datum, NULL, 16);
				if(asciiHexaIndex == 0U)
				{
					bootFrame.address = datum << 12U;
					asciiHexaIndex = 1U;
				}
				else if(asciiHexaIndex == 1U)
				{
					bootFrame.address += datum << 8U;
					asciiHexaIndex = 2U;
				}
				else if(asciiHexaIndex == 2U)
				{
					bootFrame.address += datum << 4U;
					asciiHexaIndex = 3U;
				}
				else
				{
					bootFrame.address += datum;
					asciiHexaIndex = 0U;
					bootFrameIndex = BOOT_FRAME_TYPE;
				}
			}
			break;
			case BOOT_FRAME_TYPE :
			{
				//get type of frame
				datum = strtol((const char *)&datum, NULL, 16);
				if(asciiHexaIndex == 0U)
				{
					bootFrame.type = datum << 4U;
					asciiHexaIndex = 1U;
				}
				else
				{
					bootFrame.type += datum;
					asciiHexaIndex = 0U;
					bootFrameIndex = BOOT_FRAME_DATA;
				
					//end of file, no data
					if(bootFrame.type == 1)
					{
						bootFrameIndex = BOOT_FRAME_CS;
					}
					indexData = 0U;
				}
			}
			break;
			case BOOT_FRAME_DATA :
			{
				//get data
				datum = strtol((const char *)&datum, NULL, 16);
				if(asciiHexaIndex == 0U)
				{
					bootFrame.data[indexData] = datum << 4U;
					asciiHexaIndex = 1U;
				}
				else
				{
					bootFrame.data[indexData++] += datum;
					asciiHexaIndex = 0U;
				
					//we have taken all data in this line
					if( indexData == bootFrame.nbData)
					{
						bootFrameIndex = BOOT_FRAME_CS;
					}
				}
			}
			break;
			case BOOT_FRAME_CS :
			{
				//get checksum
				datum = strtol((const char *)&datum, NULL, 16);
				if(asciiHexaIndex == 0U)
				{
					bootFrame.checksum = datum << 4U;
					asciiHexaIndex = 1U;
				}
				else
				{
					bootFrame.checksum += datum;
					asciiHexaIndex = 0U;
					bootFrameIndex = BOOT_FRAME_END;
				}
			}
			break;
			case BOOT_FRAME_END :
			{
				//get the end of frame "\r\n"
				if(asciiHexaIndex == 0U)
				{
					if(datum == '\r')
					{
						asciiHexaIndex = 1U;
					}
				}
				else
				{
					if(datum == '\n')
					{
						//fill data in temp buffer
						for(Int8U loop = 0U; loop < bootFrame.nbData; loop++ )
						{
							bufferPage[ writtenBytes + loop ] = bootFrame.data[loop];
						}
						//get how many bytes are ready to write on flash
						writtenBytes += bootFrame.nbData;
					
						//wait for fill fully the buffer
						if((writtenBytes >= SPM_PAGESIZE) || (bootFrame.type == 1))
						{
							//recover address
							if(bootFrame.type == 1)
							{
								if(writtenBytes > SPM_PAGESIZE)
								{
									bootFrame.address = bootFrame.oldAddress + (writtenBytes - 256);
								}
								else 
								{
									bootFrame.address = bootFrame.oldAddress + writtenBytes;
								}
							}
						
							//turn on LED indicates we are writing on flash
							DrvLedSetState(E_LED_ERROR, E_LED_STATE_ON);
						
							//write page on flash
							DrvBootloadBootProgramPage(((bootFrame.address/SPM_PAGESIZE)%SPM_PAGESIZE)*SPM_PAGESIZE ,bufferPage);
						
							//erase the buffer except last line if partially written
							asciiHexaIndex = 0U;
							if(writtenBytes > SPM_PAGESIZE)
							{
								writtenBytes = writtenBytes - SPM_PAGESIZE;
								//move last line to first
								for( Int16U loop = 256U; loop < SPM_PAGESIZE + writtenBytes ; loop++)
								{
									bufferPage[ loop - SPM_PAGESIZE ] = bufferPage[ loop ];
								}
							}
							else
							{
								writtenBytes = 0U;
							}
							//erase
							for( Int16U loop = writtenBytes; loop < sizeof(bufferPage) ; loop++)
							{
								bufferPage[loop] = 0xFFU;
							}
						
							//turn off LED indicates we are finish to write on flash
							DrvLedSetState(E_LED_ERROR, E_LED_STATE_OFF);
						
							//go to app
							if(bootFrame.type == 1)
							{
								//send CS to be sure frame is ok
								DrvUartFillTxBuffer(E_UART_1, bootFrame.checksum);
								DrvUartSendData(E_UART_1);
							
								//go to app
								DrvEepromSetRunMode(VAL_EEPROM_CHECK_RUN_APP);
															
								//clear interrupt
								DrvInterruptClearAllInterrupts();
								//reset wdg
								wdt_enable(WDTO_15MS);
								//go to App area
								while(1);
							}
						}
					
						//send CS to be sure frame is ok
						DrvUartFillTxBuffer(E_UART_1, bootFrame.checksum);
						DrvUartSendData(E_UART_1);
						bootFrameIndex = BOOT_FRAME_START;
					}
				}
			}
			break;
		}
	}
}

void DrvBootloadBootProgramPage (Int32U page, Int8U *buf)
{
	Int16U i;
	Int8U sreg;
	// Disable interrupts.
	sreg = SREG;
	cli();
	eeprom_busy_wait ();
	boot_page_erase (page);
	boot_spm_busy_wait ();      // Wait until the memory is erased.
	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		Int16U w = *buf++;
		w += (*buf++) << 8;
		
		boot_page_fill (page + i, w);
	}
	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.
	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable ();
	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;
}

char DrvBootloadFlashReadByte (Int32U addr)
{
	return pgm_read_byte(addr);
}

void DrvBootloadFlashReadPage (Int8U page, Int8U *buf)
{
	Int32U addr = (page*SPM_PAGESIZE);
	
	for (int i = 0; i < 128; i++)
	{
		*buf = DrvBootloadFlashReadByte(addr);
		boot_spm_busy_wait();
		buf++;
		addr++;
	}
}



