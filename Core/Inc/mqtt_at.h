#pragma once
#include "main.h"

/* Gunakan USART1 ke ESP-AT */
HAL_StatusTypeDef ESP_Init(void);
HAL_StatusTypeDef ESP_JoinWiFi(const char* ssid, const char* pass);

/* MQTT over TCP (raw) */
HAL_StatusTypeDef MQTT_Connect(const char* clientId, const char* host, uint16_t port);
HAL_StatusTypeDef MQTT_Publish(const char* topic, const char* payload, uint8_t qos, uint8_t retain);
