#include <dynamics.h>
#include <process.h>
#include <stdlib.h>

/* Generates state estimations of the vehicle's dynamics based on sensor data and vehicle parameters*/

#define VEHICLE_MASS 1300       // Vehicle mass in kg
#define CG_HEIGHT 0.5           // Height of the center of gravity in meters
#define TRACK_WIDTH 1.413       // Track width in meters
#define WHEELBASE 2.7           // Wheelbase in meters
#define GRAVITY 9.81            // Acceleration due to gravity in m/s^2
#define W_FRONT 0.55            // percentage of weight over front axle
#define W_REAR 0.45             // percentage of weight over rear axle
#define MAX_RPM 7300            // max engine speed in RPM
#define FINAL_DRIVE 3.15        // differential gear ratio
#define GEAR_RATIOS {4.21, 2.49, 1.67, 1.24, 1.0} // transmission gear ratios
#define TIRE_DIAMETER 0.6518    // tire diameter in meters
#define RPM_BUFFER 0.10         // percent difference that RPM can be and still be in that gear

Estimations initializeDynamics(Estimations *temp_estimations){

    // init weights
    temp_estimations->dynamics.mass.staticWeight.vehicleWeight = VEHICLE_MASS * GRAVITY;
    temp_estimations->dynamics.mass.staticWeight.front = W_FRONT * temp_estimations->dynamics.mass.staticWeight.vehicleWeight;
    temp_estimations->dynamics.mass.staticWeight.rear = W_REAR * temp_estimations->dynamics.mass.staticWeight.vehicleWeight;
    
    // init load transfer
    temp_estimations->dynamics.mass.loadTransfer.loadFrL = temp_estimations->dynamics.mass.staticWeight.front / 2;
    temp_estimations->dynamics.mass.loadTransfer.loadFrR = temp_estimations->dynamics.mass.staticWeight.front / 2;
    temp_estimations->dynamics.mass.loadTransfer.loadReL = temp_estimations->dynamics.mass.staticWeight.rear / 2;
    temp_estimations->dynamics.mass.loadTransfer.loadReR = temp_estimations->dynamics.mass.staticWeight.rear / 2;

    // init brake apply boolean
    temp_estimations->brakesApplied = false;

    // init gear ratios
    temp_estimations->gearing.gear = NEUTRAL;
    temp_estimations->gearing.finalDrive = FINAL_DRIVE;
    
    uint16_t temp_gear_ratios[] = GEAR_RATIOS;

    for (int i = 0; i < NUM_OF_GEARS; i++){
        temp_estimations->gearing.gearRatios[i] = temp_gear_ratios[i];
    }
    
    return *temp_estimations;
}

Load calculate_load_transfer(inertial *data, Mass *mass_temp) {
    
    const uint16_t longLoad = (VEHICLE_MASS * data->accel_x * CG_HEIGHT) / TRACK_WIDTH;
    const uint16_t latLoad = (VEHICLE_MASS * data->accel_y * CG_HEIGHT) / TRACK_WIDTH;
    
    mass_temp->loadTransfer.loadFrL = mass_temp->staticWeight.front + longLoad - latLoad;
    mass_temp->loadTransfer.loadFrR = mass_temp->staticWeight.front + longLoad + latLoad;
    mass_temp->loadTransfer.loadReL = mass_temp->staticWeight.rear - longLoad - latLoad;
    mass_temp->loadTransfer.loadReR = mass_temp->staticWeight.rear - longLoad + latLoad;

    return mass_temp->loadTransfer;
}

Gear calculateGear(Gearing *gearing_temp){
    // calculates the expected engine RPM for each gear based on vehicle speed
    // if that gear's expected engine RPM is within 10% if the engine RPM, return gear
    
    const uint16_t actualEngineRPM = S_signal.frame1.data[1] + S_signal.frame1.data[2]; // do this correctly as these are just hexidecimal values
    const uint32_t rotationalWheelSpeed = (S_signal.speeds.drivingWheelSpeed * 1000) / (3.6 * 3.14159 * TIRE_DIAMETER);
    Gear currentGear = NEUTRAL;

    // iterate through gears and check if our expected RPM is within range of actual
    for (int i = 0; i < NUM_OF_GEARS; i++){
        const bool inRange = (abs(actualEngineRPM - (S_signal.speeds.drivingWheelSpeed * gearing_temp->gearRatios[i] * gearing_temp->finalDrive)) < (RPM_BUFFER*actualEngineRPM));
        if (inRange){
            currentGear = i + 1; //mapped to enums (1 = First Gear, 2 = Second Gear, etc.)
        }
    }
    return currentGear; // this will be neutral if it didn't fit into any of the estimated gears
}

void updateGlobals(Estimations *temp_estimations){
    // disable interrupts from other modules

    S_estimations = *temp_estimations;

    // resume interrupts from other modules
}


void updateDynamics(){
    Estimations s_estimations;
    s_estimations = initializeDynamics(&s_estimations);
    s_estimations.dynamics.mass.loadTransfer = calculate_load_transfer(&S_signal.imu_data, &s_estimations.dynamics.mass);
    s_estimations.gearing.gear = calculateGear(&s_estimations.gearing);

    updateGlobals(&s_estimations);
}