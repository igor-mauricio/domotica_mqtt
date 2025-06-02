#include <Servo.h>

// Pin definitions
const int whiteLedPin = 13;
const int fanForward = 7;
const int fanBackward = 6;

// Variables
String inputString = "";
bool stringComplete = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize pins
  pinMode(whiteLedPin, OUTPUT);
  pinMode (fanForward, OUTPUT); //define D7 pin as output
   pinMode (fanBackward, OUTPUT); //define  D6 pin as output

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
  sendSensorData();
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
   } else if (message == "off") {
      digitalWrite(whiteLedPin, LOW);
    }
    Serial.println("LEDCOMMANDRECEIVED - [" + message+"]");
  } else if (topic == "casa/ventilador") {
   if (message == "horario") {
      digitalWrite (fanForward, HIGH);
      digitalWrite (fanBackward, LOW);
   } else if (message == "antihorario") {
      digitalWrite (fanForward, LOW);
      digitalWrite (fanBackward, HIGH);
    } else if (message == "desligar") {
      digitalWrite (fanForward, LOW);
      digitalWrite (fanBackward, LOW);
    }
    Serial.println("LEDCOMMANDRECEIVED - [" + message+"]");
  }
}

void sendSensorData() {
  // Read soil humidity sensor
  int tensaoSolar = analogRead(A1);
  String mensagemSolar = "casa/solar:" + String(tensaoSolar);
  Serial.println(mensagemSolar);

  delay(1000); // Send data every second
}

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