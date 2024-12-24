import csv
import time
from datetime import datetime
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import smbus

# BNO055 I2C Address and Registers
BNO055_ADDRESS = 0x28
ACC_DATA_X_LSB = 0x08
GYR_DATA_Z_LSB = 0x14

# Conversion constants
GRAVITY = 9.81  # Earth's gravity
DEG_TO_RAD = 0.0174533
G_FORCE_CONVERSION = 1 / GRAVITY  # To convert m/s² to G-forces

# Initialize I2C bus
bus = smbus.SMBus(1)

# Initialize live plotting
plt.style.use("seaborn")
fig, ax = plt.subplots()
ax.set_title("Live G-Force Acceleration")
ax.set_xlabel("Time (s)")
ax.set_ylabel("G-Force")
ax.set_ylim(-5, 5)  # Adjust range for G-force
ax.set_xlim(0, 10)  # Show 10 seconds of data
line_x, = ax.plot([], [], label="X-axis")
line_y, = ax.plot([], [], label="Y-axis")
line_z, = ax.plot([], [], label="Z-axis")
ax.legend()

# Data buffers for live graph
time_buffer = []
accel_x_buffer = []
accel_y_buffer = []
accel_z_buffer = []

# CSV Setup
csv_filename = "sensor_data.csv"
csv_columns = ["Timestamp", "Accel_X (m/s²)", "Accel_Y (m/s²)", "Accel_Z (m/s²)",
               "Yaw (rad/s)", "Pitch (rad/s)", "Roll (rad/s)"]

# Write CSV Header
with open(csv_filename, mode="w", newline="") as csv_file:
    writer = csv.DictWriter(csv_file, fieldnames=csv_columns)
    writer.writeheader()

# Global time counter
start_time = time.time()
elapsed_time = 0  # Variable to keep track of elapsed time

def read_register(address, length):
    """Reads data from a specific register."""
    return bus.read_i2c_block_data(BNO055_ADDRESS, address, length)

def get_sensor_data():
    """Fetches accelerometer and gyroscope data."""
    # Read accelerometer
    accel_data = read_register(ACC_DATA_X_LSB, 6)
    accel_x_raw = (accel_data[1] << 8) | accel_data[0]
    accel_y_raw = (accel_data[3] << 8) | accel_data[2]
    accel_z_raw = (accel_data[5] << 8) | accel_data[4]
    accel_x = accel_x_raw / 100.0
    accel_y = accel_y_raw / 100.0
    accel_z = (accel_z_raw / 100.0) - GRAVITY

    # Read gyroscope
    gyro_data = read_register(GYR_DATA_Z_LSB, 6)
    yaw_rate_raw = (gyro_data[1] << 8) | gyro_data[0]
    pitch_rate_raw = (gyro_data[3] << 8) | gyro_data[2]
    roll_rate_raw = (gyro_data[5] << 8) | gyro_data[4]
    yaw_rate = yaw_rate_raw * DEG_TO_RAD
    pitch_rate = pitch_rate_raw * DEG_TO_RAD
    roll_rate = roll_rate_raw * DEG_TO_RAD

    return accel_x, accel_y, accel_z, yaw_rate, pitch_rate, roll_rate

def update_graph(frame):
    """Updates the live plot."""
    global time_buffer, accel_x_buffer, accel_y_buffer, accel_z_buffer, elapsed_time

    # Get sensor data
    accel_x, accel_y, accel_z, yaw_rate, pitch_rate, roll_rate = get_sensor_data()

    # Convert acceleration to G-forces
    accel_x_g = accel_x * G_FORCE_CONVERSION
    accel_y_g = accel_y * G_FORCE_CONVERSION
    accel_z_g = accel_z * G_FORCE_CONVERSION

    # Update graph buffers
    elapsed_time = time.time() - start_time  # Update elapsed time
    time_buffer.append(elapsed_time)
    accel_x_buffer.append(accel_x_g)
    accel_y_buffer.append(accel_y_g)
    accel_z_buffer.append(accel_z_g)

    # Keep graph within a 10-second window
    if len(time_buffer) > 100:
        time_buffer = time_buffer[-100:]
        accel_x_buffer = accel_x_buffer[-100:]
        accel_y_buffer = accel_y_buffer[-100:]
        accel_z_buffer = accel_z_buffer[-100:]

    # Update graph data
    line_x.set_data(time_buffer, accel_x_buffer)
    line_y.set_data(time_buffer, accel_y_buffer)
    line_z.set_data(time_buffer, accel_z_buffer)
    ax.set_xlim(time_buffer[0], time_buffer[-1])
    return line_x, line_y, line_z

def log_data_to_csv():
    """Logs data to the CSV file."""
    while True:
        # Get sensor data
        accel_x, accel_y, accel_z, yaw_rate, pitch_rate, roll_rate = get_sensor_data()

        # Log data with elapsed time as the timestamp
        with open(csv_filename, mode="a", newline="") as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=csv_columns)
            writer.writerow({
                "Timestamp": elapsed_time,  # Using elapsed_time as the timestamp
                "Accel_X (m/s²)": accel_x,
                "Accel_Y (m/s²)": accel_y,
                "Accel_Z (m/s²)": accel_z,
                "Yaw (rad/s)": yaw_rate,
                "Pitch (rad/s)": pitch_rate,
                "Roll (rad/s)": roll_rate
            })
        time.sleep(0.1)  # Log every 100ms

# Start CSV logging in a separate thread
import threading
threading.Thread(target=log_data_to_csv, daemon=True).start()

# Start live plotting
ani = FuncAnimation(fig, update_graph, interval=100)
plt.show()
