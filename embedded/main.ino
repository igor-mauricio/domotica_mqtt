#include <Servo.h>

// Pin definitions
const int whiteLedPin = 13;

// Variables
String inputString = "";
bool stringComplete = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize pins
  pinMode(whiteLedPin, OUTPUT);

  inputString.reserve(200); // Reserve memory for the input string
}

void loop() {
  // Check if a complete string has been received
  if (stringComplete) {
    handleSerialInput(inputString);
    inputString = "";
    stringComplete = false;
  }

  // Periodically send sensor data
  // sendSensorData();
}

void handleSerialInput(String input) {
  int separatorIndex = input.indexOf(':');
  if (separatorIndex == -1) {
    Serial.println("Invalid format");
    return; // Invalid format
  }

  String topic = input.substring(0, separatorIndex);
  String message = input.substring(separatorIndex + 1, input.length()-1);

  if (topic == "casa/led") {
    if (message == "on") {
      digitalWrite(whiteLedPin, HIGH);
      Serial.println("TURNING ON LED");
  } else if (message == "off") {
      digitalWrite(whiteLedPin, LOW);
      Serial.println("TURNING OFF LED");
    } else {
      Serial.println("Command not recognized");
    }
    Serial.println("LEDCOMMANDRECEIVED - [" + message+"]");
  }
}

// void sendSensorData() {
//   // Read soil humidity sensor
//   int soilValue = analogRead(soilSensorPin);
//   String soilMessage = "sensor/soil:" + String(soilValue);
//   Serial.println(soilMessage);

//   // Read button state
//   int buttonState = digitalRead(buttonPin);
//   String buttonMessage = "sensor/button:" + String(buttonState);
//   Serial.println(buttonMessage);

//   delay(1000); // Send data every second
// }

// Serial event handler
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}