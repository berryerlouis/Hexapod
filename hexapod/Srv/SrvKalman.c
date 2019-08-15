/*
 * SrvKalman.c
 *
 * Created: 06/07/2012 16:34:50
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/conf_hard.h"
                
#include "SrvKalman.h"


////////////////////////////////////////PRIVATE VARIABLES////////////////////////////////////////
/* Kalman filter variables and constants */
const float Q_angleX = 0.001; // Process noise covariance for the accelerometer - Sw
const float Q_gyroX = 0.003; // Process noise covariance for the gyro - Sw
const float R_angleX = 0.001; // Measurement noise covariance - Sv

float angleX = 0; // The angle output from the Kalman filter
float biasX = 0; // The gyro bias calculated by the Kalman filter
float PX_00 = 0, PX_01 = 0, PX_10 = 0, PX_11 = 0;
float dtX, yX, SX;
float KX_0, KX_1;

/* Kalman filter variables and constants */
const float Q_angleY = 0.001; // Process noise covariance for the accelerometer - Sw
const float Q_gyroY = 0.003; // Process noise covariance for the gyro - Sw
const float R_angleY = 0.001; // Measurement noise covariance - Sv

float angleY = 0; // The angle output from the Kalman filter
float biasY = 0; // The gyro bias calculated by the Kalman filter
float PY_00 = 0, PY_01 = 0, PY_10 = 0, PY_11 = 0;
float dtY, yY, SY;
float KY_0, KY_1;

/* Kalman filter variables and constants */
const float Q_angleZ = 0.0001; // Process noise covariance for the magnetometer - Sw
const float Q_gyroZ = 0.003; // Process noise covariance for the gyro - Sw
const float R_angleZ = 0.001; // Measurement noise covariance - Sv

float angleZ = 0; // The angle output from the Kalman filter
float biasZ = 0; // The gZro bias calculated bZ the Kalman filter
float PZ_00 = 0, PZ_01 = 0, PZ_10 = 0, PZ_11 = 0;
float dtZ, ZZ, SZ;
float KZ_0, KZ_1;

/* Kalman filter variables and constants */
const float Q_baroALT = 0.001; // Process noise covariance for the barometer - Sw
const float Q_accelALT = 0.003; // Process noise covariance for the accelerometer - Sw
const float R_angleALT = 0.03; // Measurement noise covariance - Sv

float angleALT = 0; // The angle output from the Kalman filter
float biasALT = 0; // The gZro bias calculated bZ the Kalman filter
float PALT_00 = 0, PALT_01 = 0, PALT_10 = 0, PALT_11 = 0;
float dtALT, ZALT, SALT;
float KALT_0, KALT_1;
////////////////////////////////////////PUBLIC FONCTIONS////////////////////////////////////////
void SrvKalmanFilterInit( void )
{
	angleX = 0; // The angle output from the Kalman filter
	biasX = 0; // The gyro bias calculated by the Kalman filter
	PX_00 = 0, PX_01 = 0, PX_10 = 0, PX_11 = 0;
	dtX = 0, yX = 0, SX = 0;
	KX_0 = 0, KX_1 = 0;

	angleY = 0; // The angle output from the Kalman filter
	biasY = 0; // The gyro bias calculated by the Kalman filter
	PY_00 = 0, PY_01 = 0, PY_10 = 0, PY_11 = 0;
	dtY = 0, yY = 0, SY = 0;
	KY_0 = 0, KY_1 = 0;

	angleZ = 0; // The angle output from the Kalman filter
	biasZ = 0; // The gZro bias calculated bZ the Kalman filter
	PZ_00 = 0, PZ_01 = 0, PZ_10 = 0, PZ_11 = 0;
	dtZ = 0, ZZ, SZ = 0;
	KZ_0 = 0, KZ_1 = 0;
	
	angleALT = 0; // The angle output from the Kalman filter
	biasALT = 0; // The gZro bias calculated bZ the Kalman filter
	PALT_00 = 0, PALT_01 = 0, PALT_10 = 0, PALT_11 = 0;
	dtALT = 0, ZALT = 0, SALT = 0;
	KALT_0 = 0, KALT_1 = 0;
}


float SrvKalmanFilterX(float newAngle, float newRate, float dtime)
{						
	// Discrete Kalman filter time update equations - Time Update ("Predict")
	// Update xhat - Project the state ahead
	/* Step 1 */
	dtX = (float)dtime;
	angleX += dtX * (newRate - biasX);

	// Update estimation error covariance - Project the error covariance ahead
	/* Step 2 */
	PX_00 += dtX * (dtX*PX_11 - PX_01 - PX_10 + Q_angleX);
	PX_01 -= dtX * PX_11;
	PX_10 -= dtX * PX_11;
	PX_11 += Q_gyroX * dtX;

	// Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
	// Calculate Kalman gain - Compute the Kalman gain
	/* Step 4 */
	SX = PX_00 + R_angleX;
	/* Step 5 */
	KX_0 = PX_00 / SX;
	KX_1 = PX_10 / SX;

	// Calculate angle and bias - Update estimate with measurement zk (newAngle)
	/* Step 3 */
	yX = newAngle - angleX;
	/* Step 6 */
	angleX += KX_0 * yX;
	biasX += KX_1 * yX;

	// Calculate estimation error covariance - Update the error covariance
	/* Step 7 */
	PX_00 -= KX_0 * PX_00;
	PX_01 -= KX_0 * PX_01;
	PX_10 -= KX_1 * PX_00;
	PX_11 -= KX_1 * PX_01;
	
    return angleX;
}

