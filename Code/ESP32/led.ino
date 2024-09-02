#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Include the ArduinoJson library

#define LED_PIN 16

// WiFi credentials
const char *ssid = "connect";
const char *password = "ramzeSade";

// MQTT broker details
const char *mqtt_broker = "192.168.136.198";
const char *topic = "led";
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
  pinMode(LED_PIN, OUTPUT);
  
  reconnectWiFi();  // Connect to WiFi
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
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

  // Extract token and message from JSON
  const char* token = doc["token"];
  const char* message = doc["data"];

  // Check if token is valid
  if (strcmp(token, valid_token) == 0) {
    // Control the LED based on the message
    if (strncmp(message, "on", 2) == 0) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED is ON");
    } else if (strncmp(message, "off", 3) == 0) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED is OFF");
    } else {
      Serial.println("Invalid command. Use 'on' or 'off'.");
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