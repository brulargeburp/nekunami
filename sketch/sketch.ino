int dc_sensorPin = A0; // Analog pin connected to the voltage sensor
int ac_sensorPin = A1;
int relayPin = 7;   // Digital pin connected to the relay (simulating circuit breaker)
float voltageThreshold = 4.5; // Voltage threshold to trip the circuit (adjust as needed)
bool circuitTripped = false; // Flag to indicate if the circuit has been tripped

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Initialize relay to be normally closed (circuit on)
}

void loop() {
  // DC sensor
  int dc_sensorValue = analogRead(dc_sensorPin);
  float dc_voltage = (dc_sensorValue * 5.0) / 1023.0; // Calculate voltage based on 5V Arduino reference

  Serial.print("Measured Voltage: ");
  Serial.println(dc_voltage);

  // AC sensor
  int ac_sensorValue = analogRead(ac_sensorPin);
  float ac_voltage = (ac_sensorValue * 5.0) / 1023.0; // Calculate voltage based on 5V Arduino reference

  Serial.print("Measured Voltage: ");
  Serial.println(ac_voltage);

  //SSR Circuit Breaker Logic
  if (voltage > voltageThreshold && !circuitTripped) {
    Serial.println("Overvoltage detected! Tripping circuit.");
    digitalWrite(relayPin, LOW); // Open the relay (trip the circuit)
    circuitTripped = true;      // Set the flag so it doesn't keep tripping
    Serial.println("Circuit tripped.  Requires reset (reboot Arduino).");
  }
  else if (voltage <= voltageThreshold && circuitTripped)
  {
    Serial.println("Voltage normalized! Circuit would still require reset.");
  }

    delay(100); // Reduced Delay
}
