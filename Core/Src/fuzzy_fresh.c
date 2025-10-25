#include "fuzzy_fresh.h"

/* Membership segitiga (a-b-c) */
static float mu_tri(float x, float a, float b, float c) {
  if (x <= a || x >= c) return 0.f;
  if (x == b) return 1.f;
  return (x < b) ? (x - a) / (b - a) : (c - x) / (c - b);
}
static float maxf(float a, float b){ return a>b?a:b; }
static float minf(float a, float b){ return a<b?a:b; }

/* MQ135 (0-1000), MQ2 (200-10000) → gabung ke "Gas rendah/sedang/tinggi"  */
static void gas_membership(float mq135_ppm, float mq2_ppm, float* g_low, float* g_med, float* g_high) {
  /* definisi dapat disesuaikan; ini baseline wajar */
  float g135_low  = mu_tri(mq135_ppm,   0,  50, 200);
  float g135_med  = mu_tri(mq135_ppm, 100, 300, 600);
  float g135_high = mu_tri(mq135_ppm, 500, 800,1000);

  float g2_low    = mu_tri(mq2_ppm,   200,  500, 1500);
  float g2_med    = mu_tri(mq2_ppm,  800,  2500, 5000);
  float g2_high   = mu_tri(mq2_ppm, 3500, 7000,10000);

  *g_low  = maxf(g135_low , g2_low );
  *g_med  = maxf(g135_med , g2_med );
  *g_high = maxf(g135_high, g2_high);
}

/* pH membership (sesuai inputmu) */
static void ph_membership(float pH, float* acid, float* neut, float* base) {
  *acid = mu_tri(pH, 0.0f, 6.0f, 7.0f);
  *neut = mu_tri(pH, 6.0f, 7.0f, 8.0f);
  *base = mu_tri(pH, 7.0f, 14.0f, 14.0f);
}

const char* fuzzy_fresh_label(float mq135_ppm, float mq2_ppm, float pH) {
  float gL,gM,gH; gas_membership(mq135_ppm, mq2_ppm, &gL,&gM,&gH);
  float pA,pN,pB; ph_membership(pH, &pA,&pN,&pB);

  float ss=0,s=0,ks=0,b=0;

  /* Tabel 10.2 (9 rules) */
  /* 1 */ ss = maxf(ss, minf(gL, pN));
  /* 2 */ s  = maxf(s , minf(gL, pA));
  /* 3 */ ks = maxf(ks, minf(gL, pB));
  /* 4 */ s  = maxf(s , minf(gM, pN));
  /* 5 */ ks = maxf(ks, minf(gM, pA));
  /* 6 */ b  = maxf(b , minf(gM, pB));
  /* 7 */ b  = maxf(b , gH);             /* gas tinggi → B */
  /* 8 */ b  = maxf(b , pB);             /* pH basa → B  */
  /* 9 */ b  = maxf(b , minf(gH, pN));   /* eksplisit */

  /* argmax */
  float out[4] = {ss,s,ks,b};
  uint8_t idx = 0;
  for (uint8_t i=1;i<4;i++) if (out[i]>out[idx]) idx=i;

  switch(idx){
    case 0: return "SS";
    case 1: return "S";
    case 2: return "KS";
    default: return "B";
  }
}
