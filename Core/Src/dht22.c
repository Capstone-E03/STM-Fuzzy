/* Adapted directly from your DHT.c (busy-wait, no TIM2 needed) */
#include "dht22.h"

#define lineDown()    HAL_GPIO_WritePin(sensor->DHT_Port, sensor->DHT_Pin, GPIO_PIN_RESET)
#define lineUp()      HAL_GPIO_WritePin(sensor->DHT_Port, sensor->DHT_Pin, GPIO_PIN_SET)
#define getLine()     (HAL_GPIO_ReadPin(sensor->DHT_Port, sensor->DHT_Pin) == GPIO_PIN_SET)
#define DelayMs(ms)   HAL_Delay(ms)

static void goToOutput(DHT_sensor *sensor) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  lineUp();
  GPIO_InitStruct.Pin = sensor->DHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = sensor->pullUp;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(sensor->DHT_Port, &GPIO_InitStruct);
}

static void goToInput(DHT_sensor *sensor) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = sensor->DHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = sensor->pullUp;
  HAL_GPIO_Init(sensor->DHT_Port, &GPIO_InitStruct);
}

DHT_data DHT_getData(DHT_sensor *sensor) {
  DHT_data data = { -128.0f, -128.0f };

  /* polling control (datasheet: 1Hz for DHT22) */
  uint16_t interval = (sensor->type == DHT11) ? 2000 : 1000;
  if ((HAL_GetTick() - sensor->lastPollingTime < interval) && sensor->lastPollingTime != 0) {
    data.hum  = sensor->lastHum;
    data.temp = sensor->lastTemp;
    return data;
  }
  sensor->lastPollingTime = HAL_GetTick() + 1;

  /* start signal */
  goToOutput(sensor);
  lineDown();
  if (sensor->type == DHT11) DelayMs(20);
  else                       DelayMs(2);     /* 1–10 ms cukup */
  lineUp();

  /* response phase */
  goToInput(sensor);

  /* wait for DHT to pull low */
  uint32_t timeout = 0;
  while (getLine()) { if (timeout++ > 10000) return data; }
  timeout = 0;
  while (!getLine()) { if (timeout++ > 10000) return data; }
  timeout = 0;
  while (getLine()) { if (timeout++ > 10000) return data; }

  /* read 40 bits */
  uint8_t bits[5] = {0,0,0,0,0};
  for (uint8_t i=0;i<40;i++) {
    /* wait for low */
    timeout = 0;
    while (!getLine()) { if (timeout++ > 10000) return data; }
    /* count high length */
    uint32_t t = 0;
    while (getLine()) { if (t++ > 20000) break; }
    /* threshold: short ~0 -> 0, long -> 1 */
    bits[i/8] <<= 1;
    if (t > 40) bits[i/8] |= 1;
  }

  /* checksum */
  uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
  if ((sum & 0xFF) != bits[4]) {
    /* invalid frame → return last good if exists */
    data.hum  = sensor->lastHum;
    data.temp = sensor->lastTemp;
    return data;
  }

  if (sensor->type == DHT11) {
    data.hum  = bits[0];
    data.temp = bits[2];
  } else { /* DHT22 */
    int16_t t = (bits[2] & 0x7F) << 8 | bits[3];
    if (bits[2] & 0x80) t = -t;
    uint16_t h = (bits[0] << 8) | bits[1];
    data.temp = t / 10.0f;
    data.hum  = h / 10.0f;
  }

  sensor->lastHum  = data.hum;
  sensor->lastTemp = data.temp;
  return data;
}
