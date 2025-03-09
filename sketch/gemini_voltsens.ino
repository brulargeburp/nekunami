#include <SoftwareSerial.h>

int dc_sensorPin = A0;
int ac_sensorPin = A1;
int relayPin = 7;
float dcVoltageThreshold = 4.5; // Example
float acVoltageThreshold = 1.0; // Example
float hysteresis = 0.5;        // Example
bool circuitTripped = false;

// Calibration values (replace with your actual calibration data)
float dcVoltageSlope = 0.0049;  // Example: slope from calibration
float dcVoltageIntercept = 0.0;  // Example: intercept from calibration

float acVoltageSlope = 0.0049;  // Example: slope from calibration
float acVoltageIntercept = 0.0;  // Example: intercept from calibration

const int numSamples = 50; // Number of samples for AC RMS
int acSamples[numSamples];
int sampleIndex = 0;
unsigned long lastSampleTime = 0;
const unsigned long sampleInterval = 2; // Sample every 2 milliseconds (adjust for your AC frequency)

// bluetooth serial
SoftwareSerial BTSerial(0, 1);

void setup() {
  BTSerial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Circuit is initially ON
}

void loop() {
  // DC Voltage Measurement
  int dc_sensorValue = analogRead(dc_sensorPin);
  float dc_voltage = (dc_sensorValue * dcVoltageSlope) + dcVoltageIntercept;  // Calibrated voltage
  BTSerial.print("Measured DC Voltage: ");
  BTSerial.println(dc_voltage);

  // AC Voltage Measurement (RMS)
  if (millis() - lastSampleTime >= sampleInterval) {
    lastSampleTime = millis();
    acSamples[sampleIndex] = analogRead(ac_sensorPin);
    sampleIndex++;

    if (sampleIndex >= numSamples) {
      sampleIndex = 0;
      float sumSquares = 0;
      for (int i = 0; i < numSamples; i++) {
        // Convert to voltage, center, and calculate squares
        float centeredVoltage = ((acSamples[i] * acVoltageSlope) + acVoltageIntercept) - 2.5;
        sumSquares += (centeredVoltage * centeredVoltage);
      }
      float ac_voltage = sqrt(sumSquares / numSamples);
      BTSerial.print("Measured AC Voltage (RMS): ");
      BTSerial.println(ac_voltage);
    }
  }

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
    if (dc_voltage > dcVoltageThreshold || ac_voltage > acVoltageThreshold) {
      BTSerial.println("Overvoltage detected!");
      digitalWrite(relayPin, LOW); //trip
      circuitTripped = true;
      BTSerial.println("Circuit tripped.");
    }
  } else if (dc_voltage < dcVoltageThreshold - hysteresis && ac_voltage < acVoltageThreshold - hysteresis) {
    BTSerial.println("Voltage is back to normal");
    digitalWrite(relayPin, HIGH);
    circuitTripped = false;
    BTSerial.println("Circuit is reenabled");
  }
  delay(1); // Very short delay for ADC stability
}
