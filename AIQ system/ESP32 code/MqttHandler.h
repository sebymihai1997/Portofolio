#ifndef MQTThANDLER_H
#define MQTThANDLER_H

#include <Arduino.h>

void setupMqtt(); // Setup MQTT client and connect to broker
void handleMqtt(); // Handle MQTT client loop
void reconnect(); // Reconnect to MQTT broker if disconnected
void publishSensorData(float temperature, float humidity, float iaq, float accuracy, float co2, float tvoc, float dust); // Publish sensor data to MQTT topics
bool isNetworkOnline(); // Check if network is online
void retryConnection(); // Force reconnection to MQTT broker

#endif  // MQTThANDLER_H