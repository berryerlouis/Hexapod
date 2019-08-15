////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "SrvMultiWii.h"
#include "SrvIhm.h"
#include "SrvImu.h"
#include "SrvPID.h"
#include "SrvMotors.h"

#include "Drv/DrvUart.h"
#include "Drv/DrvTwi.h"
#include "Drv/DrvServo.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
//enum used for parsing the incomming frame
typedef enum {
	MULTIWII_STATE_WAIT_PREAMBLE_0 = 0U,
	MULTIWII_STATE_WAIT_PREAMBLE_1,
	MULTIWII_STATE_GOT_PREAMBLE,
	MULTIWII_STATE_GOT_DIRECTION,
	MULTIWII_STATE_GOT_SIZE,
	MULTIWII_STATE_GOT_COMMAND,
	MULTIWII_STATE_GOT_DATA,
	MULTIWII_STATE_GOT_CRC,
} multiwii_state_t;

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//sent data to GS
static void SrvMultiWiiSendMessage ( Int8U command, Int8U size, Int8U *data );

////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////////
//permits to decode the icomming data
static Boolean SrvMultiWiiDecodeMessage( void );
static void SrvMultiWiiDoMessage( void );

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
multiWiiMessage inMessage = { .command =  0U, .crc = 0U, .direction = 0U, .size = 0U };
multiWiiMessage outMessage = { .command =  0U, .crc = 0U, .direction = 0U, .size = 0U };
Int8U cptDataMessage = 0U;
multiwii_state_t status = MULTIWII_STATE_WAIT_PREAMBLE_0;
Int16U nbDataAvailable = 0U;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvMultiWiiInit ( void )
{
	return DrvUartInit( E_UART_1, UART_SPEED_115200 );
}

