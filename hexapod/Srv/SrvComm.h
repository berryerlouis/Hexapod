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

//exemple of frame

//	<0 12 34 56 789 01 >
//	0  : header
//	12 : cluster
//	34 : command/attribut id
//	56 : size
//  789 : params
//	01 : crc



#define COMM_START_MESSAGE			'<'   
#define COMM_STOP_MESSAGE			'>'   

#define COMM_HEADER_MESSAGE			0x00U	//header

//error cluster and command
#define COMM_CLUSTER_UNDEFINED		0xFFU 
#define COMM_CLUSTER_COMMAND_UNDEFINED		0xFFU 

//id of clusters
typedef enum
{
	COMM_ERROR_NONE					,
	COMM_ERROR_SIZE_MESSAGE			,
	COMM_ERROR_UNKNOWN_CLUSTER		,
	COMM_ERROR_UNKNOWN_COMMAND		,
	COMM_ERROR_COMMAND_PARSE		,
}EErrorMessage;

//id of clusters
typedef enum
{
	COMM_CLUSTER_GENARAL	,
	COMM_CLUSTER_BEHAVIOR	,
	COMM_CLUSTER_IMU		,
	COMM_CLUSTER_DETECTION	,
	COMM_CLUSTER_SERVOS		,
	COMM_CLUSTER_LEG		,
	COMM_CLUSTER_DISPLAY	,
	COMM_CLUSTER_HEAD	
}ECluster;



//COMM_CLUSTER_GENARAL	
#define COMM_CLUSTER_GENARAL_COMMAND_VERSION		0x00U   //version
#define COMM_CLUSTER_GENARAL_COMMAND_START_PERIODIC	0x01U   //start sending periodic frame
#define COMM_CLUSTER_GENARAL_COMMAND_STOP_PERIODIC	0x02U   //stop sending periodic frame

//COMM_CLUSTER_BEHAVIOR
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_POSITION				0x00U   //get position
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_POSITION				0x01U   //set position
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_ROTATION_TRANSLATION	0x02U   //set roll pitch yaw on body andset translation x y z
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_WALK					0x03U   //set walk
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_WALK					0x04U   //get walk
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_GROUND_SIZE			0x05U   //set ground size
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_GROUND_SIZE			0x06U   //get ground size
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_ELEVATION				0x07U   //set elevation
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_ELEVATION				0x08U   //get elevation
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_FEELING				0x09U   //set feeling
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_FEELING				0x0AU   //get feeling
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_SPEED					0x0BU   //set speed
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_SPEED					0x0CU   //get speed
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_AMPLITUDE				0x0DU   //set amplitude
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_AMPLITUDE				0x0EU   //get amplitude
#define COMM_CLUSTER_BEHAVIOR_COMMAND_SET_DIRECTION				0x0FU   //set direction
#define COMM_CLUSTER_BEHAVIOR_COMMAND_GET_DIRECTION				0x10U   //get direction
#define COMM_CLUSTER_BEHAVIOR_COMMAND_START_SCAN				0x11U   //start scan


//COMM_CLUSTER_IMU
#define COMM_CLUSTER_IMU_COMMAND_GET_THRESHOLD		0x00U   //get Threshold
#define COMM_CLUSTER_IMU_COMMAND_SET_THRESHOLD		0x01U   //set Threshold
#define COMM_CLUSTER_IMU_COMMAND_GET_ROLL_PITCH_YAW	0x02U   //get roll pitch yaw
#define COMM_CLUSTER_IMU_COMMAND_GET_ROLL			0x03U   //get roll
#define COMM_CLUSTER_IMU_COMMAND_GET_PITCH			0x04U   //get pitch
#define COMM_CLUSTER_IMU_COMMAND_GET_YAW			0x05U   //get yaw

//COMM_CLUSTER_DETECTION
#define COMM_CLUSTER_DETECTION_ULTRASON_COMMAND_GET_THRESHOLD	0x00U   //get Threshold
#define COMM_CLUSTER_DETECTION_ULTRASON_COMMAND_SET_THRESHOLD	0x01U   //set Threshold
#define COMM_CLUSTER_DETECTION_ULTRASON_COMMAND_GET_DISTANCE	0x02U   //get distance
#define COMM_CLUSTER_DETECTION_LAZER_COMMAND_GET_THRESHOLD		0x03U   //get Threshold
#define COMM_CLUSTER_DETECTION_LAZER_COMMAND_SET_THRESHOLD		0x04U   //set Threshold
#define COMM_CLUSTER_DETECTION_LAZER_COMMAND_GET_DISTANCE		0x05U   //get distance

//COMM_CLUSTER_SERVOS
#define COMM_CLUSTER_SERVOS_COMMAND_STATUS_SERVO		0x00U  //servomotors status
#define COMM_CLUSTER_SERVOS_COMMAND_ENABLE_SERVO		0x01U  //servomotors enable
#define COMM_CLUSTER_SERVOS_COMMAND_SERVO_READ			0x02U  //servomotors read
#define COMM_CLUSTER_SERVOS_COMMAND_SERVO_WRITE			0x03U  //servomotors write
#define COMM_CLUSTER_SERVOS_COMMAND_SERVO_MIN_READ		0x04U  //servomotors read min
#define COMM_CLUSTER_SERVOS_COMMAND_SERVO_MIN_WRITE		0x05U  //servomotors write min
#define COMM_CLUSTER_SERVOS_COMMAND_SERVO_MAX_READ		0x06U  //servomotors read max
#define COMM_CLUSTER_SERVOS_COMMAND_SERVO_MAX_WRITE		0x07U  //servomotors write max
#define COMM_CLUSTER_SERVOS_COMMAND_SERVO_READ_ALL		0x08U  //servomotors read

//COMM_CLUSTER_LEG
#define COMM_CLUSTER_LEG_COMMAND_GET_LEG_XYZ			0x00U   //get x y z on leg
#define COMM_CLUSTER_LEG_COMMAND_SET_LEG_XYZ			0x01U   //set x y z on leg


//COMM_CLUSTER_DISPLAY

//COMM_CLUSTER_HEAD
#define COMM_CLUSTER_LEG_COMMAND_GET_LEG_XYZ			0x00U   //get x y z on leg
#define COMM_CLUSTER_LEG_COMMAND_SET_LEG_XYZ			0x01U   //set x y z on leg

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	uint8_t header;
	ECluster cluster;
	uint8_t command;
	uint8_t size;
	char data[ 50U ];
}commMessage;

////////////////////////////////////////PUBLIC FONCTIONS/////////////////////////////////////////
//init of serial communication
Boolean SrvCommInit (void)  ;
//update serial communication
void SrvCommUpdate ( void ) ;

#endif /* SRVCOMM_H_ */