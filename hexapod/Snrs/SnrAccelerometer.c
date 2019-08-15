////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include   "SnrAccelerometer.h"

#ifdef ACC_LIS331DLH
	#include   "Cmps/CmpLIS331DLH.h"
#endif // ACC_LIS331DLH
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_SAMPLE_TO_CALIB	250U
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static Accelerometer accelerometer;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean SnrAccelerometerInit ( E_Acc_Setup setup)
{
	Boolean oSuccess = FALSE;
	
#ifdef ACC_LIS331DLH
	accelerometer.data = &lis331dlhData;
	accelerometer.functions = &lis331dlh;
#endif // ACC_LIS331DLH


	//init du composant
	if( TRUE == accelerometer.functions->init(setup) )
	{
		//get last calibration
		if( DrvEepromIsConfigured() == TRUE )
		{
			Int16S calib[ 3U ];
			DrvEepromReadAcc(calib);
			accelerometer.data->calibrationData.axis.axes[ 0U ] = calib[ 0U ];
			accelerometer.data->calibrationData.axis.axes[ 1U ] = calib[ 1U ];
			accelerometer.data->calibrationData.axis.axes[ 2U ] = calib[ 2U ];
			accelerometer.data->calibrationData.status = TRUE;
		}
		else
		{
			accelerometer.data->calibrationData.status = FALSE;
			//set calibration to 0
			for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
			{
				accelerometer.data->calibrationData.axis.axes[ loopAxis ] = 0;
			}
		}
		oSuccess = TRUE;
	}

	return oSuccess;
}
Boolean SnrAccelerometerGetAcceleration	( void )
{
	Boolean oSuccess = FALSE;
	if(accelerometer.functions->readData() == TRUE)
	{
		for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
		{
			accelerometer.data->rawData.axes[ loopAxis ]  -= accelerometer.data->calibrationData.axis.axes[ loopAxis ];
		}
		oSuccess = TRUE;
	}
	return oSuccess;
}
Boolean SnrAccelerometerIsCalibrate	( void )
{
	return accelerometer.data->calibrationData.status ;
}
Boolean SnrAccelerometerSetCalibration ( void )
{
	for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
	{
		accelerometer.data->calibrationData.axis.axes[ loopAxis ] = 0;
	}
	accelerometer.data->calibrationData.status = FALSE;
	for( Int8U loopCalib = 0; loopCalib < NB_SAMPLE_TO_CALIB; loopCalib++)
	{
		if(accelerometer.functions->readData() == TRUE)
		{
			for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
			{
				accelerometer.data->calibrationData.axis.axes[ loopAxis ] += accelerometer.data->rawData.axes[ loopAxis ];
			}
		}
		else
		{
			return accelerometer.data->calibrationData.status ;
		}
	}
	
	for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
	{
		accelerometer.data->calibrationData.axis.axes[ loopAxis ] = accelerometer.data->calibrationData.axis.axes[ loopAxis ] / NB_SAMPLE_TO_CALIB;
	}
	accelerometer.data->calibrationData.axis.z -= accelerometer.data->gravity;
	
	accelerometer.data->calibrationData.status  = TRUE;
	return accelerometer.data->calibrationData.status ;
}
Boolean	SnrAccelerometerSaveCalibration	( void )
{
	Int16S calibrationDataTab[ 3U ] =
	{
		(Int16S)accelerometer.data->calibrationData.axis.x,
		(Int16S)accelerometer.data->calibrationData.axis.y,
		(Int16S)accelerometer.data->calibrationData.axis.z
	};
	DrvEepromWriteAcc(calibrationDataTab);
	return TRUE;
}


AccelerometerData* SnrAccelerometerGetStruct( void )
{
	return accelerometer.data;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
