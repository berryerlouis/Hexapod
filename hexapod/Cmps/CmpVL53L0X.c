/*
 * CmpVL53L0X.c
 *
 * Created: 26/05/2020 15:18:32
 *  Author: louis
 */ 

#include "CmpVL53L0X.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvTwi.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

Boolean CmpVL53L0XInit( void )
{
	Boolean oSuccess = FALSE;
	uint8_t modelId = 0x00U;
	uint8_t reg = 0x00U;
	uint8_t stop = 0x00U;
	
	//check device
	DrvTwiReadReg( VL53L0X_ADDRESS, VL53L0X_IDENTIFICATION_MODEL_ID, &modelId );
	if(modelId != 0xEEU)
	{
		oSuccess = TRUE;
	}
	
	DrvTwiReadReg(VL53L0X_ADDRESS, VL53L0X_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV, &reg);
	DrvTwiWriteReg(VL53L0X_ADDRESS,VL53L0X_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV, reg | 0x01); // set bit 0
	  
	  // "Set I2C standard mode"
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x88, 0x00);

	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x80, 0x01);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0xFF, 0x01);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x00, 0x00);
	DrvTwiReadReg(VL53L0X_ADDRESS, 0x91, &reg);
	stop = reg;
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x00, 0x01);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0xFF, 0x00);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x80, 0x00);
	
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x80, 0x01);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0xFF, 0x01);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x00, 0x00);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x91, stop);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x00, 0x01);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0xFF, 0x00);
	DrvTwiWriteReg(VL53L0X_ADDRESS, 0x80, 0x00);

	DrvTwiWriteReg(VL53L0X_ADDRESS, VL53L0X_SYSRANGE_START, 0x01);
	
	uint16_t range;
	DrvTwiReadRegBuf(VL53L0X_ADDRESS, VL53L0X_RESULT_RANGE_STATUS + 10, (Int8U*)&range,2);
	

	DrvTwiWriteReg(VL53L0X_ADDRESS, VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);
	return oSuccess;
}
Int16U CmpVL53L0XGetDistance( void )
{
	return 0;
}