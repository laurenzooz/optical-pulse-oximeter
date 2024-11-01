#include <Arduino.h>
#include <arduinoFFT.h>
#include <BLEDevice.h>
#include <BLEService.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <LittleFS.h>

#define SAMPLES 64                 // Must be a power of 2
#define SAMPLING_FREQUENCY 1000    // Hz, must be less than 10000 due to ADC sampling speed
#define ANALOG_PIN A0              // Define the analog input pin for the sensor
#define FILE_PATH "/high_spikes_interval_50ms.log" 

unsigned int samplingPeriod_us;
unsigned long microseconds;

double vReal[SAMPLES];             // Array of real values
double vImag[SAMPLES];             // Array of imaginary values

// Initialize the FFT object without specifying the sampling frequency
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY, false);

// BLE definitions
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
float bpm = 0.0;

#define SERVICE_UUID           "adf2a6e6-9b6d-4b5f-a487-77e21aafbc88"  // UUID for the Heart Rate service
#define CHARACTERISTIC_UUID    "2A37"  // UUID for the Heart Rate Measurement characteristic

// Callback when a device connects or disconnects
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
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
    BLEDevice::init("OHRS"); // Set device name
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks()); // Set callback for connection events

    // Create a Heart Rate service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a Heart Rate Measurement characteristic
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

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

    // Set the sampling period for the analog readings
    samplingPeriod_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));

    
}

void loop() {
    /* Collect SAMPLES number of readings */
    File file = LittleFS.open(FILE_PATH, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        delay(1000);
        return;
    }
    for (int i = 0; i < SAMPLES; i++) {
        microseconds = micros(); // Overflows after around 70 minutes!

        vReal[i] = file.parseFloat();
        Serial.print("Read value: ");
        Serial.println(vReal[i]); // change to analogRead(ANALOG_PIN);  for pin
        vImag[i] = 0; 

        /* Wait for the next sample */
        while (micros() < (microseconds + samplingPeriod_us)) {}
    }

    /* Perform FFT on the samples */
    FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD); // Apply a window function to reduce spectral leakage
    FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD); // Compute the FFT
    FFT.complexToMagnitude(vReal, vImag, SAMPLES); // Compute magnitudes of the frequency bins

    /* Find the dominant frequency */
    double maxMagnitude = 0; // Reset maximum magnitude
    double dominantFrequency = 0; // Reset dominant frequency

    for (int i = 1; i < (SAMPLES / 2); i++) { // Start from 1 to skip the DC component
        double frequency = (i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES; // Calculate frequency of each bin
        if (vReal[i] > maxMagnitude) {
            maxMagnitude = vReal[i]; // Update maximum magnitude
            dominantFrequency = frequency; // Update dominant frequency
        }
    }

    /* Calculate and send BPM */
    bpm = dominantFrequency * 60; // Convert frequency to beats per minute
    Serial.print("Dominant Frequency: ");
    Serial.print(dominantFrequency);
    Serial.print(" Hz, Pulse: ");
    Serial.print(bpm);
    Serial.println(" BPM");

    // Update BLE characteristic with BPM if a device is connected
    if (deviceConnected) {
        uint8_t bpmData[2];
        bpmData[0] = 0x00; // Flags byte, set to 0x00 for 8-bit heart rate format
        bpmData[1] = (uint8_t)bpm; // Heart rate measurement as a single byte
        pCharacteristic->setValue(bpmData, 2); // Set characteristic value with flags + bpm
        pCharacteristic->notify(); // Notify connected client with BPM data
    }
    delay(1000); // Delay before next analysis
}