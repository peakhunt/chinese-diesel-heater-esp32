#ifndef __NTC50_DEF_H__
#define __NTC50_DEF_H__

#include "adc.h"

typedef struct
{
  float               temp;
  uint8_t             vs;
  float               r1;
  float               r2;
  adc_channel_map_t   chnl;
} ntc50_t;

extern void ntc50_init(ntc50_t* ntc50, uint8_t vs, float r2, adc_channel_map_t chnl);

#endif /* !__NT50_DEF_H__ */