float SrvKalmanFilterY(float newAngle, float newRate, float dtime)
{
    // Discrete Kalman filter time update equations - Time Update ("Predict")
    // Update xhat - Project the state ahead
    /* Step 1 */
	dtY = (float)dtime;
    angleY += dtY * (newRate - biasY);

    // Update estimation error covariance - Project the error covariance ahead
    /* Step 2 */
    PY_00 += dtY * (dtY*PY_11 - PY_01 - PY_10 + Q_angleY);
    PY_01 -= dtY * PY_11;
    PY_10 -= dtY * PY_11;
    PY_11 += Q_gyroY * dtY;

    // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
    // Calculate Kalman gain - Compute the Kalman gain
    /* Step 4 */
    SY = PY_00 + R_angleY;
    /* Step 5 */
    KY_0 = PY_00 / SY;
    KY_1 = PY_10 / SY;

    // Calculate angle and bias - Update estimate with measurement zk (newAngle)
    /* Step 3 */
    yY = newAngle - angleY;
    /* Step 6 */
    angleY += KY_0 * yY;
    biasY += KY_1 * yY;

    // Calculate estimation error covariance - Update the error covariance
    /* Step 7 */
    PY_00 -= KY_0 * PY_00;
    PY_01 -= KY_0 * PY_01;
    PY_10 -= KY_1 * PY_00;
    PY_11 -= KY_1 * PY_01;
    
    return angleY;
}

float SrvKalmanFilterZ(float newAngle, float newRate, float dtime)
{
        dtZ = dtime; // Convert from microseconds to seconds

        // Discrete Kalman filter time update equations - Time Update ("Predict")
        // Update xhat - Project the state ahead
        angleZ += dtZ * (newRate - biasZ);

        // Update estimation error covariance - Project the error covariance ahead
        PZ_00 += -dtZ * (PZ_10 + PZ_01) + Q_angleZ * dtZ;
        PZ_01 += -dtZ * PZ_11;
        PZ_10 += -dtZ * PZ_11;
        PZ_11 += +Q_gyroZ * dtZ;

        // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
        // Calculate Kalman gain - Compute the Kalman gain
        SZ = PZ_00 + R_angleZ;
        KZ_0 = PZ_00 / SZ;
        KZ_1 = PZ_10 / SZ;

        // Calculate angle and resting rate - Update estimate with measurement zk
        ZZ = newAngle - angleZ;
        angleZ += KZ_0 * ZZ;
        biasZ += KZ_1 * ZZ;

        // Calculate estimation error covariance - Update the error covariance
        PZ_00 -= KZ_0 * PZ_00;
        PZ_01 -= KZ_0 * PZ_01;
        PZ_10 -= KZ_1 * PZ_00;
        PZ_11 -= KZ_1 * PZ_01;

        return angleZ;
}

float SrvKalmanFilterAlt(float newAngle, float newRate, float dtime)
{
        dtALT = dtime; // Convert from microseconds to seconds

        // Discrete Kalman filter time update equations - Time Update ("Predict")
        // Update xhat - Project the state ahead
        angleALT += dtALT * (newRate - biasALT);

        // Update estimation error covariance - Project the error covariance ahead
        PALT_00 += -dtALT * (PALT_10 + PALT_01) + Q_baroALT * dtALT;
        PALT_01 += -dtALT * PALT_11;
        PALT_10 += -dtALT * PALT_11;
        PALT_11 += +Q_accelALT * dtALT;

        // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
        // Calculate Kalman gain - Compute the Kalman gain
        SALT = PALT_00 + R_angleALT;
        KALT_0 = PALT_00 / SALT;
        KALT_1 = PALT_10 / SALT;

        // Calculate angle and resting rate - Update estimate with measurement zk
        ZALT = newAngle - angleALT;
        angleALT += KALT_0 * ZALT;
        biasALT += KALT_1 * ZALT;

        // Calculate estimation error covariance - Update the error covariance
        PALT_00 -= KALT_0 * PALT_00;
        PALT_01 -= KALT_0 * PALT_01;
        PALT_10 -= KALT_1 * PALT_00;
        PALT_11 -= KALT_1 * PALT_01;

        return angleALT;
}
