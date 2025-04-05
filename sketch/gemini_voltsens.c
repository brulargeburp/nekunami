int dcLoad_sensorPin = A0; // DC Load sensor on A0
int dcArduino_sensorPin = A1; // DC Arduino sensor on A1 (replacing AC sensor)
int transistorPin = 2; // TransistorPin for PNP transistor
bool circuitTripped = false;

// Calibration values (replace with your actual calibration data)
float dcLoad_VoltageSlope = 0.02445;  // Example: slope from calibration for the load sensor
float dcLoad_VoltageIntercept = 0.0;  // Example: intercept from calibration for the load sensor

float dcArduino_VoltageSlope = 0.02445;  // Example: slope from calibration for the arduino sensor
float dcArduino_VoltageIntercept = 0.0;  // Example: intercept from calibration for the arduino sensor

// Voltage thresholds for automatic circuit closure with hysteresis
float autoCloseVoltageThresholdHigh = 5.0; // Upper threshold to open the circuit (turn OFF the transistor)
float autoCloseVoltageThresholdLow = 3.0;  // Lower threshold to close the circuit (turn ON the transistor)
// Hysteresis is the difference between these two values (0.5V in this case)

void setup() {
  Serial.begin(9600);
  pinMode(transistorPin, OUTPUT);
  digitalWrite(transistorPin, LOW); // Circuit is initially ON (transistor ON) - PNP logic
}

void loop() {
  // DC Load Voltage Measurement
  int dcLoad_sensorValue = analogRead(dcLoad_sensorPin);
  float dcLoad_voltage = (dcLoad_sensorValue * dcLoad_VoltageSlope) + dcLoad_VoltageIntercept;  // Calibrated voltage
  //Serial.print("Load Voltage: ");
  Serial.print(dcLoad_voltage);
  Serial.print("|");

  // DC Arduino Voltage Measurement
  int dcArduino_sensorValue = analogRead(dcArduino_sensorPin);
  float dcArduino_voltage = (dcArduino_sensorValue * dcArduino_VoltageSlope) + dcArduino_VoltageIntercept;
  //Serial.print("Arduino Voltage: ");
  Serial.println(dcArduino_voltage);

  // Automatic Circuit Closure Logic with Hysteresis (PNP Transistor Control)
  if (circuitTripped == false) { // If the circuit is not tripped (transistor is ON)
    if (dcLoad_voltage >= autoCloseVoltageThresholdHigh) {
      digitalWrite(transistorPin, HIGH); // Turn OFF the transistor (open the circuit)
      circuitTripped = true;      // Set tripped flag
      Serial.println("Circuit Auto-Opened (Voltage above high threshold)");
    }
  } else { // If the circuit is tripped (transistor is OFF)
    if (dcLoad_voltage <= autoCloseVoltageThresholdLow) {
      digitalWrite(transistorPin, LOW); // Turn ON the transistor (close the circuit)
      circuitTripped = false;      // Reset tripped flag
      Serial.println("Circuit Auto-Closed (Voltage below low threshold)");
    }
  }

  // Manual Trip/Reset (PNP Transistor Control)
  if (Serial.available() > 0) {
    char incomingCommand = Serial.read();

    if (incomingCommand == '0' && !circuitTripped) { // TRIP (!circuitTripped is same as circuitTripped == false)
      digitalWrite(transistorPin, HIGH); // Turn OFF the transistor (open the circuit)
      circuitTripped = true;      // Set the flag so it doesn't keep tripping
      Serial.println("Circuit Tripped Manually");
    } else if (incomingCommand == '1') { // RESET
      digitalWrite(transistorPin, LOW); // Turn ON the transistor (close the circuit)
      circuitTripped = false;      // reset tripped flag
      Serial.println("Circuit Reset Manually");
    } else {
      Serial.println("Invalid command.");
    }

    while (Serial.available() > 0) { // clear the input buffer
      Serial.read();
    }
  }
  delay(100);
}
