# Vehicle Electronic Control Unit

## **Goal**
The goal of this project is to create an ECU that works adjacently with the main engine computer, providing more I/Os and customized algorithms and dynamic estimations of the vehicle. This will also have GPS data and serve as a lap timer + lap data collector, providing important metrics of the vehicle's behavior to determine performance during track days.

## **Implementation**
There a few different categories of the implementation:
### **Signal Processing**
This project utilizes an STM microcontroller that directly gets the following sensor data:
- **3-Axis Acceleration**: From a BNO-055 IMU communicating over SPI, the STM microcontroller gets acceleration in all three axes. The accelerometer is placed near the CG of the vehicle for the best measurements of the vehicle's dynamics.
- **3-Axis Rotational Velocity**: From the same IMU, we also obtain rotational velocity around each of the three axes, giving us yaw rate, pitch rate, and roll rate.
- **GPS Data**: Using a GPS unit from AdaFruit, we get Latitude, Longitude, and Altitude on our STM microcontroller.
- **Wheel Speeds**: We can directly tap into the vehicle's ABS wheel speed sensors as well as differential wheel speed sensor to get wheel speed readings on the microcontroller.
- **Exhaust Gas Temperature**: Using Type-K thermocouples, I decided the best way to understand the health and performance of each cylinder would be through individual EGTs to measure the temperature after combustion in the exhaust. This can help us adjust fuel trims to compensate for a cylinder that runs leaner or richer, preserving the life of our engine. Measured in degrees Celsius.
- **Steering Angle**: Having the steering angle allows us to calculate various other parameters of the car, measured in degrees.
- **Tire Pressure + Temperature**: Having both tire pressure and temperature can tell us when we are ready to drive the car to the limit, or tell us when our tires are too hot and its time for a cooldown lap.

### **CAN Message RX**
From the engine's computer, we will receive important information about the engine that will be used in algorithms and other calculations on our microcontroller:
- **Engine Speed**: The speed that the crankshaft is rotating at, in revolutions per minute (RPM)
- **Lambda AFR**: The air fuel ratio measured in the exhaust.
- **Oil Pressure**: The engine's oil pressure, helping us understand if we are experiencing oil starvation at certain g-forces and to validate the performance of our oil pan baffles, measured in PSI
- **Manifold Air Pressure**: The engine's intake manifold pressure, important for us to understand the amount of boost the engine is undergoing at a given point in time, allowing us to make setup adjustments for faster spool, more top end, etc. in PSI
- **Fuel Pressure**: The fuel pressure at the fuel rail/pressure regulator, similar to oil pressure, helping us understand where we experience fuel starvation (what lateral/longitudinal accel), in PSI
- **Ethanol Content**: Since this engine will be on a flex fuel setup, having an ethanol content sensor on our display will be important to understand what boost level we will be operating at.
- **Throttle Position**: On the track, knowing when we are on/off throttle can help us optimize braking zones and when to get back on throttle.
- **Coolant Temperature**: This is used for radiator fan control algorithms, to optimize our cooling efficiency, allowing the car to warm up/cool down in the most efficient manner, in degrees Fahrenheit.
- **Intake Air Temperature**: Intake air temps will allow us to know when we are heatsoaking the engine, feeding it hot air, resulting in inefficient engine performance, in degrees Fahrenheit.
- **Battery Voltage**: Knowing the battery voltage of the entire car can put us in a backup/reduced power state, since we have the risk of an electronic failure + reduced fuel, ignition, and fan power.
- **Engine Run Time**: Engine run time is used to add a startup timer to give the microcontroller and ECU some time to initialize their values before we begin calculations.

### **Vehicle Dynamics Estimation**:
Utilizing these collected engine and vehicle parameters, we can calculate other parameters using physics and dynamics. These can be used for data analysis or for control algorithms to alter the vehicle's performance:
- **Grade**: From the IMU and GPS data, we can calculate the grade the vehicle is at on the track, and have this data when analyzing logs from the track.
- **Wheel Forces**: Understanding the forces on each tire during turns, braking, and acceleration gives us insight into the grip level that is attainable under various circumstances, and can help us make adjustments to optimize the suspension and tire setup.
- **Vehicle + Wheel Side Slip**: Getting the side slip at the CG as well as for each wheel can help us determine when we are reaching our slip limit, and warn us.
- **Driver Intended Yaw Rate**: From the steering angle, we can get the driver's intended yaw rate, an important metric in determining oversteer/understeer.
- **Oversteer/Understeer Gradient**: With driver intended yaw rate and actual yaw rate, we can determine the understeer/oversteer gradient the car experiences.

### **CAN Message TX**
The engine's computer can't take in all of these parameters we have estimated, but since we didn't have enough I/O's to add EGTs and wheel speeds, transmitting our measured/calculated data over CAN is the next best option.
- **EGT per Cylinder**: We can send our measured thermocouple data over CAN so that the engine's computer can make adjustments to the fuel trims to keep the engine running healthy.
- **Wheel Speeds**: To have basic traction control (ignition retard based on a slip target), we can feed in our measured wheel speeds to the engine's computer over CAN.

### **Digital Dash Display**
With all of this data on the microcontroller, it would be amazing if we could see the vital data on a screen, along with other features including logging and lap timing.

#### Features
- **Vital Engine Parameters**: Since most stock clusters only display a needle with coolant temp, oil pressure, engine RPM, vehicle speed, and other data, it is nice to have a digital display for this instead.
- **Track Map**: The UI can display the current track that we are at, using GPS data to determine lap times.
- **Logging**: With a simple press of a button, the UI allows us to start, stop, and save logs. The logs will then be exported to MDFs, which can then be viewed with programs such as CANape, or parsed in Python for custom data analysis.
