#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEService.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <PeakDetection.h> 

PeakDetection peakDetection; 

// BLE definitions
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

#define SERVICE_UUID "adf2a6e6-9b6d-4b5f-a487-77e21aafbc88"  // UUID for the Heart Rate service
#define CHARACTERISTIC_UUID "00002a37-0000-1000-8000-00805f9b34fb"                           // UUID for the Heart Rate Measurement characteristic

// Callback for device connection events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("Optical Pulse Oximeter");  // Set device name
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());  // Set callback for connection events

  // Create a Heart Rate service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a Heart Rate Measurement characteristic
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Set connection parameters for BLE
  pAdvertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE Heart Rate Monitor is now advertising...");

  //pinMode(1, INPUT);                            // analog pin used to connect the sensor
  //peakDetection.begin(48, 3, 0.6); // defaults
  peakDetection.begin(128, 2, 0.7);  // test different values (lag, threshold, influence)
}

unsigned long bpm = 0; // To store the sum of the intervals for averaging
unsigned long previousPeakTime = 0;  // Variable to store the time of the previous peak
unsigned long currentPeakTime = 0;   // Variable to store the time of the current peak
unsigned long peakInterval = 0;      // Interval between peaks

const int numIntervals = 5;  // Number of intervals to average
unsigned long intervals[numIntervals];  // Array to store the last 5 intervals (in bpm)
unsigned long totalBPM = 0;  // Sum of the last 5 bpm values
int bpmIndex = 0;  // Index for storing the bpm values in the array
unsigned long avgBPM = 0;  // Calculated average of the last 5 bpm values

void loop() {

  if (Serial.available() > 0) {
    // Read the incoming data
    float value = Serial.parseFloat(); // read from the python data sent

    //double data = (double)analogRead(1) / 512 - 1;  // Converts the sensor value to a range between -1 and 1. Seems to work without converting the range?
    peakDetection.add(value);                     // Adds a new data point
    int peak = peakDetection.getPeak();          // 0, 1, or -1 (detects peaks)
    double filtered = peakDetection.getFilt();   // Moving average filter
    
    // send data over bluetooth
    uint8_t bpmData[3];
    bpmData[0] = 0x00;                      // Flags byte, set to 0x00 for 8-bit heart rate format
    bpmData[1] = (uint8_t)value;       // Heart rate measurement as a single byte
      

    // When a peak is detected (value 1), calculate the time interval between peaks
    if (peak == 1) {
        currentPeakTime = millis();  // Get the current time when the peak occurs

        // If this is not the first peak, calculate the time interval between peaks
        if (previousPeakTime != 0) {
            peakInterval = currentPeakTime - previousPeakTime;  // Calculate the interval
            if (peakInterval > 300 && peakInterval < 1000){
              
              bpm = 1/(peakInterval*0.001) * 60; // calculate bpm and print
              Serial.print("Heart rate: ");
              Serial.print(bpm);
              Serial.println(" bpm");

              // Update the total BPM sum and store the current BPM in the array
              totalBPM -= intervals[bpmIndex];  // Remove the oldest BPM value from totalBPM
              intervals[bpmIndex] = bpm;  // Add the new BPM value to the array
              totalBPM += bpm;  // Add the new BPM value to the total sum

              // Move to the next index (circular buffer)
              bpmIndex = (bpmIndex + 1) % numIntervals;

              // Calculate the average BPM of the last 5 values (if all values are set, aka if last value is set)
              if (intervals[numIntervals - 1] != 0) {
                avgBPM = totalBPM / numIntervals;
                Serial.print("Average Heart rate (5 readings): ");
                Serial.print(avgBPM);  // Print the average heart rate
                Serial.println(" bpm");
              }           
            }
        }
        // Update the previous peak time to the current one
        previousPeakTime = currentPeakTime;


    }

    bpmData[2] = (uint8_t)avgBPM;       // Heart rate measurement as a single byte

    pCharacteristic->setValue(bpmData, 3);  // Set characteristic value with flags + bpm
    pCharacteristic->notify();              // Notify connected client with BPM data
  }
  delay(10);  // Optional delay for stability, adjust as needed
}