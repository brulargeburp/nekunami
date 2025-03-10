#include <SoftwareSerial.h>

int dcLoad_sensorPin = A0; // DC Load sensor on A0
int dcArduino_sensorPin = A1; // DC Arduino sensor on A1 (replacing AC sensor)
int relayPin = 7;
float dcLoadVoltageThreshold = 4.5; // Example threshold for load voltage
float dcArduinoVoltageThreshold = 4.5; // Example threshold for arduino voltage
float hysteresis = 0.5;        // Example hysteresis value
bool circuitTripped = false;

// Calibration values (replace with your actual calibration data)
float dcLoad_VoltageSlope = 0.0049;  // Example: slope from calibration for the load sensor
float dcLoad_VoltageIntercept = 0.0;  // Example: intercept from calibration for the load sensor

float dcArduino_VoltageSlope = 0.0049;  // Example: slope from calibration for the arduino sensor
float dcArduino_VoltageIntercept = 0.0;  // Example: intercept from calibration for the arduino sensor

// bluetooth serial
SoftwareSerial BTSerial(0, 1);

void setup() {
  BTSerial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Circuit is initially ON
}

void loop() {
  // DC Load Voltage Measurement
  int dcLoad_sensorValue = analogRead(dcLoad_sensorPin);
  float dcLoad_voltage = (dcLoad_sensorValue * dcLoad_VoltageSlope) + dcLoad_VoltageIntercept;  // Calibrated voltage
  BTSerial.print("Measured DC Load Voltage: ");
  BTSerial.println(dcLoad_voltage);

  // DC Arduino Voltage Measurement
  int dcArduino_sensorValue = analogRead(dcArduino_sensorPin);
  float dcArduino_voltage = (dcArduino_sensorValue * dcArduino_VoltageSlope) + dcArduino_VoltageIntercept;
  BTSerial.print("Measured DC Arduino Voltage: ");
  BTSerial.println(dcArduino_voltage);

  // Manual Trip/Reset
  if (BTSerial.available() > 0) {
    char incomingCommand = BTSerial.read();

    if (incomingCommand == 'T' && !circuitTripped) { // TRIP
      BTSerial.println("Tripping circuit manually.");
      digitalWrite(relayPin, LOW); // Open the relay (trip the circuit)
      circuitTripped = true;      // Set the flag so it doesn't keep tripping
      BTSerial.println("Circuit tripped.");
    } else if (incomingCommand == 'R') { // RESET
      BTSerial.println("Resetting circuit manually.");
      digitalWrite(relayPin, HIGH); // close the relay (reset the circuit)
      circuitTripped = false;      //reset tripped flag
      BTSerial.println("Circuit reenabled");
    } else {
      BTSerial.println("Invalid command.");
    }

    while (BTSerial.available() > 0) { //clear the input buffer.
      BTSerial.read();
    }
  }

  // Automatic tripping logic (add to loop)
  if (!circuitTripped) {
    if (dcLoad_voltage > dcLoadVoltageThreshold || dcArduino_voltage > dcArduinoVoltageThreshold) {
      BTSerial.println("Overvoltage detected!");
      digitalWrite(relayPin, LOW); //trip
      circuitTripped = true;
      BTSerial.println("Circuit tripped.");
    }
  } else if (dcLoad_voltage < dcLoadVoltageThreshold - hysteresis && dcArduino_voltage < dcArduinoVoltageThreshold - hysteresis) {
    BTSerial.println("Voltage is back to normal");
    digitalWrite(relayPin, HIGH);
    circuitTripped = false;
    BTSerial.println("Circuit is reenabled");
  }
  delay(1); // Very short delay for ADC stability
}
