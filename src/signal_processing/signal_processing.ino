#include <Arduino.h>
#include <arduinoFFT.h>
#include <BLEDevice.h>
#include <BLEService.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int points_per_second = 20;  // Sampling rate is 20 samples per second (1 / 0.05)
//const int interval_duration = 5;   // Interval duration in seconds
const int points_per_interval = 64;

// Heart rate limits (in frequency, ~40 to 180 bpm)
const float min_freq = 0.65;
const float max_freq = 3;

float signal_data[points_per_interval];  // Renamed to avoid conflict with C standard library `signal`
int num_samples = 0;                     // Track the number of samples received
float heart_rates[8];                    // To store heart rates for each interval
int interval_count = 0;

double vReal[points_per_interval];
double vImag[points_per_interval]; 

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
    //Serial.println(value);
    signal_data[num_samples++] = value;  // Updated to `signal_data` to avoid conflict

    // Check if we've received enough samples for one interval
    if (num_samples >= points_per_interval) {
      //Serial.println("Gonna calculate the pulse");
      processInterval();
      num_samples = 0;  // Reset sample count for the next interval
    }
  }
  delay(50);  // Delay before next analysis
}

void processInterval() {
  //Serial.println("Starting the first loop");

  
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);	/* Weigh data */
  FFT.compute(FFTDirection::Forward); /* Compute FFT */
  FFT.complexToMagnitude(); /* Compute magnitudes */
  
  float peak = FFT.majorPeak();
  
  /*
  Serial.println("Starting the loop");

  for (int i = 1; i < points_per_interval / 2; i++) {
    float frequency = i * (points_per_second / (float)points_per_interval);
    float magnitude = sqrt(vReal[i * 2] * vReal[i * 2] + vReal[i * 2 + 1] * vReal[i * 2 + 1]);

    if (frequency >= min_freq && frequency <= max_freq && magnitude > max_magnitude) {
      max_magnitude = magnitude;
      dominant_frequency = frequency;
      Serial.print("new dominant freq ");
      Serial.println(dominant_frequency);
    }
  }

*/
  // Calculate and send BPM
  float bpm = peak * 60;  // Convert frequency to beats per minute
  Serial.print("Dominant Frequency: ");
  Serial.print(dominant_frequency);
  Serial.print(" Hz, Pulse: ");
  Serial.print(bpm);
  Serial.println(" BPM");

  uint8_t bpmData[2];
  bpmData[0] = 0x00;                      // Flags byte, set to 0x00 for 8-bit heart rate format
  bpmData[1] = (uint8_t)bpm;              // Heart rate measurement as a single byte
  pCharacteristic->setValue(bpmData, 2);  // Set characteristic value with flags + bpm
  pCharacteristic->notify();              // Notify connected client with BPM data
}
