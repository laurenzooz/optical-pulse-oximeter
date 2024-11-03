import serial
import time

# Open the serial port. Replace 'COM3' with your ESP32's port and set the correct baud rate.
ser = serial.Serial('/dev/ttyACM3', 115200)
time.sleep(2)  # Give some time to establish connection

# Open the text file and send each line over serial
while (True):
	ser.write(1200)  # Send value with newline
	time.sleep(0.05)

ser.close()
