#include "fuzzy_store.h"

/* segitiga */
static float mu_tri(float x, float a, float b, float c) {
  if (x <= a || x >= c) return 0.f;
  if (x == b) return 1.f;
  return (x < b) ? (x - a) / (b - a) : (c - x) / (c - b);
}
static float maxf(float a, float b){ return a>b?a:b; }
static float minf(float a, float b){ return a<b?a:b; }

const char* fuzzy_store_label(float t, float h) {
  /* Temp: dingin(0,0,12.5) hangat(0,12.5,25) panas(12.5,25,25) */
  float t_d = mu_tri(t, 0.f, 0.f, 12.5f);
  float t_hg= mu_tri(t, 0.f, 12.5f, 25.f);
  float t_p = mu_tri(t, 12.5f, 25.f, 25.f);

  /* Hum: rendah(50,50,75) sedang(50,75,100) tinggi(75,100,100) */
  float h_r = mu_tri(h, 50.f, 50.f, 75.f);
  float h_s = mu_tri(h, 50.f, 75.f,100.f);
  float h_t = mu_tri(h, 75.f,100.f,100.f);

  float SB=0,B=0,KB=0,BR=0;

  /* Tabel 10.3 */
  SB = maxf(SB, minf(t_d, h_r));
  B  = maxf(B , minf(t_d, h_s));
  KB = maxf(KB, minf(t_d, h_t));

  B  = maxf(B , minf(t_hg, h_r));
  KB = maxf(KB, minf(t_hg, h_s));
  BR = maxf(BR, minf(t_hg, h_t));

  KB = maxf(KB, minf(t_p, h_r));
  BR = maxf(BR, minf(t_p, h_s));
  BR = maxf(BR, minf(t_p, h_t));

  /* argmax ke label */
  float out[4] = {SB,B,KB,BR};
  int idx=0; for(int i=1;i<4;i++) if(out[i]>out[idx]) idx=i;
  switch(idx){
    case 0: return "SB";
    case 1: return "B";
    case 2: return "KB";
    default: return "BR";
  }
}
