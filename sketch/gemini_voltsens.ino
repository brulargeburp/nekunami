#include <SoftwareSerial.h>

int dcLoad_sensorPin = A0; // DC Load sensor on A0
int dcArduino_sensorPin = A1; // DC Arduino sensor on A1 (replacing AC sensor)
int relayPin = 7;
//int potenPin = A2; // Potentiometer connected to A2 (Removed)
bool circuitTripped = false;

// Calibration values (replace with your actual calibration data)
float dcLoad_VoltageSlope = 0.02445;  // Example: slope from calibration for the load sensor
float dcLoad_VoltageIntercept = 0.0;  // Example: intercept from calibration for the load sensor

float dcArduino_VoltageSlope = 0.02445;  // Example: slope from calibration for the arduino sensor
float dcArduino_VoltageIntercept = 0.0;  // Example: intercept from calibration for the arduino sensor

// bluetooth serial
SoftwareSerial BTSerial(0, 1);

// Potentiometer related variables (Removed)
//float potenVoltage = 0.0; // Voltage read from the potentiometer
//float potenMaxVoltage = 5.0; // Maximum voltage from the potentiometer (assuming 5V reference)
//float potenMinVoltage = 0.0; // Minimum voltage from the potentiometer
//float potenScaleFactor = 1.0; // Scale factor to adjust the potentiometer's effect (adjust as needed)

void setup() {
  BTSerial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Circuit is initially ON
}

void loop() {
  // DC Load Voltage Measurement
  int dcLoad_sensorValue = analogRead(dcLoad_sensorPin);
  float dcLoad_voltage = (dcLoad_sensorValue * dcLoad_VoltageSlope) + dcLoad_VoltageIntercept;  // Calibrated voltage
  BTSerial.print("Load Voltage: ");
  BTSerial.println(dcLoad_voltage);
  Serial.print("Load ADC: "); 
  Serial.println(dcLoad_sensorValue);

  // DC Arduino Voltage Measurement
  int dcArduino_sensorValue = analogRead(dcArduino_sensorPin);
  float dcArduino_voltage = (dcArduino_sensorValue * dcArduino_VoltageSlope) + dcArduino_VoltageIntercept;
  BTSerial.print("Arduino Voltage: ");
  BTSerial.println(dcArduino_voltage);
  Serial.print("Arduino ADC: ");
  Serial.println(dcArduino_sensorValue);

  // Potentiometer Reading and Scaling (Removed)
  //int potenValue = analogRead(potenPin);
  //potenVoltage = (potenValue * potenMaxVoltage) / 1023.0; // Convert analog reading to voltage
  //BTSerial.print("Potentiometer Voltage: ");
  //BTSerial.println(potenVoltage);

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
