#include <arduinoFFT.h>

#define SAMPLES 64             // Must be a power of 2
#define SAMPLING_FREQUENCY 1000 // Hz, must be less than 10000 due to ADC sampling speed

arduinoFFT FFT = arduinoFFT();
unsigned int samplingPeriod_us;
unsigned long microseconds;

double vReal[SAMPLES];         // Array of real values
double vImag[SAMPLES];         // Array of imaginary values

void setup() {
  Serial.begin(115200);
  samplingPeriod_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}

void loop() {
  /* Collect SAMPLES number of analog readings */
  for (int i = 0; i < SAMPLES; i++) {
    microseconds = micros();    // Overflows after around 70 minutes!

    vReal[i] = analogRead(A0);  // Read the signal from analog pin 0
    vImag[i] = 0;               // Set imaginary part to 0

    /* Wait for the next sample */
    while (micros() < (microseconds + samplingPeriod_us)) {}
  }

  /* Perform FFT on the samples */
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  // Apply a window function to reduce spectral leakage
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);                  // Compute the FFT
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);                    // Compute magnitudes of the frequency bins

   /* Find the dominant frequency */
  maxMagnitude = 0;                // Reset maximum magnitude
  dominantFrequency = 0;           // Reset dominant frequency

  for (int i = 1; i < (SAMPLES / 2); i++) {  // Start from 1 to skip the DC component
    double frequency = (i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES; // Calculate frequency of each bin
    if (vReal[i] > maxMagnitude) {
      maxMagnitude = vReal[i];     // Update maximum magnitude
      dominantFrequency = frequency; // Update dominant frequency
    }
  }

  /* Print the dominant frequency and magnitude */
  Serial.print("Dominant Frequency: ");
  Serial.print(dominantFrequency);
  Serial.print(" Hz, So the pulse is: ");
  Serial.println(dominantFrequency * 60);

  delay(1000);  // Delay before next analysis
}
