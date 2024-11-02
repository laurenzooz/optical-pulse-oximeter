#include <Arduino.h>
#include <arduinoFFT.h>
#include <BLEDevice.h>
#include <BLEService.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <LittleFS.h>

#define ANALOG_PIN A0  // Define the analog input pin for the sensor
#define FILE_PATH "/high_spikes_interval_50ms.log"

const int sampleInterval = 50;                                    // Sampling interval in ms (20 Hz sampling rate)
const int intervalDuration = 5000;                                // Interval duration in ms (5 seconds)
const int pointsPerInterval = intervalDuration / sampleInterval;  // Number of points per interval

// bpm limits (converted to freq)
const float minFrequency = 40.0 / 60.0;
const float maxFrequency = 180.0 / 60.0;

arduinoFFT FFT = arduinoFFT();

// Arrays for FFT computation
double signal[pointsPerInterval];
double real[pointsPerInterval];
double imag[pointsPerInterval];

// Variables
float heartRates[32];
int heartRateIndex = 0;


// BLE definitions
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
float bpm = 0.0;

#define SERVICE_UUID "adf2a6e6-9b6d-4b5f-a487-77e21aafbc88"  // UUID for the Heart Rate service
#define CHARACTERISTIC_UUID "2A37"                           // UUID for the Heart Rate Measurement characteristic

// Callback when a device connects or disconnects
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

  Serial.begin(115200);
  if (!LittleFS.begin()) {
    Serial.println("An error occurred while mounting LittleFS.");
    return;
  }
  Serial.println("LittleFS mounted successfully.");

  // Initialize BLE
  BLEDevice::init("OHRS");  // Set device name
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


  File file = LittleFS.open(FILE_PATH, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    delay(1000);
    return;
  }

  Serial.println("Heart Rate Detection System Started.");
}

void loop() {

  // Read data for each 5-second interval
  int sampleIndex = 0;

  while (dataFile.available() && sampleIndex < pointsPerInterval) {
    String line = dataFile.readStringUntil('\n');
    signal[sampleIndex++] = line.toDouble();  // Convert each line to a double and store it
  }

  if (sampleIndex < pointsPerInterval) {
    Serial.println("End of file reached. Stopping analysis.");
    dataFile.close();
    while (true);  // Stop further processing
  }

  for (int i = 0; i < pointsPerInterval; i++) {
    real[i] = signal[i];  // Copy data to real part of the FFT input
    imag[i] = 0;          // Imaginary part is zero for real signals
  }

  /* Perform FFT on the samples */
  FFT.Windowing(real, pointsPerInterval, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(real, imag, pointsPerInterval, FFT_FORWARD);
  FFT.ComplexToMagnitude(real, imag, pointsPerInterval);

  float dominantFrequency = 0;
  float maxMagnitude = 0;
  for (int i = 1; i < pointsPerInterval / 2; i++) {                               // Only look at positive frequencies
    float frequency = i * (1.0 / (pointsPerInterval * sampleInterval / 1000.0));  // Frequency in Hz
    if (frequency > minFrequency && frequency < maxFrequency) {
      if (real[i] > maxMagnitude) {
        maxMagnitude = real[i];
        dominantFrequency = frequency;
      }
    }
  }

  float heartRateBPM = dominantFrequency * 60;      // Convert frequency to BPM
  if (heartRateBPM >= 40 && heartRateBPM <= 180) {  // Check if it's in the acceptable range
    heartRates[heartRateIndex++] = heartRateBPM;
    Serial.print("Interval ");
    Serial.print(heartRateIndex);
    Serial.print(": Estimated Heart Rate: ");
    Serial.print(heartRateBPM);
    Serial.println(" BPM");

    if (deviceConnected) {
      uint8_t bpmData[2];
      bpmData[0] = 0x00;                           // Flags byte, set to 0x00 for 8-bit heart rate format
      bpmData[1] = (uint8_t)bpm;                   // Heart rate measurement as a single byte
      pCharacteristic->setValue(heartRateBPM, 2);  // Set characteristic value with flags + bpm
      pCharacteristic->notify();                   // Notify connected client with BPM data
    }

  } else {
    Serial.print("Interval ");
    Serial.print(heartRateIndex + 1);
    Serial.println(": No valid heart rate detected");
    heartRates[heartRateIndex++] = 0;  // Placeholder for no valid heart rate
  }


  delay(1000);  // Delay before next analysis

  if (heartRateIndex >= 10) {
    Serial.println("Summary of Heart Rates for Each 5-Second Interval:");
    for (int i = 0; i < heartRateIndex; i++) {
      Serial.print("Interval ");
      Serial.print(i + 1);
      Serial.print(": ");
      if (heartRates[i] > 0) {
        Serial.print(heartRates[i]);
        Serial.println(" BPM");
      } else {
        Serial.println("No valid heart rate detected");
      }
    }
    heartRateIndex = 0;  // Reset for a new cycle
  }
}