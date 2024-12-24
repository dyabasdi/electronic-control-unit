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
#define FILTER_SIZE 5       // Number of samples for the moving average
#define ACCEL_THRESHOLD 50.0 // Maximum valid acceleration in m/s²
#define GYRO_THRESHOLD 10.0  // Maximum valid rotational velocity in rad/s

// Circular buffer for filtering
float gyro_yaw_buffer[FILTER_SIZE] = {0}, gyro_pitch_buffer[FILTER_SIZE] = {0}, gyro_roll_buffer[FILTER_SIZE] = {0};
float accel_x_buffer[FILTER_SIZE] = {0}, accel_y_buffer[FILTER_SIZE] = {0}, accel_z_buffer[FILTER_SIZE] = {0};
int buffer_index = 0;

// Last valid data
float last_valid_accel_x = 0, last_valid_accel_y = 0, last_valid_accel_z = 0;
float last_valid_yaw_rate = 0, last_valid_pitch_rate = 0, last_valid_roll_rate = 0;

void read_register(uint8_t reg, uint8_t *buffer, uint8_t len) {
    bcm2835_i2c_setSlaveAddress(BNO055_ADDRESS);
    bcm2835_i2c_write(&reg, 1);
    bcm2835_i2c_read((char *)buffer, len);
}

float compute_average(float *buffer) {
    float sum = 0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += buffer[i];
    }
    return sum / FILTER_SIZE;
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

        // Check if the rotational velocities exceed the threshold and use last valid data if necessary
        if (fabs(yaw_rate) > GYRO_THRESHOLD) yaw_rate = last_valid_yaw_rate;
        else last_valid_yaw_rate = yaw_rate;

        if (fabs(pitch_rate) > GYRO_THRESHOLD) pitch_rate = last_valid_pitch_rate;
        else last_valid_pitch_rate = pitch_rate;

        if (fabs(roll_rate) > GYRO_THRESHOLD) roll_rate = last_valid_roll_rate;
        else last_valid_roll_rate = roll_rate;

        // Update gyroscope buffers
        gyro_yaw_buffer[buffer_index] = yaw_rate;
        gyro_pitch_buffer[buffer_index] = pitch_rate;
        gyro_roll_buffer[buffer_index] = roll_rate;

        // Read accelerometer data
        read_register(ACC_DATA_X_LSB, accel_buffer, 6);
        int16_t accel_x_raw = (int16_t)((accel_buffer[1] << 8) | accel_buffer[0]);
        int16_t accel_y_raw = (int16_t)((accel_buffer[3] << 8) | accel_buffer[2]);
        int16_t accel_z_raw = (int16_t)((accel_buffer[5] << 8) | accel_buffer[4]);

        // Scale down the raw values (move decimal place back 2)
        float accel_x = accel_x_raw / 100.0;
        float accel_y = accel_y_raw / 100.0;
        float accel_z = (accel_z_raw / 100.0) - GRAVITY;

        // Check if the accelerometer values exceed the threshold and use last valid data if necessary
        if (fabs(accel_x) > ACCEL_THRESHOLD) accel_x = last_valid_accel_x;
        else last_valid_accel_x = accel_x;

        if (fabs(accel_y) > ACCEL_THRESHOLD) accel_y = last_valid_accel_y;
        else last_valid_accel_y = accel_y;

        if (fabs(accel_z) > ACCEL_THRESHOLD) accel_z = last_valid_accel_z;
        else last_valid_accel_z = accel_z;

        // Update accelerometer buffers
        accel_x_buffer[buffer_index] = accel_x;
        accel_y_buffer[buffer_index] = accel_y;
        accel_z_buffer[buffer_index] = accel_z;

        // Increment buffer index
        buffer_index = (buffer_index + 1) % FILTER_SIZE;

        // Compute filtered values
        float filtered_yaw = compute_average(gyro_yaw_buffer);
        float filtered_pitch = compute_average(gyro_pitch_buffer);
        float filtered_roll = compute_average(gyro_roll_buffer);

        float filtered_accel_x = compute_average(accel_x_buffer);
        float filtered_accel_y = compute_average(accel_y_buffer);
        float filtered_accel_z = compute_average(accel_z_buffer);

        // Print filtered data
        printf("Filtered Gyroscope (rad/s): Yaw=%.2f, Pitch=%.2f, Roll=%.2f\n",
               filtered_yaw, filtered_pitch, filtered_roll);
        printf("Filtered Accelerometer (m/s²): X=%.2f, Y=%.2f, Z=%.2f\n\n",
               filtered_accel_x, filtered_accel_y, filtered_accel_z);

        usleep(100000); // Delay 100ms
    }

    bcm2835_i2c_end();
    bcm2835_close();
    return 0;
}
