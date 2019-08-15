/*
 * SrvComm.h
 *
 * Created: 31/05/2012 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVCOMM_H_
#define SRVCOMM_H_


#include "../Conf/ConfHard.h"
#include "../Tools/tools_typedefs.h"


////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////
#define COMM_START_MESSAGE      '<'   
#define COMM_STOP_MESSAGE       '>'   

#define COMM_HEADER_MESSAGE		'0'   //header
		
#define COMM_COMMAND_VERSION			'0'   //version
#define COMM_COMMAND_SERVO_READ			'1'   //servomotors read
#define COMM_COMMAND_SERVO_WRITE		'2'   //servomotors write
#define COMM_COMMAND_SERVO_MIN_READ		'3'   //servomotors read
#define COMM_COMMAND_SERVO_MIN_WRITE	'4'   //servomotors write
#define COMM_COMMAND_SERVO_MAX_READ		'5'   //servomotors read
#define COMM_COMMAND_SERVO_MAX_WRITE	'6'   //servomotors write

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	uint8_t header;
	uint8_t command;
	uint8_t size;
	uint8_t data[ 18U ];
}commMessage;

////////////////////////////////////////PUBLIC FONCTIONS/////////////////////////////////////////
//init of serial communication
Boolean SrvCommInit (void)  ;
//update serial communication
void SrvCommUpdate ( void ) ;

#endif /* SRVCOMM_H_ */