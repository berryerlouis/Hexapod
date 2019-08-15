/*
 * CmpAK8975.h
 *
 * Created: 03/07/2012 11:49:50
 *  Author: berryer
 */ 


#ifndef CMPAK8975_H_
#define CMPAK8975_H_


#include "Conf/conf_hard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

#define AK8975_TWI_ADDRESS		      0x0C

#define AK8975_RA_WIA_DEVICE_ID   0x48

#define AK8975_RA_WIA             0x00
#define AK8975_RA_INFO            0x01
#define AK8975_RA_ST1             0x02
#define AK8975_RA_HXL             0x03
#define AK8975_RA_HXH             0x04
#define AK8975_RA_HYL             0x05
#define AK8975_RA_HYH             0x06
#define AK8975_RA_HZL             0x07
#define AK8975_RA_HZH             0x08
#define AK8975_RA_ST2             0x09
#define AK8975_RA_CNTL            0x0A
#define AK8975_RA_RSV             0x0B // RESERVED, DO NOT USE
#define AK8975_RA_ASTC            0x0C
#define AK8975_RA_TS1             0x0D // SHIPMENT TEST, DO NOT USE
#define AK8975_RA_TS2             0x0E // SHIPMENT TEST, DO NOT USE
#define AK8975_RA_I2CDIS          0x0F
#define AK8975_RA_ASAX            0x10
#define AK8975_RA_ASAY            0x11
#define AK8975_RA_ASAZ            0x12

#define AK8975_ST1_DRDY_BIT       0

#define AK8975_ST2_HOFL_BIT       3
#define AK8975_ST2_DERR_BIT       2

#define AK8975_CNTL_MODE_MASK     0x0F

#define AK8975_MODE_POWERDOWN     0x0
#define AK8975_MODE_SINGLE        0x1
#define AK8975_MODE_SELFTEST      0x8
#define AK8975_MODE_FUSEROM       0xF

#define AK8975_ASTC_SELF_BIT      6

#define AK8975_I2CDIS_BIT         0
 
#define MICRO_TESLA_PER_COUNT		0.3
#define AK8975_DATA_RESOLUTION		13


#define LOCAL_MAGNETIC_DECLINAISON		13.38

#define MEASURE_MAX ((1 << (AK8975_DATA_RESOLUTION - 1)) / MICRO_TESLA_PER_COUNT)
typedef struct 
{
	Int16S x; 
	Int16S y; 
	Int16S z;
}S_Mag_Angle;

////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
Boolean CmpAK8975Init(void);
Boolean CmpAK8975IsCalibrate(void);
//Magnetomer X Y Z
Boolean CmpAK8975GetMagnetomer(S_Mag_Angle *mag);

#endif /* CMPAK8975_H_ */


