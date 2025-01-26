#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define BNO055_ADDRESS 0x28

// Register addresses for gyroscope and accelerometer
#define GYR_DATA_Z_LSB 0x14
#define ACC_DATA_X_LSB 0x08

// Constants
#define DEG_TO_RAD 0.0174533
#define GRAVITY 9.81

void read_register(uint8_t reg, uint8_t *buffer, uint8_t len) {
    bcm2835_i2c_setSlaveAddress(BNO055_ADDRESS);
    bcm2835_i2c_write(&reg, 1);
    bcm2835_i2c_read((char *)buffer, len);
}

int main() {
    if (!bcm2835_init()) {
        printf("BCM2835 initialization failed\n");
        return 1;
    }

    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);

    uint8_t gyro_buffer[6], accel_buffer[6];
    while (1) {
        // Read gyroscope data
        read_register(GYR_DATA_Z_LSB, gyro_buffer, 6);
        int16_t yaw_rate_raw = (int16_t)((gyro_buffer[1] << 8) | gyro_buffer[0]);
        int16_t pitch_rate_raw = (int16_t)((gyro_buffer[3] << 8) | gyro_buffer[2]);
        int16_t roll_rate_raw = (int16_t)((gyro_buffer[5] << 8) | gyro_buffer[4]);

        // Convert to radians per second
        float yaw_rate = yaw_rate_raw * DEG_TO_RAD;
        float pitch_rate = pitch_rate_raw * DEG_TO_RAD;
        float roll_rate = roll_rate_raw * DEG_TO_RAD;

        // Read accelerometer data
        read_register(ACC_DATA_X_LSB, accel_buffer, 6);
        int16_t accel_x_raw = (int16_t)((accel_buffer[1] << 8) | accel_buffer[0]);
        int16_t accel_y_raw = (int16_t)((accel_buffer[3] << 8) | accel_buffer[2]);
        int16_t accel_z_raw = (int16_t)((accel_buffer[5] << 8) | accel_buffer[4]);

        // Scale down the raw values (move decimal place back 2)
        float accel_x = accel_x_raw / 100.0;
        float accel_y = accel_y_raw / 100.0;
        float accel_z = (accel_z_raw / 100.0) - GRAVITY;

        // Print the raw data (Gyroscope in rad/s, Accelerometer in m/s²)
        printf("Gyroscope (rad/s): Yaw=%.2f, Pitch=%.2f, Roll=%.2f\n",
               yaw_rate, pitch_rate, roll_rate);
        printf("Accelerometer (m/s²): X=%.2f, Y=%.2f, Z=%.2f\n\n",
               accel_x, accel_y, accel_z);

        usleep(100000); // Delay 100ms
    }

    bcm2835_i2c_end();
    bcm2835_close();
    return 0;
}
