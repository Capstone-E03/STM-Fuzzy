#include "mqtt_at.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1; /* ESP-AT UART */

/* --- small RX helper (polling) --- */
static void uart_send(const char* s){ HAL_UART_Transmit(&huart1,(uint8_t*)s,strlen(s),HAL_MAX_DELAY); }
static void uart_send_raw(const uint8_t* d, uint16_t n){
  HAL_UART_Transmit(&huart1,(uint8_t*)d,n,HAL_MAX_DELAY);
}
static void uart_recv_drain(uint32_t timeout_ms){
  uint8_t ch;
  uint32_t t0 = HAL_GetTick();
  while (HAL_GetTick()-t0 < timeout_ms) {
    if (HAL_UART_Receive(&huart1,&ch,1,50)==HAL_OK){ /* drop */ }
  }
}

HAL_StatusTypeDef ESP_Init(void){
  uart_send("AT\r\n");
  uart_recv_drain(200);
  uart_send("ATE0\r\n");    /* echo off */
  uart_recv_drain(200);
  uart_send("AT+RST\r\n");
  HAL_Delay(1200);
  uart_recv_drain(1000);
  uart_send("AT+CWMODE=1\r\n");
  uart_recv_drain(200);
  return HAL_OK;
}

HAL_StatusTypeDef ESP_JoinWiFi(const char* ssid, const char* pass){
  char cmd[160];
  snprintf(cmd,sizeof(cmd),"AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pass);
  uart_send(cmd);
  /* tunggu join (timeout besar) */
  uart_recv_drain(7000);
  return HAL_OK;
}

/* ---- MQTT RAW (minimal) ---- */
static HAL_StatusTypeDef tcp_connect(const char* host, uint16_t port){
  char cmd[160];
  snprintf(cmd,sizeof(cmd),"AT+CIPSTART=\"TCP\",\"%s\",%u\r\n", host, port);
  uart_send(cmd);
  uart_recv_drain(3000);
  return HAL_OK;
}

/* send AT+CIPSEND + binary */
static void cipsend(const uint8_t* buf, uint16_t len){
  char cmd[32];
  snprintf(cmd,sizeof(cmd),"AT+CIPSEND=%u\r\n", (unsigned)len);
  uart_send(cmd);
  HAL_Delay(50);
  uart_send_raw(buf, len);
  HAL_Delay(20);
  uart_recv_drain(200);
}

HAL_StatusTypeDef MQTT_Connect(const char* clientId, const char* host, uint16_t port){
  tcp_connect(host, port);

  /* build CONNECT packet (protocol v3.1.1) */
  uint8_t pkt[128]; uint8_t idx=0;
  uint8_t cid_len = strlen(clientId);
  uint8_t rem_len = 10 + 2 + cid_len;

  pkt[idx++] = 0x10;               /* CONNECT */
  pkt[idx++] = rem_len;
  pkt[idx++] = 0x00; pkt[idx++] = 0x04; /* "MQTT" */
  pkt[idx++] = 'M'; pkt[idx++] = 'Q'; pkt[idx++] = 'T'; pkt[idx++] = 'T';
  pkt[idx++] = 0x04;               /* level 4 */
  pkt[idx++] = 0x02;               /* clean session */
  pkt[idx++] = 0x00; pkt[idx++] = 60; /* keepalive 60s */
  pkt[idx++] = 0x00; pkt[idx++] = cid_len;
  memcpy(&pkt[idx], clientId, cid_len); idx += cid_len;

  cipsend(pkt, idx);
  return HAL_OK;
}

HAL_StatusTypeDef MQTT_Publish(const char* topic, const char* payload, uint8_t qos, uint8_t retain){
  uint16_t tlen = strlen(topic);
  uint16_t plen = strlen(payload);
  uint16_t rem  = 2 + tlen + plen + (qos?2:0);

  uint8_t header = 0x30 | ((qos & 0x3) << 1) | (retain ? 0x01 : 0x00);

  uint8_t pkt_hdr[5]; uint8_t h=0;
  pkt_hdr[h++] = header;
  /* simple remaining length (assume < 127) */
  pkt_hdr[h++] = (uint8_t)rem;

  /* topic length + topic */
  uint8_t pkt_topic[2]; pkt_topic[0] = (tlen>>8)&0xFF; pkt_topic[1] = tlen&0xFF;

  /* CIPSEND total */
  uint16_t total = h + 2 + tlen + (qos?2:0) + plen;
  char cmd[32]; snprintf(cmd,sizeof(cmd),"AT+CIPSEND=%u\r\n", (unsigned)total);
  uart_send(cmd);
  HAL_Delay(30);

  uart_send_raw(pkt_hdr, h);
  uart_send_raw(pkt_topic, 2);
  uart_send_raw((uint8_t*)topic, tlen);
  if (qos){ uint8_t pid[2]={0,1}; uart_send_raw(pid,2); }
  uart_send_raw((uint8_t*)payload, plen);
  HAL_Delay(10);
  uart_recv_drain(150);
  return HAL_OK;
}
