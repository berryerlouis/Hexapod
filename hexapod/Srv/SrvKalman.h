/*
 * SrvKalman.h
 *
 * Created: 06/07/2012 16:35:15
 *  Author: berryer
 */ 


#ifndef SRVKALMAN_H_
#define SRVKALMAN_H_


#include "Tools/tools_typedefs.h"

void SrvKalmanFilterInit( void );
float SrvKalmanFilterX(float newAngle, float newRate, float dtime) ;
float SrvKalmanFilterY(float newAngle, float newRate, float dtime) ;
float SrvKalmanFilterZ(float newAngle, float newRate, float dtime) ;
float SrvKalmanFilterAlt(float newAngle, float newRate, float dtime) ;
#endif /* SRVKALMAN_H_ */