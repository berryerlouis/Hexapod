/*
 * tools_typedefs.h
 *
 * Created: 28/06/2011 15:55:05
 *  Author: berryer
 */ 


#ifndef TOOLS_TYPEDEFS_H_
#define TOOLS_TYPEDEFS_H_

#include <util/atomic.h>
#include <avr/interrupt.h> 
#include <avr/pgmspace.h>
#include <avr/wdt.h> 
#include <string.h>

// Unsigned 8 bits char
typedef char Char;

// Unsigned 8 bits integer type
typedef unsigned char Int8U;
// Unsigned 16 bits integer type
typedef unsigned int Int16U;
// Unsigned 32 bits integer type
typedef unsigned long Int32U;

// Signed 8 bits integer type
typedef signed char Int8S;
// Signed 16 bits integer type
typedef signed int Int16S;
// Signed 32 bits integer type
typedef signed long Int32S;

// Boolean type
typedef Int8U Boolean;
#define FALSE       0U
#define TRUE        1U
#define	ENABLE		TRUE
#define	DISABLE		FALSE
#define	ARMED		TRUE
#define	DISARMED	FALSE

#define NB_AXIS		3U
typedef struct
{
	Int16S x;
	Int16S y;
	Int16S z;
}SAxis;

typedef struct
{
	union
	{
		struct
		{
			Int32S x;
			Int32S y;
			Int32S z;
		};
		Int32S axes[NB_AXIS];
	};
}SAxis32;

typedef struct
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float axes[NB_AXIS];
	};
}AxisFloat;

//buffer def
typedef struct
{
	Int8U *bufferScreen;
	Int16U index;
	Int16U nbData;
	Int16U maxSize;
}Buffer;

static inline float EaseIn(float t)
{
	return t * t;
}

static inline float Flip(float x)
{
	return 1 - x;
}

static inline float EaseOut(float t)
{
	return Flip(sqrt(Flip(t)));
}

static inline float LerpSimple(float start_value, float end_value, float pct)
{
	return (start_value + (end_value - start_value) * pct);
}

static inline float EaseInOut(float t)
{
	return LerpSimple(EaseIn(t), EaseOut(t), t);
}
static inline AxisFloat LerpAxis(AxisFloat start_value, AxisFloat end_value, float t)
{
	AxisFloat position;
	position.x = LerpSimple(start_value.x,end_value.x,t);
	position.y = LerpSimple(start_value.y,end_value.y,t);
	position.z = LerpSimple(start_value.z,end_value.z,t);
	return position;
}

// Null pointer
#ifndef NULL
#define NULL     ((void *)0U)
#endif

#define UNUSED(x) (void)(x)
   
#ifndef ATOMIC
#define ATOMIC(code) \
						Int8U sreg = SREG;  \
						cli();\
						do{ code }while(0);\
						SREG = sreg;
#endif


static inline uint8_t asciiHexToInt(char ch)
{
	uint8_t num=0;
	if(ch>='0' && ch<='9')
	{
		num=ch-0x30;
	}
	else
	{
		switch(ch)
		{
			case 'A': case 'a': num=10; break;
			case 'B': case 'b': num=11; break;
			case 'C': case 'c': num=12; break;
			case 'D': case 'd': num=13; break;
			case 'E': case 'e': num=14; break;
			case 'F': case 'f': num=15; break;
			default: num=0;
		}
	}
	return num;
}



