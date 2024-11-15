void setup() {
  Serial.begin(115200);  // Match this to the baud rate in your Python script
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');  // Read data until newline
    int simulatedValue = data.toInt();           // Convert to integer
    Serial.println(simulatedValue);              // Use simulatedValue as if it were an analog reading
    delay(100);                                  // Add delay if necessary to control processing rate
  }
}
