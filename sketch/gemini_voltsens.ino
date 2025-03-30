//#include <SoftwareSerial.h>

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
//SoftwareSerial BTSerial(3, 4); // RX, TX - Use different pins than Serial!

// Voltage threshold for automatic circuit closure
float autoCloseVoltageThreshold = 6.0; // Set the threshold to 3.5V

void setup() {
  //while (!Serial);
  //BTSerial.begin(9600); // Initialize BTSerial
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Circuit is initially ON
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

  // Automatic Circuit Closure Logic
  if (!circuitTripped && dcArduino_voltage >= autoCloseVoltageThreshold) {
    digitalWrite(relayPin, HIGH); // Close the relay (reset the circuit) - Changed to HIGH to match initial state on reset.
    circuitTripped = true;      // Set tripped flag
    Serial.println("Circuit Auto-Closed (Voltage above threshold)");
  }

  // Manual Trip/Reset
  if (Serial.available() > 0) {
    char incomingCommand = Serial.read();

    if (incomingCommand == '0' && !circuitTripped) { // TRIP
      digitalWrite(relayPin, LOW); // Open the relay (trip the circuit)
      circuitTripped = true;      // Set the flag so it doesn't keep tripping
      Serial.println("Circuit Tripped Manually");
    } else if (incomingCommand == '1') { // RESET
      digitalWrite(relayPin, HIGH); // close the relay (reset the circuit)
      circuitTripped = false;      //reset tripped flag
      Serial.println("Circuit Reset Manually");
    } else {
      Serial.println("Invalid command.");
    }

    while (Serial.available() > 0) { //clear the input buffer.
      Serial.read();
    }
  }

  delay(100); // Increased delay for better readability and stability.
}
