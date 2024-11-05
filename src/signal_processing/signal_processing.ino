#include <Arduino.h>
#include <arduinoFFT.h>
#include <BLEDevice.h>
#include <BLEService.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//const int points_per_second = 20;  // Sampling rate is 20 samples per second (1 / 0.05)
//const int interval_duration = 5;   // Interval duration in seconds
const int points_per_interval = 128;

// Heart rate limits (in frequency, 40 to 180 bpm)
const float min_freq = 0.8;
const float max_freq = 3.0;

int num_samples = 0;   // Track the number of samples received
int interval_count = 0;
int num_bpms = 0; // amount of already measured and calculated bpm values

double vReal[points_per_interval];
double vImag[points_per_interval];

float bpm_values[3]; // store 3 values, get the median

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, points_per_interval, 20, false);

// BLE definitions
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

#define SERVICE_UUID "adf2a6e6-9b6d-4b5f-a487-77e21aafbc88"  // UUID for the Heart Rate service
#define CHARACTERISTIC_UUID "2A37"                           // UUID for the Heart Rate Measurement characteristic

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
}

void loop() {
  if (Serial.available() > 0) {
    // Read the incoming data
    float value = Serial.parseFloat();

    vReal[num_samples++] = value;  // save the real values


    /*
    // Convert the incoming value to an 8-bit integer format for BLE transmission
    uint8_t valueData[2];
    valueData[0] = 0x01;                 // Custom flag byte for raw value notification
    valueData[1] = (uint8_t)(constrain(value, 0, 255));       // Raw value as a single byte
    pCharacteristic->setValue(valueData, 2);  // Set characteristic value with flag + raw value
    pCharacteristic->notify();                // Notify connected client with the raw data
    */

    // Check if we've received enough samples for one interval
    if (num_samples >= points_per_interval) {
      processInterval();
      num_samples = 0;  // Reset sample count for the next interval
    }

    // if three bpm values are measured, get the median
    if (bpm_values[2] != 0) {
      // shift newest values to left, empty the last. Don't start over, just constantly keep last three values saved(so minus one from current count.)
      num_bpms--;
      sendData(); // Send the data to bluetooth and serial.
    }
  }
  delay(50);  // Delay before next analysis
}

void processInterval() {



  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); /* Weigh data */
  FFT.compute(FFTDirection::Forward);                       /* Compute FFT */
  FFT.complexToMagnitude();                                 /* Compute magnitudes */

  float max_magnitude = 0.0;
  float dominant_frequency = 0.0;


  // find higest magnitude between the desired range
  for (int i = 1; i < (points_per_interval / 2); i++) {   
    double frequency = (i * 20.0) / points_per_interval;  // Calculate frequency of each bin
    
    // Check if the frequency is within heart rate limits
    if (frequency >= min_freq && frequency <= max_freq) {
      if (vReal[i] > max_magnitude) {
        max_magnitude = vReal[i];        // Update maximum magnitude
        dominant_frequency = frequency;  // Update dominant frequency
      }
    }
  }

//  float peak = FFT.majorPeak();

  // Calculate and send BPM
  float bpm = dominant_frequency * 60;  // Convert frequency to beats per minute

  bpm_values[num_bpms++] = bpm; // save the measured bpm

  Serial.print("Last bpm: ");
  Serial.println(bpm);

  //Serial.print("Dominant Frequency: ");
  //Serial.print(dominant_frequency);
  //Serial.print(" Hz, Pulse: ");

}

// find the median from three values
float medianOfThree(float a, float b, float c) {
    if ((a <= b && b <= c) || (c <= b && b <= a)) {
        return b;  // b is the median
    } else if ((b <= a && a <= c) || (c <= a && a <= b)) {
        return a;  // a is the median
    } else {
        return c;  // c is the median
    }
}


void sendData() {

  
  float median_bpm = medianOfThree(bpm_values[0], bpm_values[1], bpm_values[2]);
  


  Serial.print("Median");
  Seria.println(median_bpm);
  uint8_t bpmData[2];
  bpmData[0] = 0x00;                      // Flags byte, set to 0x00 for 8-bit heart rate format
  bpmData[1] = (uint8_t)median_bpm;              // Heart rate measurement as a single byte
  pCharacteristic->setValue(bpmData, 2);  // Set characteristic value with flags + bpm
  pCharacteristic->notify();              // Notify connected client with BPM data
}

