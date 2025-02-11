#include <stdint.h>

typedef struct {
    float longitude;
    float latitude;
    float headingAngle;
} GPS;

typedef struct {
    float ax;
    float ay;
    float az;
    float mx;
    float my;
    float mz;
} IMU;