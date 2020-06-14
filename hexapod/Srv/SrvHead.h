/*
 * SrvHead.h
 *
 * Created: 03/06/2020 14:32:39
 *  Author: louis
 */ 


#ifndef SrvHead_H_
#define SrvHead_H_

#include "Drv/DrvServo.h"

/////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////////
#define	NB_SERVOS_ON_HEAD		1U
#define	HEAD_HORI_SERVO_INDEX	0U

#define HEAD_HORI_AMPLITUDE			1800
#define HEAD_HORI_AMPLITUDE_DIV_2	HEAD_HORI_AMPLITUDE / 2

#define HEAD_HORI_OFT	-100
#define HEAD_HORI_MIN	HEAD_HORI_OFT - HEAD_HORI_AMPLITUDE_DIV_2
#define HEAD_HORI_MID	HEAD_HORI_OFT
#define HEAD_HORI_MAX	HEAD_HORI_OFT + HEAD_HORI_AMPLITUDE_DIV_2

typedef void (*SrvHeadNotification) ( void );
/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////////
typedef struct
{
	Boolean enable;
	Boolean initialized;
	Boolean scanning;
	SServo *servos[NB_SERVOS_ON_HEAD];
	SrvHeadNotification notification;
}SHead;
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean SrvHeadInit( void );
void SrvHeadUpdate( void );
Boolean SrvHeadScan( void );
SHead* SrvHeadGetStruct( void );
void SrvHeadSetPosition( Int16S angle, Int16U time );
void SrvHeadSetNotification (SrvHeadNotification cb) ;
#endif /* SrvHead_H_ */