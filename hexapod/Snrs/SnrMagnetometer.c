////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include   "SnrGyroscope.h"

#ifdef MAG_HMC5883
	#include   "Cmps/CmpHMC5883.h"
#endif // MAG_HMC5883
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_SAMPLE_TO_CALIB	250U
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static Magnetometer magnetometer;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean SnrMagnetometerInit ( E_Mag_Setup setup )
{
	Boolean oSuccess = FALSE;
	
#ifdef MAG_HMC5883
	magnetometer.data = &hmc5883Data;
	magnetometer.functions = &hmc5883;
#endif // MAG_HMC5883


	//init du composant
	if( TRUE == magnetometer.functions->init(setup) )
	{
		//set declination
		magnetometer.data->declination = LOCAL_MAGNETIC_DECLINAISON;
		//get last calibration
		if( DrvEepromIsConfigured() == TRUE )
		{
			Int16S calib[ 3U ];
			DrvEepromReadMag(calib);
			magnetometer.data->calibrationData.axis.axes[ 0U ] = calib[ 0U ];
			magnetometer.data->calibrationData.axis.axes[ 1U ] = calib[ 1U ];
			magnetometer.data->calibrationData.axis.axes[ 2U ] = calib[ 2U ];
			magnetometer.data->calibrationData.status = TRUE;
		}
		else
		{
			//calib only on demand
			magnetometer.data->calibrationData.status = TRUE;
			//set calibration to 0
			for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
			{
				magnetometer.data->calibrationData.axis.axes[ loopAxis ] = 0;
			}
		}
		oSuccess = TRUE;
	}

	return oSuccess;
}
Boolean SnrMagnetometerGetScaleData	( void )
{
	//read raw data
	magnetometer.functions->readData();
	//set the gain
	for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
	{
		magnetometer.data->rawData.axes[ loopAxis ] *= magnetometer.data->gainData.axes[ loopAxis ];
	}
	
	//set the calibration offset
	if( magnetometer.data->calibrationData.status != FALSE )
	{
		for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
		{
			magnetometer.data->rawData.axes[ loopAxis ] -= magnetometer.data->calibrationData.axis.axes[ loopAxis ];
		}
	}
	return TRUE;
}

Boolean SnrMagnetometerIsCalibrate	( void )
{
	return magnetometer.data->calibrationData.status ;
}

Boolean SnrMagnetometerSetCalibration ( void )
{
	MagnetometerRawData calibrationDataMinTemp;
	MagnetometerRawData calibrationDataMaxTemp;
	
	//if calibration was already done
	if( magnetometer.data->calibrationData.status != FALSE )
	{
		magnetometer.data->calibrationData.status = FALSE;
		for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
		{
			magnetometer.data->calibrationData.axis.axes[ loopAxis ] = 0.0F;
		}
	}
	
	//first read
	SnrMagnetometerGetScaleData();
	for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
	{		
		calibrationDataMinTemp.axes[ loopAxis ] = magnetometer.data->rawData.axes[ loopAxis ];
		calibrationDataMaxTemp.axes[ loopAxis ] = magnetometer.data->rawData.axes[ loopAxis ];
	}
	
	
	Int32U timeTest = 0UL;
	timeTest = (Int32U)((Int32U)DrvTickGetTimeUs() + (Int32U)((Int32U)10000UL * 1000UL));
	do
	{
		if(DrvTickGetTimeUs() >= timeTest)
		{
			break;
		}
		else
		{
			wdt_enable(WDTO_15MS);
			//next read
			SnrMagnetometerGetScaleData();
			for( Int8U loopAxis = 0U ; loopAxis < NB_AXIS ; loopAxis++ )
			{
				//get min and max for x,y,z axis
				if( magnetometer.data->rawData.axes[ loopAxis ] < calibrationDataMinTemp.axes[ loopAxis ] )
				{
					calibrationDataMinTemp.axes[ loopAxis ] = magnetometer.data->rawData.axes[ loopAxis ];
				}
				else if( magnetometer.data->rawData.axes[ loopAxis ] > calibrationDataMaxTemp.axes[ loopAxis ])
				{
					calibrationDataMaxTemp.axes[ loopAxis ] = magnetometer.data->rawData.axes[ loopAxis ];
				}
				magnetometer.data->calibrationData.axis.axes[loopAxis] = (calibrationDataMinTemp.axes[ loopAxis ] + calibrationDataMaxTemp.axes[ loopAxis ])>>1U;
			}
		}
	} while (TRUE);
	
	//the calibration was done
	magnetometer.data->calibrationData.status = TRUE;
	return TRUE;
	
}
Boolean	SnrMagnetometerSaveCalibration	( void )
{
	/*Int16S calibrationDataTab[ 3U ] =
	{
		(Int16S)magnetometer.data->calibrationData.x,
		(Int16S)magnetometer.data->calibrationData.y,
		(Int16S)magnetometer.data->calibrationData.z
	};
	DrvEepromWriteAcc(calibrationDataTab);*/
	return TRUE;
}

MagnetometerData* SnrMagnetometerGetStruct( void )
{
	return magnetometer.data;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
