/* Header for dynamics.c */
#include <stdio.h>
#include <stdint.h>
#include <string.h> // For memcpy
#include <stdbool.h>
#define NUM_OF_GEARS 5

typedef enum{
    NEUTRAL,
    FIRST,
    SECOND,
    THIRD,
    FOURTH,
    FIFTH,
    SIXTH,
    REVERSE
} Gear;

typedef struct{
    Gear gear;
    uint16_t gearRatios[NUM_OF_GEARS];
    uint16_t finalDrive;
} Gearing;
typedef struct{
    uint16_t vehicleWeight;
    uint16_t front;
    uint16_t rear;
} Static_W;

typedef struct{
    uint16_t loadFrL;
    uint16_t loadFrR;
    uint16_t loadReL;
    uint16_t loadReR;
} Load;

typedef struct{
    Static_W staticWeight;
    Load loadTransfer;
} Mass;

typedef struct{
    int8_t gradeEstimation;
    Mass mass;
} Dynamics;

typedef struct{
    Gearing gearing;
    Dynamics dynamics;
    bool brakesApplied;
} Estimations;

extern Estimations S_estimations;