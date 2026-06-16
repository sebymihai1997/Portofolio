/*  
    Master Thesis - Air Quality Monitoring System
    Student: Mihai Sebastian-Andrei
    MqttHandler.cpp
    MQTT handling for Air Quality Monitoring System
    - Connects to WiFi using multiple access points
    - Resolves MQTT broker IP using mDNS
    - Manages MQTT connection and publishes sensor data
*/

#include "MqttHandler.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>

// WiFi configuration
const char* WIFI_SSID_HOME = "Caministii";
const char* WIFI_PASS_HOME = "305qazwsx";

const char* WIFI_SSID_HOTSPOT = "Xiaomi 14T";
const char* WIFI_PASS_HOTSPOT = "12345678";

// MQTT configuration
const char* MQTT_BROKER_HOSTNAME = "raspberrypi"; 
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "aqm/living/telemetry";

// Global objects for WiFi and MQTT
WiFiMulti wifiMulti; // WiFiMulti for managing multiple access points
WiFiClient espClient; // WiFi client for MQTT
PubSubClient client(espClient); // MQTT client

// Variable to hold resolved MQTT broker IP
IPAddress mqttServerIP; // Initialized to

bool mqttEnabled = false; // Flag to indicate if MQTT is enabled

// Function to resolve MQTT broker IP using mDNS
void resolveMqttServer() {
    Serial.println("[MQTT] Search for server 'raspberrypi.local'...");
    
    // Start mDNS responder
    if (!MDNS.begin("esp32-client")) {
        Serial.println("[MQTT] Errior starting mDNS responder!");
        return;
    }

    for (int i = 0; i < 3; i++) {
        mqttServerIP = MDNS.queryHost(MQTT_BROKER_HOSTNAME); // Resolve hostname to IP
        
        Serial.print("[DEBUG] MDNS a return IP: ");
        Serial.println(mqttServerIP.toString());

        if (mqttServerIP.toString() != "0.0.0.0") {
            // Successfully resolved
            Serial.print("[MQTT] Server found at IP: ");
            Serial.println(mqttServerIP);
            client.setServer(mqttServerIP, MQTT_PORT);
            mqttEnabled = true; // Valid MQTT server IP found
            return;
        }

        Serial.print("[MQTT] Try to connect "); Serial.print(i + 1); Serial.println("/3 failed...");
        delay(1000);
    }
}

// Function to setup WiFi and connect to MQTT broker
void setupMqtt() {
    // Connect to WiFi multiple access points
    wifiMulti.addAP(WIFI_SSID_HOME, WIFI_PASS_HOME);
    wifiMulti.addAP(WIFI_SSID_HOTSPOT, WIFI_PASS_HOTSPOT);

    Serial.println("[WiFi] Se cauta retea...");
    
    // Wait for connection
    int wifiRetries = 0;
    while (wifiMulti.run() != WL_CONNECTED && wifiRetries < 3) {
        delay(500);
        Serial.print(".");
        wifiRetries++;
    }
    
    if (wifiMulti.run() != WL_CONNECTED) {
        Serial.println("\n[WiFi] Abandon: No network connection.");
        mqttEnabled = false; // Offline mode
        return;
    }

    Serial.println("\n[WiFi] Conectat!");
    Serial.print("[WiFi] IP: "); Serial.println(WiFi.localIP());
    Serial.print("[WiFi] SSID: "); Serial.println(WiFi.SSID());

    // Resolve MQTT broker IP
    resolveMqttServer();
}

// Function reconnect to MQTT broker if disconnected
void reconnect() {
    
    if (!mqttEnabled) return; // MQTT disabled, skip reconnection

    if (!client.connected()) {
        
        if (wifiMulti.run() != WL_CONNECTED) return;

        
        if (mqttServerIP.toString() == "0.0.0.0") {
            resolveMqttServer();
        }

        Serial.print("[DEBUG] Incerc conectarea la Broker IP: ");
        Serial.println(mqttServerIP);

        Serial.print("[MQTT] Trying to connect...");
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println("Connected!");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" retry connection.");
        }
    }
}

// Function to handle MQTT client loop
void handleMqtt() {
    if (!mqttEnabled) return; // MQTT disabled, skip reconnection
    // Ensure WiFi is connected
    if (wifiMulti.run() != WL_CONNECTED) {
        // Not connected to WiFi, skip MQTT handling
        return;
    }

    // Reconnect to MQTT broker if disconnected
    if (!client.connected()) {
        reconnect();
    }
    
    // Process MQTT client loop
    client.loop();
}

// Function to publish sensor data to MQTT topics
void publishSensorData(float temp, float hum, float iaq, float accuracy, float co2, float tvoc, float dust) {
    if (!mqttEnabled) return; // MQTT disabled, skip reconnection
    if (!client.connected()) return; // Not connected to MQTT broker

    // Create JSON payload
    // Format: {"temp": 24.5, "hum": 45.2, "iaq": 123.0, "co2": 400.0, "tvoc": 15.0, "dust": 0.05}
    char msg[200];
    snprintf(msg, sizeof(msg), 
             "{\"temp\": %.1f, \"hum\": %.1f, \"iaq\": %.0f, \"acc\": %.0f, \"co2\": %.0f, \"tvoc\": %.0f, \"dust\": %.2f}",
             temp, hum, iaq, accuracy, co2, tvoc, dust);

    Serial.print("[MQTT] Trimit: ");
    Serial.println(msg);
    
    // Publish message to MQTT topic
    client.publish(MQTT_TOPIC, msg);
}

// Function to check if network is online
bool isNetworkOnline() {
    return mqttEnabled && client.connected() && (WiFi.status() == WL_CONNECTED);
}
// Function to force retry connection to MQTT broker
void retryConnection() {
    if (isNetworkOnline()) {
        // Already connected, no need to retry
        return;
    }

    Serial.println("\n[USER] Buton pressed -> Try reconnection...");
    
    
    // Reset mqttEnabled flag
    mqttEnabled = false; 
    mqttServerIP = IPAddress(0, 0, 0, 0);
    client.setServer(mqttServerIP, MQTT_PORT);
    WiFi.disconnect();
    // Call the setup function again to attempt reconnection
    setupMqtt(); 
}