static inline float SetRange(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static inline float SetRangeInt16(Int16U x, Int16U in_min, Int16U in_max, Int16U out_min, Int16U out_max)
{
	return ((Int32U)(x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min;
}
	
static inline float SetLimits(float val, float min, float max)
{
	if(val > max)
	{
		val = max;
	}
	else if(val < min)
	{
		val = min;
	}
	return val;
}
static inline Int16S SetLimitsInt16S(Int16S val, Int16S min, Int16S max)
{
	if(val > max)
	{
		val = max;
	}
	else if(val < min)
	{
		val = min;
	}
	return val;
}

static inline Int16U SetLimitsInt16U(Int16U val, Int16U min, Int16U max)
{
	if(val > max)
	{
		val = max;
	}
	else if(val < min)
	{
		val = min;
	}
	return val;
}

static inline void swap_endianness(void *buf, Int8U size) 
{
  /* we swap in-place, so we only have to
  * place _one_ element on a temporary tray
  */
  Int8U tray;
  Int8U *from;
  Int8U *to;
  /* keep swapping until the pointers have assed each other */
  for (from = (Int8U*)buf, to = &from[size-1]; from < to; from++, to--) {
    tray = *from;
    *from = *to;
    *to = tray;
  }
}
/*
 * Basic Macros
 */
#define ToDeg(x) (x * 180.0F) / M_PI		//	180/M_PI  
#define ToRad(x) (x * M_PI)	/ 180.0F//	M_PI/180

#define BIT_READ( reg , bit )		(reg  & _BV(bit)) > 0U ? 1U : 0U
#define BIT_HIGH( reg , bit )		reg |=  _BV(bit)
#define BIT_LOW( reg , bit )		reg &= ~_BV(bit)
#define BIT_TOGGLE( reg , bit )		reg ^=  _BV(bit)
#define BIT_SET( reg , bit , val)							\
									if( val )				\
									{						\
										BIT_HIGH( reg , bit);\
									}						\
									else					\
									{						\
										BIT_LOW( reg , bit);	\
									};


						
#define MultiS16X16to32(longRes, intIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"muls %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mulsu %B2, %A1 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2) \
: \
"r26" \
)		
						

/*
 * Basic types for the microcontroler
 */
#define ABS(X)						((X) > 0 ? (X) : -(X))    
#define MID(X,limitdown,limitup)	((X > limitdown ) && (X < limitup ))
#define MIN(A,B)					(((A)<(B)) ? (A) : (B) )
#define MAX(A,B)					(((A)>(B)) ? (A) : (B) )

//definit un type de pointeur de fonction pour abstraire les interruptions micro
typedef void (*ptrfct_Isr_Callback)(void);
//defini un pointeur vers une fct null, reset du micro
#define RESET_SOFT() ptrfct_Isr_Callback ptrfct_null = NULL; ptrfct_null();

#define ________ 0U
#define _______X 1U
#define ______X_ 2U
#define ______XX 3U
#define _____X__ 4U
#define _____X_X 5U
#define _____XX_ 6U
#define _____XXX 7U
#define ____X___ 8U
#define ____X__X 9U
#define ____X_X_ 10U
#define ____X_XX 11U
#define ____XX__ 12U
#define ____XX_X 13U
#define ____XXX_ 14U
#define ____XXXX 15U
#define ___X____ 16U
#define ___X___X 17U
#define ___X__X_ 18U
#define ___X__XX 19U
#define ___X_X__ 20U
#define ___X_X_X 21U
#define ___X_XX_ 22U
#define ___X_XXX 23U
#define ___XX___ 24U
#define ___XX__X 25U
#define ___XX_X_ 26U
#define ___XX_XX 27U
#define ___XXX__ 28U
#define ___XXX_X 29U
#define ___XXXX_ 30U
#define ___XXXXX 31U
#define __X_____ 32U
#define __X____X 33U
#define __X___X_ 34U
#define __X___XX 35U
#define __X__X__ 36U
#define __X__X_X 37U
#define __X__XX_ 38U
#define __X__XXX 39U
#define __X_X___ 40U
#define __X_X__X 41U
#define __X_X_X_ 42U
#define __X_X_XX 43U
#define __X_XX__ 44U
#define __X_XX_X 45U
#define __X_XXX_ 46U
#define __X_XXXX 47U
#define __XX____ 48U
#define __XX___X 49U
#define __XX__X_ 50U
#define __XX__XX 51U
#define __XX_X__ 52U
#define __XX_X_X 53U
#define __XX_XX_ 54U
#define __XX_XXX 55U
#define __XXX___ 56U
#define __XXX__X 57U
#define __XXX_X_ 58U
#define __XXX_XX 59U
#define __XXXX__ 60U
#define __XXXX_X 61U
#define __XXXXX_ 62U
#define __XXXXXX 63U
#define _X______ 64U
#define _X_____X 65U
#define _X____X_ 66U
#define _X____XX 67U
#define _X___X__ 68U
#define _X___X_X 69U
#define _X___XX_ 70U
#define _X___XXX 71U
#define _X__X___ 72U
#define _X__X__X 73U
#define _X__X_X_ 74U
#define _X__X_XX 75U
#define _X__XX__ 76U
#define _X__XX_X 77U
#define _X__XXX_ 78U
#define _X__XXXX 79U
#define _X_X____ 80U
#define _X_X___X 81U
#define _X_X__X_ 82U
#define _X_X__XX 83U
#define _X_X_X__ 84U
#define _X_X_X_X 85U
#define _X_X_XX_ 86U
#define _X_X_XXX 87U
#define _X_XX___ 88U
#define _X_XX__X 89U
#define _X_XX_X_ 90U
#define _X_XX_XX 91U
#define _X_XXX__ 92U
#define _X_XXX_X 93U
#define _X_XXXX_ 94U
#define _X_XXXXX 95U
#define _XX_____ 96U
#define _XX____X 97U
#define _XX___X_ 98U
#define _XX___XX 99U
#define _XX__X__ 100U
#define _XX__X_X 101U
#define _XX__XX_ 102U
#define _XX__XXX 103U
#define _XX_X___ 104U
#define _XX_X__X 105U
#define _XX_X_X_ 106U
#define _XX_X_XX 107U
#define _XX_XX__ 108U
#define _XX_XX_X 109U
#define _XX_XXX_ 110U
#define _XX_XXXX 111U
#define _XXX____ 112U
#define _XXX___X 113U
#define _XXX__X_ 114U
#define _XXX__XX 115U
#define _XXX_X__ 116U
#define _XXX_X_X 117U
#define _XXX_XX_ 118U
#define _XXX_XXX 119U
#define _XXXX___ 120U
#define _XXXX__X 121U
#define _XXXX_X_ 122U
#define _XXXX_XX 123U
#define _XXXXX__ 124U
#define _XXXXX_X 125U
#define _XXXXXX_ 126U
#define _XXXXXXX 127U
#define X_______ 128U
#define X______X 129U
#define X_____X_ 130U
#define X_____XX 131U
#define X____X__ 132U
#define X____X_X 133U
#define X____XX_ 134U
#define X____XXX 135U
#define X___X___ 136U
#define X___X__X 137U
#define X___X_X_ 138U
#define X___X_XX 139U
#define X___XX__ 140U
#define X___XX_X 141U
#define X___XXX_ 142U
#define X___XXXX 143U
#define X__X____ 144U
#define X__X___X 145U
#define X__X__X_ 146U
#define X__X__XX 147U
#define X__X_X__ 148U
#define X__X_X_X 149U
#define X__X_XX_ 150U
#define X__X_XXX 151U
#define X__XX___ 152U
#define X__XX__X 153U
#define X__XX_X_ 154U
#define X__XX_XX 155U
#define X__XXX__ 156U
#define X__XXX_X 157U
#define X__XXXX_ 158U
#define X__XXXXX 159U
#define X_X_____ 160U
#define X_X____X 161U
#define X_X___X_ 162U
#define X_X___XX 163U
#define X_X__X__ 164U
#define X_X__X_X 165U
#define X_X__XX_ 166U
#define X_X__XXX 167U
#define X_X_X___ 168U
#define X_X_X__X 169U
#define X_X_X_X_ 170U
#define X_X_X_XX 171U
#define X_X_XX__ 172U
#define X_X_XX_X 173U
#define X_X_XXX_ 174U
#define X_X_XXXX 175U
#define X_XX____ 176U
#define X_XX___X 177U
#define X_XX__X_ 178U
#define X_XX__XX 179U
#define X_XX_X__ 180U
#define X_XX_X_X 181U
#define X_XX_XX_ 182U
#define X_XX_XXX 183U
#define X_XXX___ 184U
#define X_XXX__X 185U
#define X_XXX_X_ 186U
#define X_XXX_XX 187U
#define X_XXXX__ 188U
#define X_XXXX_X 189U
#define X_XXXXX_ 190U
#define X_XXXXXX 191U
#define XX______ 192U
#define XX_____X 193U
#define XX____X_ 194U
#define XX____XX 195U
#define XX___X__ 196U
#define XX___X_X 197U
#define XX___XX_ 198U
#define XX___XXX 199U
#define XX__X___ 200U
#define XX__X__X 201U
#define XX__X_X_ 202U
#define XX__X_XX 203U
#define XX__XX__ 204U
#define XX__XX_X 205U
#define XX__XXX_ 206U
#define XX__XXXX 207U
#define XX_X____ 208U
#define XX_X___X 209U
#define XX_X__X_ 210U
#define XX_X__XX 211U
#define XX_X_X__ 212U
#define XX_X_X_X 213U
#define XX_X_XX_ 214U
#define XX_X_XXX 215U
#define XX_XX___ 216U
#define XX_XX__X 217U
#define XX_XX_X_ 218U
#define XX_XX_XX 219U
#define XX_XXX__ 220U
#define XX_XXX_X 221U
#define XX_XXXX_ 222U
#define XX_XXXXX 223U
#define XXX_____ 224U
#define XXX____X 225U
#define XXX___X_ 226U
#define XXX___XX 227U
#define XXX__X__ 228U
#define XXX__X_X 229U
#define XXX__XX_ 230U
#define XXX__XXX 231U
#define XXX_X___ 232U
#define XXX_X__X 233U
#define XXX_X_X_ 234U
#define XXX_X_XX 235U
#define XXX_XX__ 236U
#define XXX_XX_X 237U
#define XXX_XXX_ 238U
#define XXX_XXXX 239U
#define XXXX____ 240U
#define XXXX___X 241U
#define XXXX__X_ 242U
#define XXXX__XX 243U
#define XXXX_X__ 244U
#define XXXX_X_X 245U
#define XXXX_XX_ 246U
#define XXXX_XXX 247U
#define XXXXX___ 248U
#define XXXXX__X 249U
#define XXXXX_X_ 250U
#define XXXXX_XX 251U
#define XXXXXX__ 252U
#define XXXXXX_X 253U
#define XXXXXXX_ 254U
#define XXXXXXXX 255U


#endif /* TOOLS_TYPEDEFS_H_ */