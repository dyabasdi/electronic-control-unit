#include <stdint.h>
#include <stdio.h>
#include <processor.h>

GPS initializeGPS(){
    GPS gps;
    gps.longitude = 0.0;
    gps.latitude = 0.0;
    gps.headingAngle = 0.0;
    return gps;
}

IMU initializeIMU(){
    IMU imu;
    imu.ax = 0.0;
    imu.ay = 0.0;
    imu.az = 0.0;
    imu.mx = 0.0;
    imu.my = 0.0;
    imu.mz = 0.0;
    return imu;
}

void updateIMU(IMU *imuptr){

}

void updateGPS(GPS *gpsptr){

}

void updateSensorData(GPS *gpsptr, IMU *imuptr){

}


void processInputs100hz(){
    // check for init state
    if (!startupTimer){
        GPS gps = initializeGPS();
        IMU imu = initializeIMU();
    }
}