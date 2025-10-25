#pragma once
#include "main.h"

/* Tipe sensor */
typedef enum { DHT11 = 0, DHT22 = 1 } DHT_Type;

/* Data keluaran */
typedef struct {
  float hum;   /* %RH */
  float temp;  /* °C  */
} DHT_data;

/* Deskriptor sensor */
typedef struct {
  GPIO_TypeDef* DHT_Port;
  uint16_t      DHT_Pin;
  DHT_Type      type;
  uint32_t      pullUp;     /* GPIO_NOPULL atau GPIO_PULLUP */

  /* polling control */
  uint32_t lastPollingTime;
  float    lastTemp;
  float    lastHum;
} DHT_sensor;

/* API */
DHT_data DHT_getData(DHT_sensor *sensor);
