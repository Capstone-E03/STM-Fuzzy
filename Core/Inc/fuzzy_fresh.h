#pragma once
#include <stdint.h>

/* Keluaran kesegaran */
typedef enum { FZ_SS=0, FZ_S=1, FZ_KS=2, FZ_B=3 } FreshClass;

const char* fuzzy_fresh_label(float mq135_ppm, float mq2_ppm, float pH);
