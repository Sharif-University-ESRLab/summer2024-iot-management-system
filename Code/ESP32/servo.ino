#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h> // Include the ArduinoJson library

#define SERVO_PIN 26 // ESP32 pin GPIO26 connected to servo motor

Servo servoMotor;

// WiFi credentials
const char* ssid = "connect";
const char* password = "ramzeSade";

// MQTT broker details
const char *mqtt_broker = "192.168.136.198";
const char *topic = "servo";
const char *mqtt_username = "uname";
const char *mqtt_password = "upass";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Hard-coded token for validation
const char *valid_token = "your_hardcoded_token"; // Replace with your actual token

// Function prototypes
void reconnectWiFi();
void reconnectMQTT();
void callback(char *topic, byte *payload, unsigned int length);

void setup() {
  Serial.begin(9600);
  
  reconnectWiFi();  // Connect to WiFi
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servoMotor.setPeriodHertz(50);
  servoMotor.attach(SERVO_PIN, 500, 2400);

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  reconnectMQTT();  // Connect to MQTT broker
}

void reconnectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public EMQX MQTT broker connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  
  // Parse the JSON payload
  DynamicJsonDocument doc(1024); // Create a JSON document
  DeserializationError error = deserializeJson(doc, payload, length);
  
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.f_str());
    return;
  }

  // Extract token and degree from JSON
  const char* token = doc["token"];
  int degree = doc["data"];

  // Validate token
  if (strcmp(token, valid_token) == 0) {
    Serial.print("Moving servo to degree: ");
    Serial.println(degree);
    
    // Move servo to the specified degree
    for (int pos = 0; pos <= degree; pos += 1) {
      servoMotor.write(pos);
      delay(15);
    }
    for (int pos = degree; pos >= 0; pos -= 1) {
      servoMotor.write(pos);
      delay(15);
    }
  } else {
    Serial.println("Invalid token.");
  }

  Serial.println("-----------------------");
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost connection. Attempting to reconnect...");
    reconnectWiFi();  // Reconnect to WiFi
  }

  // Check MQTT connection
  if (!client.connected()) {
    reconnectMQTT();  // Reconnect to MQTT
  }

  client.loop();  // Maintain MQTT connection
}