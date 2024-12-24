#ifndef BNO055_DATA_H
#define BNO055_DATA_H

#include <stdint.h>

void read_register(uint8_t reg, uint8_t *buffer, uint8_t len);
void read_accel_data(float *accel_x, float *accel_y, float *accel_z);
void process_accel_data(float *accel_x, float *accel_y, float *accel_z);

#endif // BNO055_DATA_H
