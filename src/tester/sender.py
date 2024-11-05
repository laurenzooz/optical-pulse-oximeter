import serial
import time

# Open the serial port. Replace '/dev/ttyACM0' with your ESP32's port and set the correct baud rate.
ser = serial.Serial('/dev/ttyACM4', 115200)
time.sleep(15)  # Give some time to establish connection

while True:  # Infinite loop to keep reading the file
    with open('data.txt', 'r') as file:
        for line in file:
            value = line.strip()  # Get the data value and remove any whitespace
            if value:  # Make sure the line isn't empty
                ser.write((value + '\n').encode())  # Send value with newline
                time.sleep(0.05)  # Small delay to prevent buffer overflow
            else:
                print("Skipped an empty line.")
    print("Reached end of file, restarting...")

ser.close()
