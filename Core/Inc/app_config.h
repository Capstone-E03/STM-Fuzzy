#pragma once
#include "main.h"

/* ===== PIN & CHANNEL ===== */
#define MQ135_ADC_CH        ADC_CHANNEL_0    /* PA0 */
#define MQ2_ADC_CH          ADC_CHANNEL_1    /* PA1 */
#define PH_ADC_CH           ADC_CHANNEL_4    /* PA4 */

#define DHT_DATA_GPIO_Port  GPIOA
#define DHT_DATA_Pin        GPIO_PIN_5

/* ===== LOOP ===== */
#define MAIN_LOOP_MS        1000
#define ADC_AVG_COUNT       8

/* ===== VREF & ADC ===== */
#define VREF_F              3.3f
#define ADC_MAX_F           4095.0f

/* ===== pH CALIB ===== */
#define PH_OFFSET_DEFAULT   42.56f   /* dari main-dht.c */
#define PH_SLOPE            (-19.185f)

/* ===== MQ2 (CH4) ===== */
#define MQ2_RLOAD_KOHM      5.0f
#define MQ2_R0_DEFAULT      9.83f
#define MQ2_PARA_CH4        1000.0f
#define MQ2_PARB_CH4        (-2.5f)

/* ===== MQ135 (NH3) ===== */
#define MQ135_RLOAD_KOHM    10.0f
#define MQ135_R0_DEFAULT    76.63f
#define MQ135_PARA_NH3      102.2f
#define MQ135_PARB_NH3      (-2.473f)

/* ===== WIFI & MQTT ===== */
#define WIFI_SSID           "BenzAP"
#define WIFI_PASS           "juarasatu"

#define MQTT_HOST           "test.mosquitto.org"
#define MQTT_PORT           1883

#define MQTT_CLIENT_DATA    "stm32_f401_data"
#define MQTT_TOPIC_DATA     "stm32/sensor/data"
#define MQTT_QOS_DATA       0

#define MQTT_CLIENT_FRESH   "stm32_f401_fresh"
#define MQTT_TOPIC_FRESH    "capstone/e03/fish"
#define MQTT_QOS_FRESH      1
