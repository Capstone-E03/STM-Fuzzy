#pragma once
#include "main.h"
#include "app_config.h"

/* Public API */
void Sensors_Init(void);
void Sensors_ReadVoltages(float* v_mq135, float* v_mq2, float* v_ph); /* Volts */
float MQ135_V_to_ppm(float v);  /* NH3 ppm */
float MQ2_V_to_ppm(float v);    /* CH4 ppm */
float PH_V_to_pH(float v, float offset);

/* Helpers exposed (optional) */
uint16_t ADC_ReadOnce(uint32_t channel);
float    ADC_ReadAverage_V(uint32_t channel, uint8_t nSamples);
