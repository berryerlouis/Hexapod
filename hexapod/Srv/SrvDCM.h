/*
 * SrvDCM.h
 *
 * Created: 31/05/2012 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVDCM_H_
#define SRVDCM_H_


#include "Cmps/CmpBMA150.h"
#include "Cmps/CmpITG3200.h"
#include "Cmps/CmpAK8975.h"

#include "Tools/tools_typedefs.h"


void Normalize(void);
float Vector_Dot_Product(float vector1[3],float vector2[3]);
void Drift_correction(float MAG_Heading);
void Matrix_update(float interval);
void Euler_angles(void);


//init des moteurs
void SrvDCMInit (void) ;

extern float roll;
extern float pitch;
extern float yaw;

extern S_Gyr_Angle rotation;
extern S_Acc_Angle acceleration;
extern S_Mag_Angle magnet;

#endif /* SRVDCM_H_ */