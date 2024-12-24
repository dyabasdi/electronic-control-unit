#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* Structures */

typedef enum{
    INITIALIZING,
    STARTUP,
    FAULTED,
    READY
} States;

typedef struct{
    int32_t accel_x;
    int32_t accel_y;
    int32_t accel_z;
    int32_t yaw_rate;
    int32_t roll_rate;
    int32_t pitch_rate;
} inertial;

typedef struct{
    int32_t latitude;
    int32_t longitude;
    int32_t altitude;
    int32_t velocity;
} GPS;

typedef struct {
    uint32_t id;
    uint8_t data[8];
} CAN;

typedef struct {
    uint8_t masterCylPressure;
    bool brakesApplied;
} Brakes;

typedef enum{
    STATIONARY,
    FORWARD,
    REVERSE,
    UNDEFINED
} Direction;

typedef struct {
    uint32_t wheelSpeedFrL;
    uint32_t wheelSpeedFrR;
    uint32_t wheelSpeedReL;
    uint32_t wheelSpeedReR;
    uint32_t drivingWheelSpeed; // avg of rear 2 wheels
    uint32_t drivenWheelSpeed; // avg of front 2 wheels
    Direction wheelDirectionFrL;
    Direction wheelDirectionFrR;
    Direction wheelDirectionReL;
    Direction wheelDirectionReR;
    bool standStill;
} Speeds;

typedef struct{
    uint16_t tempCyl1;
    uint16_t tempCyl2;
    uint16_t tempCyl3;
    uint16_t tempCyl4;
    uint16_t tempCyl5;
    uint16_t tempCyl6;
} EGT;

typedef struct {
    States prevState;
    States currState;
    inertial imu_data;
    GPS gps_data;
    CAN frame1;
    CAN frame2;
    CAN frame3;
    Brakes brakes;
    Speeds speeds;
    bool clutchPressed;
    EGT exhaustGasTemp;
} Signals;

// This will interface with the rest of the code
extern Signals S_signal;