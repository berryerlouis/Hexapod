/*
 * CmpBMA150.h
 *
 * Created: 03/07/2012 11:49:50
 *  Author: berryer
 */ 


#ifndef CMPBMA150_H_
#define CMPBMA150_H_


#include "Conf/conf_hard.h"
#include "Tools/tools_typedefs.h"


////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

#define BMA150_TWI_ADDRESS			0x38

#define BMA150_RA_CHIP_ID           0x00
#define BMA150_RA_VERSION           0x01
#define BMA150_RA_X_AXIS_LSB        0x02
#define BMA150_RA_X_AXIS_MSB        0x03
#define BMA150_RA_Y_AXIS_LSB        0x04
#define BMA150_RA_Y_AXIS_MSB        0x05
#define BMA150_RA_Z_AXIS_LSB        0x06
#define BMA150_RA_Z_AXIS_MSB        0x07
#define BMA150_RA_TEMP_RD           0x08
#define BMA150_RA_SMB150_STATUS     0x09
#define BMA150_RA_SMB150_CTRL       0x0a
#define BMA150_RA_SMB150_CONF1      0x0b
#define BMA150_RA_LG_THRESHOLD      0x0c
#define BMA150_RA_LG_DURATION       0x0d
#define BMA150_RA_HG_THRESHOLD      0x0e
#define BMA150_RA_HG_DURATION       0x0f
#define BMA150_RA_MOTION_THRS       0x10
#define BMA150_RA_HYSTERESIS        0x11
#define BMA150_RA_CUSTOMER1         0x12
#define BMA150_RA_CUSTOMER2         0x13
#define BMA150_RA_RANGE_BWIDTH      0x14
#define BMA150_RA_SMB150_CONF2      0x15
#define BMA150_RA_OFFS_GAIN_X       0x16
#define BMA150_RA_OFFS_GAIN_Y       0x17
#define BMA150_RA_OFFS_GAIN_Z       0x18
#define BMA150_RA_OFFS_GAIN_T       0x19
#define BMA150_RA_OFFSET_X          0x1a
#define BMA150_RA_OFFSET_Y          0x1b
#define BMA150_RA_OFFSET_Z          0x1c
#define BMA150_RA_OFFSET_T          0x1d

#define BMA150_CHIP_ID				0x02
#define BMA150_X_AXIS_LSB_BIT          7
#define BMA150_X_AXIS_LSB_LENGTH       2
#define BMA150_X_NEW_DATA_BIT          0

#define BMA150_Y_AXIS_LSB_BIT          7
#define BMA150_Y_AXIS_LSB_LENGTH       2
#define BMA150_Y_NEW_DATA_BIT          0

#define BMA150_Z_AXIS_LSB_BIT          7
#define BMA150_Z_AXIS_LSB_LENGTH       2
#define BMA150_Z_NEW_DATA_BIT          0

#define BMA150_STATUS_HG_BIT           0
#define BMA150_STATUS_LG_BIT           1
#define BMA150_HG_LATCHED_BIT          2
#define BMA150_LG_LATCHED_BIT          3
#define BMA150_ALERT_PHASE_BIT         4
#define BMA150_ST_RESULT_BIT           7

#define BMA150_SLEEP_BIT               0
#define BMA150_SOFT_RESET_BIT          1
#define BMA150_ST0_BIT                 2
#define BMA150_ST1_BIT                 3
#define BMA150_EEW_BIT                 4
#define BMA150_UPDATE_IMAGE_BIT        5
#define BMA150_RESET_INT_BIT           6

#define BMA150_ENABLE_LG_BIT           0
#define BMA150_ENABLE_HG_BIT           1
#define BMA150_COUNTER_LG_BIT          3
#define BMA150_COUNTER_LG_LENGTH       2
#define BMA150_COUNTER_HG_BIT          5
#define BMA150_COUNTER_HG_LENGTH       2
#define BMA150_ANY_MOTION_BIT          6
#define BMA150_ALERT_BIT               7

#define BMA150_LG_HYST_BIT             2
#define BMA150_LG_HYST_LENGTH          3
#define BMA150_HG_HYST_BIT             5
#define BMA150_HG_HYST_LENGTH          3
#define BMA150_ANY_MOTION_DUR_BIT      7
#define BMA150_ANY_MOTION_DUR_LENGTH   2

#define BMA150_BANDWIDTH_MASK          0x03
#define BMA150_RANGE_MASK			   0x18

#define BMA150_WAKE_UP_BIT             0
#define BMA150_WAKE_UP_PAUSE_BIT       2
#define BMA150_WAKE_UP_PAUSE_LENGTH    2
#define BMA150_SHADOW_DIS_BIT          3
#define BMA150_LATCH_INT_BIT           4
#define BMA150_NEW_DATA_INT_BIT        5
#define BMA150_ENABLE_ADV_INT_BIT      6
#define BMA150_SPI4_BIT                7

/* range and bandwidth */
#define BMA150_RANGE_2G                0 << 3
#define BMA150_RANGE_4G                1 << 3
#define BMA150_RANGE_8G                2 << 3

#define BMA150_BW_25HZ                 0
#define BMA150_BW_50HZ                 1
#define BMA150_BW_100HZ                2
#define BMA150_BW_190HZ                3
#define BMA150_BW_375HZ                4
#define BMA150_BW_750HZ                5
#define BMA150_BW_1500HZ               6

/* mode settings */
#define BMA150_MODE_NORMAL             0
#define BMA150_MODE_SLEEP              1


#define BMA150_1G					   256


typedef struct 
{
	Int16S x; 
	Int16S y; 
	Int16S z;
}S_Acc_Angle;

////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

//fonction init du capteur
Boolean CmpBMA150Init(void);
//Acceleration X Y Z
Boolean CmpBMA150GetAcceleration(S_Acc_Angle *acc);
Boolean CmpBMA150IsCalibrate(void);

#endif /* CMPBMA150_H_ */