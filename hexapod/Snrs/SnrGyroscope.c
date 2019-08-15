////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include   "SnrGyroscope.h"

#ifdef GYR_L3G4200D
	#include   "Cmps/CmpL3G4200D.h"
#endif // GYR_L3G4200D
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_SAMPLE_TO_CALIB	250U
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static Gyroscope gyroscope;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean SnrGyroscopeInit ( E_Gyr_Setup setup)
{
	Boolean oSuccess = FALSE;
	
#ifdef GYR_L3G4200D
	gyroscope.data = &l3g4200dData;
	gyroscope.functions = &l3g4200d;
#endif // GYR_L3G4200D


	//init du composant
	if( TRUE == gyroscope.functions->init(setup) )
	{	
		//get last calibration
		if( DrvEepromIsConfigured() == TRUE )
		{
			Int16S calib[ 3U ];
			DrvEepromReadGyr(calib);
			gyroscope.data->calibrationData.axis.axes[ 0U ] = calib[ 0U ];
			gyroscope.data->calibrationData.axis.axes[ 1U ] = calib[ 1U ];
			gyroscope.data->calibrationData.axis.axes[ 2U ] = calib[ 2U ];
			gyroscope.data->calibrationData.status = TRUE;
		}
		else
		{
			gyroscope.data->calibrationData.status = FALSE;
			//set calibration to 0
			for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
			{
				gyroscope.data->calibrationData.axis.axes[ loopAxis ] = 0;
			}
		}
		oSuccess = TRUE;
	}

	return oSuccess;
}
Boolean SnrGyroscopeGetRotation	( void )
{
	Boolean oSuccess = FALSE;
	AxisFloat temp;
	if(gyroscope.functions->readData() == TRUE)
	{
		for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
		{
			if(!(gyroscope.data->rawData.axes[ loopAxis ] >= gyroscope.data->noise.axes[ loopAxis ] || 
				gyroscope.data->rawData.axes[ loopAxis ] <= -gyroscope.data->noise.axes[ loopAxis ]))
			{
				gyroscope.data->rawData.axes[ loopAxis ] = 0;
			}
			
			temp.axes[ loopAxis ] = (gyroscope.data->rawData.axes[ loopAxis ] - 
									 gyroscope.data->calibrationData.axis.axes[ loopAxis ]) * 
									 gyroscope.data->degreePerSeconds ;
			
		}
		
		gyroscope.data->roulis = temp.x;
		gyroscope.data->tangage = temp.y;
		gyroscope.data->lacet = temp.z;
				
		oSuccess = TRUE;
	}
	
	return oSuccess;
}

//Get temperature
Int8S SnrGyroscopeGetTemperature(void)
{
	return gyroscope.functions->readTemperature();
}

Boolean SnrGyroscopeIsCalibrate	( void )
{
	return gyroscope.data->calibrationData.status ;
}
Boolean SnrGyroscopeSetCalibration ( void )
{
	gyroscope.data->calibrationData.status = FALSE;
	for( Int8U loopCalib = 0; loopCalib <  NB_SAMPLE_TO_CALIB; loopCalib++)
	{
		if( TRUE == gyroscope.functions->readData())
		{
			for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
			{
				gyroscope.data->calibrationData.axis.axes[ loopAxis ] = gyroscope.data->rawData.axes[ loopAxis ];
			}
		}
		else
		{
			return gyroscope.data->calibrationData.status;
		}
	}
	
	// calibration was done
	for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
	{
		gyroscope.data->calibrationData.axis.axes[ loopAxis ] = gyroscope.data->calibrationData.axis.axes[ loopAxis ] / NB_SAMPLE_TO_CALIB;
	}	

	//get noise
	for( Int8U loopCalib = 0; loopCalib <  NB_SAMPLE_TO_CALIB; loopCalib++)
	{
		if( TRUE == gyroscope.functions->readData())
		{
			for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
			{
				if( 
				    (gyroscope.data->rawData.axes[ loopAxis ] - gyroscope.data->calibrationData.axis.axes[ loopAxis ]) >
				    gyroscope.data->noise.axes[loopAxis]
				  )
				{
					gyroscope.data->noise.axes[loopAxis] = gyroscope.data->rawData.axes[ loopAxis ] - 
															gyroscope.data->calibrationData.axis.axes[ loopAxis ];
				}
				else if(
				    (gyroscope.data->rawData.axes[ loopAxis ] - gyroscope.data->calibrationData.axis.axes[ loopAxis ]) <
				     gyroscope.data->noise.axes[loopAxis]
				)
				{
					gyroscope.data->noise.axes[loopAxis] = -gyroscope.data->rawData.axes[ loopAxis ] -
					gyroscope.data->calibrationData.axis.axes[ loopAxis ];
				}
			}
		}
		else
		{
			return gyroscope.data->calibrationData.status;
		}
	}
	
	for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
	{
		gyroscope.data->noise.axes[loopAxis] = gyroscope.data->noise.axes[loopAxis] / 100.0;
	}
	
	gyroscope.data->calibrationData.status = TRUE;
	
	return gyroscope.data->calibrationData.status;
}
Boolean	SnrGyroscopeSaveCalibration	( void )
{
	Int16S calibrationDataTab[ 3U ] =
	{
		(Int16S)gyroscope.data->calibrationData.axis.x,
		(Int16S)gyroscope.data->calibrationData.axis.y,
		(Int16S)gyroscope.data->calibrationData.axis.z
	};
	DrvEepromWriteAcc(calibrationDataTab);
	return TRUE;
}

GyroscopeData* SnrGyroscopeGetStruct( void )
{
	return gyroscope.data;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