//Fonction de dispatching d'evenements
void SrvMultiWiiDispatcher ( void )
{	
	SrvMultiWiiDecodeMessage();
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//sent data to GS
static void SrvMultiWiiSendMessage ( Int8U command, Int8U size, Int8U *data )
{
	outMessage.preamble[ 0U ] = MSP_PREAMBLE[ 0U ];
	outMessage.preamble[ 1U ] = MSP_PREAMBLE[ 1U ];
	outMessage.direction = MSP_DIRECTION_OUT;
	outMessage.crc = 0U;
	outMessage.size = size;
	outMessage.crc ^= size;
	outMessage.command = command;
	outMessage.crc ^= command;
	for(Int8U i = 0U ; i < outMessage.size ; i++ )
	{
		outMessage.data[ i ] = data[ i ];
		outMessage.crc ^= data[ i ];
	}
	//move crc just after data
	outMessage.data[ outMessage.size ] = outMessage.crc;
	
	for( Int8U loop = 0U; loop < 6U + outMessage.size ; loop++)
	{
		DrvUartFillTxBuffer(E_UART_1, (Int8U)((Int8U*)(&outMessage))[loop]);
	}
	DrvUartSendData(E_UART_1);
}

static void SrvMultiWiiDoMessage( void )
{
	switch( inMessage.command )
	{
		default:
		{
			SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
		}
		break;
		case MSP_IDENT:
		{
			struct
			{
				Int8U version;
				Int8U multitype;
				Int8U mspVersion;
				Int32U capability;
			} cmdIdentification;
			
			cmdIdentification.version = MULTIWII_VERSION; 
			cmdIdentification.multitype = 3U; //QuadX
			cmdIdentification.mspVersion = MSP_VERSION;
			cmdIdentification.capability = 0U;//(0U+BIND_CAPABLE)|DYNBAL<<2U|FLAP<<3U|NAVCAP<<4U|EXTAUX<<5U|((Int32U)NAVI_VERSION<<28);
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdIdentification),(Int8U*)&cmdIdentification);
		}
		break;
		case MSP_STATUS:
		{
			struct {
				Int16U cycleTime;
				Int16U i2cErrorsCount;
				Int16U sensors;
				Int32U flag;
				Int8U set;
			} cmdStatus;
			
			cmdStatus.cycleTime = (Int16U)imu.status.loopTime;
			cmdStatus.i2cErrorsCount = DrvTwiGetErrorsCount();
			cmdStatus.sensors = ACC<<0U|BAR<<1U|MAG<<2U|GPS<<3U|US<<4U;
			cmdStatus.flag = imu.status.armed;
			cmdStatus.set = 0U;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdStatus),(Int8U*)&cmdStatus);
		}
		break;
		case MSP_RAW_IMU:
		{
			struct {
				Int16S acc[ 3U ];
				Int16S gyr[ 3U ];
				Int16S mag[ 3U ];
			} cmdRawImu;

			cmdRawImu.acc[ 0U ] = (Int16S)imu.sensors->acc->roulis;
			cmdRawImu.acc[ 1U ] = (Int16S)imu.sensors->acc->tangage;
			cmdRawImu.acc[ 2U ] = (Int16S)(imu.sensors->acc->rawData.z >> 6U);
			cmdRawImu.gyr[ 0U ] = (Int16S)imu.sensors->gyr->roulis;
			cmdRawImu.gyr[ 1U ] = (Int16S)imu.sensors->gyr->tangage;
			cmdRawImu.gyr[ 2U ] = (Int16S)imu.sensors->gyr->lacet;
			cmdRawImu.mag[ 0U ] = (Int16S)imu.sensors->mag->rawData.x;
			cmdRawImu.mag[ 1U ] = (Int16S)imu.sensors->mag->rawData.y;
			cmdRawImu.mag[ 2U ] = (Int16S)imu.sensors->mag->rawData.z;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdRawImu),(Int8U*)&cmdRawImu);
		}
		break;
		case MSP_SERVO:
		{
			struct {
				Int16U servo0;
				Int16U servo1;
				Int16U servo2;
				Int16U servo3;
				Int16U servo4;
				Int16U servo5;
				Int16U servo6;
				Int16U servo7;
			} cmdServo;
			
			cmdServo.servo0 = DrvServoGetTicks(0);
			cmdServo.servo1 = DrvServoGetTicks(1);
			cmdServo.servo2 = DrvServoGetTicks(2);
			cmdServo.servo3 = DrvServoGetTicks(3);
			cmdServo.servo4 = 500U;
			cmdServo.servo5 = 500U;
			cmdServo.servo6 = 500U;
			cmdServo.servo7 = 500U;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdServo),(Int8U*)&cmdServo);
		}
		break;
		case MSP_MOTOR:
		{
			struct {
				Int16U motor0;
				Int16U motor1;
				Int16U motor2;
				Int16U motor3;
				Int16U motor4;
				Int16U motor5;
				Int16U motor6;
				Int16U motor7;
			} cmdMotor;
			
			cmdMotor.motor0 = 0U;
			cmdMotor.motor1 = 0U;
			cmdMotor.motor2 = 0U;
			cmdMotor.motor3 = 0U;
			cmdMotor.motor4 = 0U;
			cmdMotor.motor5 = 0U;
			cmdMotor.motor6 = 0U;
			cmdMotor.motor7 = 0U;
			#ifdef BI
			cmdMotor.motor0 = imu.moteurs->left.speed + 1000U;
			cmdMotor.motor1 = imu.moteurs->right.speed + 1000U;
			#endif
			#ifdef QUADX
			cmdMotor.motor0 = imu.moteurs->rearRight.speed + 1000U;
			cmdMotor.motor1 = imu.moteurs->frontRight.speed + 1000U;
			cmdMotor.motor2 = imu.moteurs->rearLeft.speed + 1000U;
			cmdMotor.motor3 = imu.moteurs->frontLeft.speed + 1000U;
			#endif
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdMotor),(Int8U*)&cmdMotor);
		}
		break;
		case MSP_SET_MOTOR:
		{
			SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
			/*struct {
				Int16U motor0;
				Int16U motor1;
				Int16U motor2;
				Int16U motor3;
				Int16U motor4;
				Int16U motor5;
				Int16U motor6;
				Int16U motor7;
			} cmdMotor;*/
			SrvMotorsSetSpeed(0,1000);
		}
		break;
		case MSP_RC:
		{}
		break;
		case MSP_SET_RAW_RC:
		{
			SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
		}
		break;
		case MSP_COMP_GPS:
		{
			struct {
				Int16U distanceToHome;
				Int16U directionToHome;
				Int8U update;
			} cmdCompGPS;
			cmdCompGPS.distanceToHome = 0U;
			cmdCompGPS.directionToHome = 0U;
			cmdCompGPS.update = FALSE;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdCompGPS),(Int8U*)&cmdCompGPS);
		}
		break;
		case MSP_ATTITUDE:
		{
			struct {
				Int16S angX;
				Int16S angY;
				Int16S heading;
			} cmdAttitude;
			cmdAttitude.angX = imu.angles.roulis;
			cmdAttitude.angY = imu.angles.tangage;
			cmdAttitude.heading = imu.angles.lacet;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdAttitude),(Int8U*)&cmdAttitude);
		}
		break;
		case MSP_ALTITUDE:
		{
			struct {
				Int32S altitude;
				Int16S vario;
			} cmdAltitude;
			cmdAltitude.altitude = imu.sensors->bar->altitude * 100U;//imu.sensors->gps->altitude*100U;
			cmdAltitude.vario =  imu.sensors->var->variation;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdAltitude),(Int8U*)&cmdAltitude);
		}
		break;
		case MSP_ANALOG:
		{
			struct {
				Int8U voltage;
				Int16U intPowerMeterSum;
				Int16U rssi;
				Int16U current;
			} cmdAnalog;
			cmdAnalog.voltage = 30U;
			cmdAnalog.intPowerMeterSum = 0U;
			cmdAnalog.rssi = 0U;
			cmdAnalog.current = 10U;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdAnalog),(Int8U*)&cmdAnalog);
		}
		break;
		case MSP_RC_TUNING:
		{
			struct {
				Int8U rcRate8;
				Int8U rcExpo8;
				Int8U rollPitchRate;
				Int8U yawRate;
				Int8U dynThrPID;
				Int8U thrMid8;
				Int8U thrExpo8;
			} cmdRcTunning;
			
			cmdRcTunning.rcRate8 = 90U;
			cmdRcTunning.rcExpo8 = 65U;
			cmdRcTunning.rollPitchRate = 0U;
			cmdRcTunning.yawRate = 0U;
			cmdRcTunning.dynThrPID = 0U;
			cmdRcTunning.thrMid8 = 50U;
			cmdRcTunning.thrExpo8 = 0U;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdRcTunning),(Int8U*)&cmdRcTunning);
		}
		break;
		case MSP_SET_RC_TUNING:
		{
			SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
		}
		break;
		
		
		case MSP_BOX:
		{
			Int16U bitmapBoxes[3U] = { 0U };

			SrvMultiWiiSendMessage(inMessage.command,sizeof(bitmapBoxes),(Int8U*)&bitmapBoxes);
		}
		break;
		case MSP_SET_BOX:
		{
			SrvMultiWiiSendMessage(inMessage.command,0,NULL);
		}
		break;
		case MSP_MISC:
		{
			struct {
				Int16U intPowerTrigger1;
				Int16U minThrottle;
				Int16U maxThrottle;
				Int16U minCommand;
				Int16U failSafeThrottle;
				Int16U arm;
				Int32U lifetime;
				Int16U magDeclinaison;
				Int8U  vBatScale;
				Int8U  vBatThreshold1;
				Int8U  vBatThreshold2;
				Int8U  vBatCritical;
			} cmdMisc;

			cmdMisc.intPowerTrigger1 = 0U;
			cmdMisc.minThrottle = imu.moteurs->minThrottle;
			cmdMisc.maxThrottle = imu.moteurs->maxThrottle;
			cmdMisc.minCommand = 0U;
			cmdMisc.failSafeThrottle = imu.moteurs->minThrottle;
			cmdMisc.arm = (Int16U)imu.status.armed;
			cmdMisc.lifetime = imu.status.lifeTime;
			cmdMisc.magDeclinaison = imu.sensors->mag->declination * 10U;
			cmdMisc.vBatScale = 131U;
			cmdMisc.vBatThreshold1 = 107U;
			cmdMisc.vBatThreshold2 = 99U;
			cmdMisc.vBatCritical = 93U;

			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdMisc),(Int8U*)&cmdMisc);
		}
		break;
		case MSP_MOTOR_PINS:
		{
			struct {
				Int8U pin0;
				Int8U pin1;
				Int8U pin2;
				Int8U pin3;
				Int8U pin4;
				Int8U pin5;
				Int8U pin6;
				Int8U pin7;
			} cmdMotorsPins;

			cmdMotorsPins.pin0 = EIO_PIN_A_0;
			cmdMotorsPins.pin1 = EIO_PIN_A_1;
			cmdMotorsPins.pin2 = EIO_PIN_A_2;
			cmdMotorsPins.pin3 = EIO_PIN_A_3;
			cmdMotorsPins.pin4 = 0U;
			cmdMotorsPins.pin5 = 0U;
			cmdMotorsPins.pin6 = 0U;
			cmdMotorsPins.pin7 = 0U;

			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdMotorsPins),(Int8U*)&cmdMotorsPins);
		}
		break;
		case MSP_BOXNAMES:
		{
			Char boxes[] = 
			"ARM;"
			"ANGLE;"
			"HORIZON;";
			SrvMultiWiiSendMessage(inMessage.command,strlen(boxes),(Int8U*)&boxes);
		}
		break;
		case MSP_SERVO_CONF:
		{
			struct {
				Int16U min;
				Int16U max;
				Int16U mid;
				Int8U rate;
			} cmdServosConf[8U];

			cmdServosConf->min = PULSE_WIDTH_MIN;
			cmdServosConf->mid = (PULSE_WIDTH_MAX + PULSE_WIDTH_MIN) / 2U;
			cmdServosConf->max = PULSE_WIDTH_MAX;
			cmdServosConf->rate = 100;

			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdServosConf),(Int8U*)&cmdServosConf);
		}
		break;
		case MSP_NAV_STATUS:
		{
			
		}
		break;
		
		case MSP_RAW_GPS:
		{
			struct {
				Int8U fix;
				Int8U numberSatellites;
				Int32S latitude;
				Int32S longitude;
				Int16S altitude;
				Int16U speed;
				Int16U course;
			} cmdRawGps;
			
			cmdRawGps.fix = imu.sensors->gps->satellites > 0U ? TRUE : FALSE;
			cmdRawGps.numberSatellites = imu.sensors->gps->satellites;
			cmdRawGps.latitude = imu.sensors->gps->latitude;
			cmdRawGps.longitude = imu.sensors->gps->longitude;
			cmdRawGps.altitude = imu.sensors->gps->altitude;
			cmdRawGps.speed = imu.sensors->gps->speed;
			cmdRawGps.course = imu.sensors->gps->heading;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdRawGps),(Int8U*)&cmdRawGps);
		}
		break;
		case MSP_PID:
		{
			struct {
				Int8U p;
				Int8U i;
				Int8U d;
			} cmdPID[10U];
			
			for(Int8U i = 0U; i < 10U ; i++)
			{
				if(i < E_NB_PIDS)
				{
					float fp,fi,fd;
					SrvPIDGetValues( i ,&fp,&fi,&fd);
					cmdPID[ i ].p = (Int8U) (fp * 10U);
					cmdPID[ i ].i = (Int8U) (fi * 1000U);
					cmdPID[ i ].d = (Int8U) fd;
				}
				else
				{
					cmdPID[ i ].p = 0U;
					cmdPID[ i ].i = 0U;
					cmdPID[ i ].d = 0U;
				}
			}
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdPID),(Int8U*)&cmdPID);
		}
		break;
		case MSP_SET_PID:
		{
			struct {
				float p;
				float i;
				float d;
			} cmdPID[E_NB_PIDS];
			
			wdt_enable(WDTO_500MS);
			for(Int8U i = 0U; i < E_NB_PIDS ; i++)
			{
				cmdPID[ i ].p = (float)inMessage.data[ i * 3 ] / 10U;
				cmdPID[ i ].i = (float)inMessage.data[ (i * 3U) + 1U ] / 1000U;
				cmdPID[ i ].d = (float)inMessage.data[ (i * 3U) + 2U ];
				SrvPIDSetValues(i,cmdPID[ i ].p,cmdPID[ i ].i,cmdPID[ i ].d);
			}
					
			SrvMultiWiiSendMessage(inMessage.command,0,NULL);
		}
		break;
		case MSP_ACC_CALIBRATION:
		{
			if(imu.status.armed == DISARMED)
			{
				wdt_enable(WDTO_500MS);
				SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
				SnrAccelerometerSetCalibration();
				SnrAccelerometerSaveCalibration();
			}
		}
		break;
		case MSP_MAG_CALIBRATION:
		{
			if(imu.status.armed == DISARMED)
			{
				wdt_enable(WDTO_500MS);
				SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
				SnrMagnetometerSetCalibration();
				SnrMagnetometerSaveCalibration();
			}
		}
		break;
		case MSP_SET_MISC:
		{
			if(inMessage.size > 0U)
			{
				struct {
					Int16U intPowerTrigger1;
					Int16U minThrottle;
					Int16U maxThrottle;
					Int16U minCommand;
					Int16U failSafeThrottle;
					Int16U arm;
					Int32U lifetime;
					Int16U magDeclinaison;
					Int8U  vBatScale;
					Int8U  vBatThreshold1;
					Int8U  vBatThreshold2;
					Int8U  vBatCritical;
				} cmdMisc;

				cmdMisc.intPowerTrigger1 = (Int16U)((Int16U)(inMessage.data[1U]<<8U) + inMessage.data[0U]);
				imu.moteurs->minThrottle = (Int16U)((Int16U)(inMessage.data[3U]<<8U) + inMessage.data[2U]);
				imu.moteurs->maxThrottle = 1000U; //(Int16U)((Int16U)(inMessage.data[5U]<<8U) + inMessage.data[4U]);
				cmdMisc.minCommand =  (Int16U)((Int16U)(inMessage.data[7U]<<8U) + inMessage.data[6U]);
				cmdMisc.failSafeThrottle =  (Int16U)((Int16U)(inMessage.data[9U]<<8U) + inMessage.data[8U]);
				imu.status.armed = (Int16U)((Int16U)(inMessage.data[11U]<<8U) + inMessage.data[10U]);
				imu.status.lifeTime = (Int32U)((Int32U)(inMessage.data[15U]<<24U) + (Int32U)(inMessage.data[14U]<<16U) + (Int32U)(inMessage.data[13U]<<8U) + (Int32U)(inMessage.data[12U]));
				imu.sensors->mag->declination = (float)((Int16U)(inMessage.data[17U]<<8U) + inMessage.data[16U]) / 10U;
				cmdMisc.vBatScale = inMessage.data[15U];
				cmdMisc.vBatThreshold1 = inMessage.data[16U];
				cmdMisc.vBatThreshold2 = inMessage.data[17U];
				cmdMisc.vBatCritical = inMessage.data[18U];

				SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
				
				if(imu.status.armed == TRUE)
				{
					SrvIhmPlatformArm();
				}
				else
				{
					SrvIhmPlatformDisarm();
				}
			}
		}
		break;
		
		
		case MSP_SET_SERVO_CONF:
		{
			
			struct {
				Int16U min;
				Int16U max;
				Int16U mid;
				Int8U rate;
			} cmdServosConf[8U];

			for(Int8U i = 0U; i < 8U ; i++)
			{
				cmdServosConf[ i ].min = (Int16U)inMessage.data[ i * 7U ];
				cmdServosConf[ i ].max = (Int16U)inMessage.data[ (i * 7U) + 2U ];
				cmdServosConf[ i ].mid = (Int16U)inMessage.data[ (i * 7U) + 4U ];
				cmdServosConf[ i ].rate = (Int8U)inMessage.data[ (i * 7U) + 5U ];
			}
			
			#ifdef BI
				//left
				imu.moteurs->left.servo->min= cmdServosConf[ 0U ].min;
				imu.moteurs->left.servo->max= cmdServosConf[ 0U ].max;
				imu.moteurs->left.servo->mid= cmdServosConf[ 0U ].mid;
				//right
				imu.moteurs->right.servo->min= cmdServosConf[ 1U ].min;
				imu.moteurs->right.servo->max= cmdServosConf[ 1U ].max;
				imu.moteurs->right.servo->mid= cmdServosConf[ 1U ].mid;
			#endif
			#ifdef QUADX
				//rear right
				imu.moteurs->rearRight.servo->min= cmdServosConf[ 0U ].min;
				imu.moteurs->rearRight.servo->max= cmdServosConf[ 0U ].max;
				imu.moteurs->rearRight.servo->mid= cmdServosConf[ 0U ].mid;
				//front right
				imu.moteurs->frontRight.servo->min= cmdServosConf[ 1U ].min;
				imu.moteurs->frontRight.servo->max= cmdServosConf[ 1U ].max;
				imu.moteurs->frontRight.servo->mid= cmdServosConf[ 1U ].mid;
				//rear left
				imu.moteurs->rearLeft.servo->min= cmdServosConf[ 2U ].min;
				imu.moteurs->rearLeft.servo->max= cmdServosConf[ 2U ].max;
				imu.moteurs->rearLeft.servo->mid= cmdServosConf[ 2U ].mid;
				//front left
				imu.moteurs->frontLeft.servo->min= cmdServosConf[ 3U ].min;
				imu.moteurs->frontLeft.servo->max= cmdServosConf[ 3U ].max;
				imu.moteurs->frontLeft.servo->mid= cmdServosConf[ 3U ].mid;
			#endif			
			SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
		}
		break;
		
		case MSP_DEBUG:
		{
			static struct {
				Int16U ch1;
				Int16U ch2;
				Int16U ch3;
				Int16U ch4;
			} cmdDebug;
			
			cmdDebug.ch1 = imu.status.lifeTime;
			cmdDebug.ch2 = imu.status.lastResetCause;
			cmdDebug.ch3 = imu.sensors->var->variation;
			cmdDebug.ch4 = imu.sensors->us->distance;
			SrvMultiWiiSendMessage(inMessage.command,sizeof(cmdDebug),(Int8U*)&cmdDebug);
		}
		break;
		case MSP_EEPROM_WRITE:
		{
			SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
		}
		break;
		case MSP_RESET_CONF:
		{
			SrvMultiWiiSendMessage(inMessage.command,0U,NULL);
		}
		break;

	}
}

