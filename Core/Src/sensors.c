#include "sensors.h"
#include <math.h>
#include <string.h>

/* extern from CubeMX */
extern ADC_HandleTypeDef hadc1;

/* R0 can be recalibrated & persisted; start with defaults */
static float s_mq2_r0   = MQ2_R0_DEFAULT;    /* kOhm */
static float s_mq135_r0 = MQ135_R0_DEFAULT;  /* kOhm */

void Sensors_Init(void) {
  /* nothing yet; if you want, do warm-up waits here */
}

/* ---- ADC ---- */
uint16_t ADC_ReadOnce(uint32_t channel) {
  ADC_ChannelConfTypeDef s = {0};
  s.Channel = channel;
  s.Rank = 1;
  s.SamplingTime = ADC_SAMPLETIME_84CYCLES; /* agak panjang untuk stabil */
  HAL_ADC_ConfigChannel(&hadc1, &s);

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 10);
  uint16_t val = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  return val;
}

float ADC_ReadAverage_V(uint32_t channel, uint8_t n) {
  uint32_t sum = 0;
  for (uint8_t i=0;i<n;i++) {
    sum += ADC_ReadOnce(channel);
    HAL_Delay(2);
  }
  float adc = (float)sum / (float)n;
  return (adc / ADC_MAX_F) * VREF_F;
}

void Sensors_ReadVoltages(float* v_mq135, float* v_mq2, float* v_ph) {
  if (v_mq135) *v_mq135 = ADC_ReadAverage_V(MQ135_ADC_CH, ADC_AVG_COUNT);
  if (v_mq2)   *v_mq2   = ADC_ReadAverage_V(MQ2_ADC_CH,   ADC_AVG_COUNT);
  if (v_ph)    *v_ph    = ADC_ReadAverage_V(PH_ADC_CH,    ADC_AVG_COUNT);
}

/* ---- MQ math ---- */
/* From your MQ.c: Rs = ((Vref - V) * RLOAD) / V */
static float rs_from_v(float v, float rload_kohm) {
  if (v < 1e-6f) return 1e6f;
  return ((VREF_F - v) * rload_kohm) / v; /* kOhm */
}

/* ppm = A * (Rs/R0)^B  */
float MQ2_V_to_ppm(float v) {
  float rs = rs_from_v(v, MQ2_RLOAD_KOHM);
  float ratio = rs / s_mq2_r0;
  if (ratio <= 0.f) return 0.f;
  return MQ2_PARA_CH4 * powf(ratio, MQ2_PARB_CH4);
}

float MQ135_V_to_ppm(float v) {
  float rs = rs_from_v(v, MQ135_RLOAD_KOHM);
  float ratio = rs / s_mq135_r0;
  if (ratio <= 0.f) return 0.f;
  return MQ135_PARA_NH3 * powf(ratio, MQ135_PARB_NH3);
}

/* ---- pH ---- */
/* from your main-dht.c: pH = -19.185 * V + Offset */
float PH_V_to_pH(float v, float offset) {
  return (PH_SLOPE * v) + offset;
}
