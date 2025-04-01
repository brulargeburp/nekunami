int dcLoad_sensorPin = A0; // DC Load sensor on A0
int dcArduino_sensorPin = A1; // DC Arduino sensor on A1 (replacing AC sensor)
int mosfetPin = 2; // Changed from transistorPin to mosfetPin
bool circuitTripped = false;

// Calibration values (replace with your actual calibration data)
float dcLoad_VoltageSlope = 0.02445;  // Example: slope from calibration for the load sensor
float dcLoad_VoltageIntercept = 0.0;  // Example: intercept from calibration for the load sensor

float dcArduino_VoltageSlope = 0.02445;  // Example: slope from calibration for the arduino sensor
float dcArduino_VoltageIntercept = 0.0;  // Example: intercept from calibration for the arduino sensor

// Voltage thresholds for automatic circuit closure with hysteresis
float autoCloseVoltageThresholdHigh = 5.0; // Upper threshold to open the circuit (turn ON the MOSFET)
float autoCloseVoltageThresholdLow = 3.0;  // Lower threshold to close the circuit (turn OFF the MOSFET)
// Hysteresis is the difference between these two values (0.5V in this case)

void setup() {
  Serial.begin(9600);
  pinMode(mosfetPin, OUTPUT);
  digitalWrite(mosfetPin, HIGH); // Circuit is initially ON
}

void loop() {
  // DC Load Voltage Measurement
  int dcLoad_sensorValue = analogRead(dcLoad_sensorPin);
  float dcLoad_voltage = (dcLoad_sensorValue * dcLoad_VoltageSlope) + dcLoad_VoltageIntercept;  // Calibrated voltage
  Serial.print("Load Voltage: ");
  Serial.print(dcLoad_voltage);
  Serial.print("|");

  // DC Arduino Voltage Measurement
  int dcArduino_sensorValue = analogRead(dcArduino_sensorPin);
  float dcArduino_voltage = (dcArduino_sensorValue * dcArduino_VoltageSlope) + dcArduino_VoltageIntercept;
  Serial.print("Arduino Voltage: ");
  Serial.println(dcArduino_voltage);

  // Automatic Circuit Closure Logic with Hysteresis (MOSFET Control)
  if (circuitTripped == false) { // If the circuit is not tripped (MOSFET is OFF)
    if (dcLoad_voltage >= autoCloseVoltageThresholdHigh) {
      digitalWrite(mosfetPin, HIGH); // Turn ON the MOSFET (open the circuit)
      circuitTripped = true;      // Set tripped flag
      Serial.println("Circuit Auto-Closed (Voltage above high threshold)");
    }
  } else { // If the circuit is tripped (MOSFET is ON)
    if (dcLoad_voltage <= autoCloseVoltageThresholdLow) {
      digitalWrite(mosfetPin, LOW); // Turn OFF the MOSFET (close the circuit)
      circuitTripped = false;      // Reset tripped flag
      Serial.println("Circuit Auto-Opened (Voltage below low threshold)");
    }
  }

  // Manual Trip/Reset (MOSFET Control)
  if (Serial.available() > 0) {
    char incomingCommand = Serial.read();

    if (incomingCommand == '0' && !circuitTripped) { // TRIP (!circuitTripped is same as circuitTripped == false)
      digitalWrite(mosfetPin, HIGH); // Turn ON the MOSFET (close the circuit)
      circuitTripped = true;      // Set the flag so it doesn't keep tripping
      Serial.println("Circuit Tripped Manually");
    } else if (incomingCommand == '1') { // RESET
      digitalWrite(mosfetPin, LOW); // Turn OFF the MOSFET (open the circuit)
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
