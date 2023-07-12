#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


const char* ssid = "Adham";
const char* password = "Adham000";

// Web server port
const int serverPort = 80;

// Relay pin
const int relayPin = D1;

// Light sensor pin
const int lightSensorPin = A0;

// PWM component pin
const int pwmPin = D2;

// Default phase durations in seconds
const int heatingDuration = 5;
const int lightMeasurementDuration = 1;
const int pwmComponentDuration = 3;
const int delayBetweenPhases = 2;

// Variables to store current phase and durations
int currentPhase = 0;
int heatingPhaseDuration = heatingDuration;
int lightMeasurementPhaseDuration = lightMeasurementDuration;
int pwmComponentPhaseDuration = pwmComponentDuration;
int delayBetweenPhasesDuration = delayBetweenPhases;

// Web server instance
ESP8266WebServer server(serverPort);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  // Initialize relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Initially turned off

  // Initialize PWM component pin
  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, 0); // Initially set PWM to 0

  // Start the web server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();

  // Perform actions based on the current phase
  switch (currentPhase) {
    case 0: // Heating phase
      digitalWrite(relayPin, HIGH); // Turn on the relay
      delay(heatingPhaseDuration * 1000); // Wait for the heating duration
      digitalWrite(relayPin, LOW); // Turn off the relay
      break;

    case 1: // Light measurement phase
      float averageLightLevel = getAverageLightLevel();
      int pwmValue = map(averageLightLevel, 0, 1023, 0, 255); // Map light intensity to PWM value
      analogWrite(pwmPin, pwmValue); // Set the PWM value
      delay(lightMeasurementPhaseDuration * 1000); // Wait for the light measurement duration
      break;

    case 2: // PWM component phase
      // Perform actions with the sophisticated component controlled by PWM
      // Adjust the actions based on the previously set PWM value
      delay(pwmComponentPhaseDuration * 1000); // Wait for the PWM component duration
      break;

    default: // Delay between phases
      delay(delayBetweenPhasesDuration * 1000); // Wait for the delay between phases
      break;
  }

  // Move to the next phase
  currentPhase = (currentPhase + 1) % 4;

  // Check if the phase durations have been updated through the web server
  if (server.hasArg("heating")) {
    heatingPhaseDuration = server.arg("heating").toInt();
  }
  if (server.hasArg("light_measurement")) {
    lightMeasurementPhaseDuration = server.arg("light_measurement").toInt();
  }
  if (server.hasArg("pwm_component")) {
    pwmComponentPhaseDuration = server.arg("pwm_component").toInt();
  }
  if (server.hasArg("delay")) {
    delayBetweenPhasesDuration = server.arg("delay").toInt();
  }
}

// Function to calculate the average light level from the light sensor
float getAverageLightLevel() {
  int sum = 0;
  for (int i = 0; i < 10; i++) { // Perform 10 readings for better accuracy
    sum += analogRead(lightSensorPin);
    delay(100);
  }
  return static_cast<float>(sum) / 10.0;
}
