#include <SoftwareSerial.h>

int dcLoad_sensorPin = A0; // DC Load sensor on A0
int dcArduino_sensorPin = A1; // DC Arduino sensor on A1 (replacing AC sensor)
int relayPin = 2;
bool circuitTripped = false;

// Calibration values (replace with your actual calibration data)
float dcLoad_VoltageSlope = 0.02445;  // Example: slope from calibration for the load sensor
float dcLoad_VoltageIntercept = 0.0;  // Example: intercept from calibration for the load sensor

float dcArduino_VoltageSlope = 0.02445;  // Example: slope from calibration for the arduino sensor
float dcArduino_VoltageIntercept = 0.0;  // Example: intercept from calibration for the arduino sensor

// bluetooth serial
SoftwareSerial BTSerial(0, 1);

// Voltage threshold for automatic circuit closure
float autoCloseVoltageThreshold = 3.5; // Set the threshold to 3.5V

void setup() {
  BTSerial.begin(9600);
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Circuit is initially ON
}

void loop() {
  // DC Load Voltage Measurement
  int dcLoad_sensorValue = analogRead(dcLoad_sensorPin);
  float dcLoad_voltage = (dcLoad_sensorValue * dcLoad_VoltageSlope) + dcLoad_VoltageIntercept;  // Calibrated voltage
  BTSerial.print("Load Voltage: ");
  BTSerial.print(dcLoad_voltage);
  BTSerial.print("|")
  //Serial.print("Load Voltage: ");
  //Serial.println(dcLoad_voltage);
  //Serial.print("Load ADC: ");
  //Serial.println(dcLoad_sensorValue);

  // DC Arduino Voltage Measurement
  int dcArduino_sensorValue = analogRead(dcArduino_sensorPin);
  float dcArduino_voltage = (dcArduino_sensorValue * dcArduino_VoltageSlope) + dcArduino_VoltageIntercept;
  BTSerial.print("Arduino Voltage: ");
  BTSerial.print(dcArduino_voltage);
  BTSerial.println("|")
  //Serial.print("Load Voltage: ");
  //Serial.println(dcLoad_voltage);
  //Serial.print("Arduino ADC: ");
  //Serial.println(dcArduino_sensorValue);

  // Automatic Circuit Closure Logic
  if (circuitTripped && dcArduino_voltage >= autoCloseVoltageThreshold) {
    digitalWrite(relayPin, HIGH); // Close the relay (reset the circuit)
    circuitTripped = false;      // Reset tripped flag
    BTSerial.println("Circuit Auto-Closed (Voltage above threshold)");
    Serial.println("Circuit Auto-Closed (Voltage above threshold)");
  }

  // Manual Trip/Reset
  if (BTSerial.available() > 0) {
    char incomingCommand = BTSerial.read();

    if (incomingCommand == 'T' && !circuitTripped) { // TRIP
      digitalWrite(relayPin, LOW); // Open the relay (trip the circuit)
      circuitTripped = true;      // Set the flag so it doesn't keep tripping
      BTSerial.println("Circuit Tripped Manually");
    } else if (incomingCommand == 'R') { // RESET
      digitalWrite(relayPin, HIGH); // close the relay (reset the circuit)
      circuitTripped = false;      //reset tripped flag
      BTSerial.println("Circuit Reset Manually");
    } else {
      BTSerial.println("Invalid command.");
    }

    while (BTSerial.available() > 0) { //clear the input buffer.
      BTSerial.read();
    }
  }
  delay(1); // Very short delay for ADC stability
}
