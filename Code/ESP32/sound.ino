#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Include the ArduinoJson library

#define SOUND_ANALOG 4 // Pin connected to the sound sensor

// WiFi credentials
const char* ssid = "connect";
const char* password = "ramzeSade";

// MQTT broker details
const char *mqtt_broker = "192.168.136.198";
const char *topic = "sound";
const char *mqtt_username = "uname";
const char *mqtt_password = "upass";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Hard-coded token for the JSON message
const char *hardcoded_token = "your_hardcoded_token"; // Replace with your actual token

// Function prototypes
void reconnectWiFi();
void reconnectMQTT();
void callback(char *topic, byte *payload, unsigned int length);

void setup() {
  Serial.begin(9600);
  
  reconnectWiFi();  // Connect to WiFi
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
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();
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

  // Read the sound sensor value
  int analogValue = analogRead(SOUND_ANALOG);
  
  // Create a JSON document
  DynamicJsonDocument doc(1024); // Create a JSON document
  doc["token"] = hardcoded_token; // Add the hard-coded token
  doc["data"] = analogValue;      // Add the sound sensor value

  // Serialize JSON to a string
  char jsonBuffer[128]; // Buffer to hold the JSON string
  serializeJson(doc, jsonBuffer); // Serialize the JSON document to a string
  
  Serial.println(jsonBuffer);  // Print the JSON string to the Serial Monitor
  client.publish(topic, jsonBuffer);  // Publish the JSON string to the MQTT topic

  delay(1000);  // Delay before the next reading (adjust as needed)
}