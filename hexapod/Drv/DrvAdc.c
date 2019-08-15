////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "../Conf/ConfHard.h"

#include "DrvAdc.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define STEP	(float)(256.0/1023.0)
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
volatile Adc adc[E_NB_ADCS];
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean DrvAdcInit ( void )
{
	Boolean oSuccess = TRUE;
	//AVCC with external capacitor at AREF pin
	BIT_HIGH(ADMUX,REFS0);
	BIT_HIGH(ADMUX,REFS1);
	//single ended input
	BIT_LOW(ADMUX,MUX4);
	BIT_LOW(ADMUX,MUX3);
	
	//free running
	BIT_LOW(ADCSRB,ADTS0);
	BIT_LOW(ADCSRB,ADTS1);
	BIT_LOW(ADCSRB,ADTS2);
		
	//no start now
	BIT_LOW(ADCSRA,ADSC);
	//no auto trigger
	BIT_LOW(ADCSRA,ADATE);
	//flag
	BIT_LOW(ADCSRA,ADIF);
	//disable ADC interrupt
	BIT_LOW(ADCSRA,ADIE);
	//division factor set to 8
	BIT_HIGH(ADCSRA,ADPS0);
	BIT_HIGH(ADCSRA,ADPS1);
	//enable ADC
	BIT_HIGH(ADCSRA,ADEN);
	
	return oSuccess;
}


//Fonction de lecture ADC
Int16U DrvAdcRead ( EIoPin pin )
{
	//only on ADC pins
	if(pin < EIO_PORT_A)
	{		
		ADMUX = ((ADMUX & 0xE0U) | pin);
		BIT_HIGH(ADCSRA,ADSC);
		while(ADCSRA & (1<<ADSC));
	}
	
	//
	adc[E_ADC_0].value = ADC;
	return adc[E_ADC_0].value * STEP;		
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////