//permits to decode the icomming data
static Boolean SrvMultiWiiDecodeMessage()
{
	nbDataAvailable = DrvUartDataAvailable(E_UART_1);
	//minimum message is 6 char lenght
	if( nbDataAvailable >= 6U )
	{
		//loop until the data is over
		for(Int16U loop = 0U; loop < nbDataAvailable ; loop++)
		{
			//get the datum
			Int8U datum = DrvUartReadData(E_UART_1);
			
			switch ((Int8U)status)
			{
				case MULTIWII_STATE_WAIT_PREAMBLE_0:
				if(datum == MSP_PREAMBLE[ 0U ])
				{
					//save first preamble
					inMessage.preamble[ 0U ] = MSP_PREAMBLE[ 0U ];
					//go to next step
					status    = MULTIWII_STATE_WAIT_PREAMBLE_1;
				}
				break;
				case MULTIWII_STATE_WAIT_PREAMBLE_1:
				if (datum == MSP_PREAMBLE[ 1U ])
				{
					//save second preamble
					inMessage.preamble[ 1U ] = MSP_PREAMBLE[ 1U ];
					//go to next step
					status    = MULTIWII_STATE_GOT_PREAMBLE;
				}
				break;
				case MULTIWII_STATE_GOT_PREAMBLE:
				if (datum == MSP_DIRECTION_IN)
				{
					//get the message direction
					inMessage.direction = datum;
					//go to next step
					status    = MULTIWII_STATE_GOT_DIRECTION;
				}
				break;
				case MULTIWII_STATE_GOT_DIRECTION:
				{
					//get the message size
					inMessage.size = datum;
					//initialise CRC
					inMessage.crc = datum;
					//go to next step
					status    = MULTIWII_STATE_GOT_SIZE;
				}
				break;
				case MULTIWII_STATE_GOT_SIZE:
				{
					//get the message command
					inMessage.command = datum;
					//compute CRC
					inMessage.crc ^= datum;
					//reset the number of data in the message
					cptDataMessage = 0U;
					if(inMessage.size != 0U)
					{
						//go to next step
						//message with payload
						status    = MULTIWII_STATE_GOT_COMMAND;
					}
					else
					{
						//go to next step
						//message without payload, only command
						status    = MULTIWII_STATE_GOT_DATA;
					}
				}
				break;
				case MULTIWII_STATE_GOT_COMMAND:
				{
					//get data
					inMessage.data[cptDataMessage] = datum;
					//compute CRC
					inMessage.crc ^= datum;
					//incremente the number of data
					cptDataMessage++;
					//we finish to take data
					if(cptDataMessage == inMessage.size)
					{
						//go to next step
						status    = MULTIWII_STATE_GOT_DATA;
					}
				}
				break;
				case MULTIWII_STATE_GOT_DATA:
				{
					//check CRC
					if(inMessage.crc == datum)
					{
						//do what the message want
						SrvMultiWiiDoMessage();
					}
					//even if the frame is incorrect
					//go to first waitting step
					status    = MULTIWII_STATE_WAIT_PREAMBLE_0;
				}
				break;
			}
		}
	}
	return TRUE;
}

////////////////////////////////////////ISR FUNCTIONS/////////////////////////////////////////////
