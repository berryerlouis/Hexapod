////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "SrvBody.h"
#include "../Cmps/CmpNRF24L01.h"



////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
SBody body;


//Fonction d'initialisation
Boolean SrvBodyInit ( void ) 
{
	body.legs = DrvLegGetLegs();
	
	body.battery = SrvBatteryGetStruct();
	
	body.imu = SrvImuSimpleGetSensor();
	
	body.behavior = E_BODY_STOP;
	
	body.elevation = 5;
	
	body.x = 0;
	body.y = 0;
	
	//CmpNRF24L01Init(EIO_PIN_D_4);
	
	return TRUE;
}

//Fonction de dispatching d'evenements
void SrvBodyUpdate (void)
{
}

//Set Position
Boolean SrvBodySetPosition ( int8_t x, int8_t y )
{
	body.x = x;
	body.y = y;
	
	DrvLegSetXYZ(E_LEG_U_L,10,50,body.elevation + body.x + body.y,500);
	DrvLegSetXYZ(E_LEG_U_R,-10,50,body.elevation + body.x - body.y,500);
	
	DrvLegSetXYZ(E_LEG_M_L,0,50,body.elevation + body.y,500 );
	DrvLegSetXYZ(E_LEG_M_R,0,50,body.elevation - body.y,500);
	
	DrvLegSetXYZ(E_LEG_B_L,-10,50,body.elevation - body.x + body.y,500);
	DrvLegSetXYZ(E_LEG_B_R,10,50,body.elevation - body.x - body.y,500);
	
	return TRUE;
}
//Set Elevation
Boolean SrvBodySetElevation ( uint8_t elev, uint16_t delay )
{
	body.elevation = elev;
	int16_t x;
	int16_t y;
	int16_t z;
	DrvLegGetXYZ(E_LEG_U_L,&x,&y,&z);
	DrvLegSetXYZ(E_LEG_U_L,x,y,elev,delay);
	DrvLegGetXYZ(E_LEG_M_L,&x,&y,&z);
	DrvLegSetXYZ(E_LEG_M_L,x,y,elev,delay);
	DrvLegGetXYZ(E_LEG_B_L,&x,&y,&z);
	DrvLegSetXYZ(E_LEG_B_L,x,y,elev,delay);
	DrvLegGetXYZ(E_LEG_U_R,&x,&y,&z);
	DrvLegSetXYZ(E_LEG_U_R,x,y,elev,delay);
	DrvLegGetXYZ(E_LEG_M_R,&x,&y,&z);
	DrvLegSetXYZ(E_LEG_M_R,x,y,elev,delay);
	DrvLegGetXYZ(E_LEG_B_R,&x,&y,&z);
	DrvLegSetXYZ(E_LEG_B_R,x,y,elev,delay);
	
	return TRUE;
}
//Get Elevation
uint8_t SrvBodyGetElevation ( void )
{
	return body.elevation;
}
//Get Behavior
EBodyBehavior SrvBodyGetBehavior ( void )
{
	return body.behavior;
}
//Set Behavior
Boolean SrvBodySetBehavior ( EBodyBehavior pos, uint16_t delay )
{
	body.behavior = pos;
	if(body.behavior == E_BODY_STOP)
	{
		DrvLegSetXYZ(E_LEG_U_L,0,0,0,delay);
		DrvLegSetXYZ(E_LEG_M_L,0,0,0,delay);
		DrvLegSetXYZ(E_LEG_B_L,0,0,0,delay);
		DrvLegSetXYZ(E_LEG_U_R,0,0,0,delay);
		DrvLegSetXYZ(E_LEG_M_R,0,0,0,delay);
		DrvLegSetXYZ(E_LEG_B_R,0,0,0,delay);
		return TRUE;
	}
	else if(body.behavior == E_BODY_STAR)
	{
		DrvLegSetXYZ(E_LEG_U_L,8,0,0,delay);
		DrvLegSetXYZ(E_LEG_M_L,0,0,0,delay);
		DrvLegSetXYZ(E_LEG_B_L,-8,0,0,delay);
		DrvLegSetXYZ(E_LEG_U_R,-8,0,0,delay);
		DrvLegSetXYZ(E_LEG_M_R,0,0,0,delay);
		DrvLegSetXYZ(E_LEG_B_R,8,0,0,delay);
		return TRUE;
	}
	else if(body.behavior == E_BODY_STRAIGHT)
	{
		DrvLegSetXYZ(E_LEG_U_L,0,0,50,delay);
		DrvLegSetXYZ(E_LEG_M_L,0,0,50,delay);
		DrvLegSetXYZ(E_LEG_B_L,0,0,50,delay);
		DrvLegSetXYZ(E_LEG_U_R,0,0,50,delay);
		DrvLegSetXYZ(E_LEG_M_R,0,0,50,delay);
		DrvLegSetXYZ(E_LEG_B_R,0,0,50,delay);
		return TRUE;
	}
	return FALSE;
}
