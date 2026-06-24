#ifndef _MY_MATH_H
#define _MY_MATH_H
#include "stdint.h"

void calu_envelope(const int *input, int *output, uint8_t length, int n, _Bool on);
void adjust_waveform_intensity(const int *input, int *output, uint8_t length, float scale, int max_val);

#endif // _MY_MATH_H
