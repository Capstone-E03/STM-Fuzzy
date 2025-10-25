#include "app_config.h"
#include "sensors.h"
#include "dht22.h"
#include "fuzzy_fresh.h"
#include "fuzzy_store.h"
#include "mqtt_at.h"
#include <stdio.h>

/* DHT instance (PA5, DHT22, pull-up) */
static DHT_sensor dht = {
  .DHT_Port = DHT_DATA_GPIO_Port,
  .DHT_Pin  = DHT_DATA_Pin,
  .type     = DHT22,
  .pullUp   = GPIO_PULLUP,
  .lastPollingTime = 0,
  .lastTemp = -128.0f,
  .lastHum  = -128.0f
};

static void publish_sensor(float mq135_ppm,float mq2_ppm,float pH,float T,float RH){
  char json[192];
  snprintf(json,sizeof(json),
    "{\"mq135_ppm\":%.1f,\"mq2_ppm\":%.1f,\"pH\":%.2f,\"T\":%.1f,\"RH\":%.1f}",
    mq135_ppm, mq2_ppm, pH, T, RH);
  MQTT_Publish(MQTT_TOPIC_DATA, json, MQTT_QOS_DATA, 0);
}

static void publish_fresh(const char* label){
  char json[64];
  snprintf(json,sizeof(json), "{\"fresh\":\"%s\"}", label);
  MQTT_Publish(MQTT_TOPIC_FRESH, json, MQTT_QOS_FRESH, 0);
}

void App_Run(void){
  /* Init modules */
  Sensors_Init();
  ESP_Init();
  ESP_JoinWiFi(WIFI_SSID, WIFI_PASS);
  MQTT_Connect(MQTT_CLIENT_DATA, MQTT_HOST, MQTT_PORT);
  /* CONNECT sekali sudah cukup untuk publish ke 2 topic */

  for(;;){
    /* 1) Read ADC → volts */
    float v135=0, v2=0, vph=0;
    Sensors_ReadVoltages(&v135, &v2, &vph);

    /* 2) Convert */
    float ppm135 = MQ135_V_to_ppm(v135);
    float ppm2   = MQ2_V_to_ppm(v2);
    float pH     = PH_V_to_pH(vph, PH_OFFSET_DEFAULT);

    /* 3) DHT */
    DHT_data d = DHT_getData(&dht);
    float T  = d.temp;
    float RH = d.hum;

    /* 4) Fuzzy */
    const char* fresh = fuzzy_fresh_label(ppm135, ppm2, pH);
    /* const char* store = fuzzy_store_label(T, RH); // jika ingin dipakai */

    /* 5) Publish */
    publish_sensor(ppm135, ppm2, pH, T, RH);
    publish_fresh(fresh);

    HAL_Delay(MAIN_LOOP_MS);
  }
}
