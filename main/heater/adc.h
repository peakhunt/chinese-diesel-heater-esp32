#ifndef __ACD_DEF_H__
#define __ACD_DEF_H__

#include <stdint.h>

typedef enum
{
  adc_channel_map_0,        // PIN34, ADC1_CH6
  adc_channel_map_1,        // PIN35, ADC1_CH7
} adc_channel_map_t;
#define ADC_MAX_CHANNELS    (adc_channel_map_1 + 1)

typedef uint16_t adcsample_t;

typedef void (*adc_listener)(adc_channel_map_t ch, adcsample_t sample, void* arg);

extern void adc_init(void);
extern void adc_listen(adc_channel_map_t ch, adc_listener listener, void* arg);
extern adcsample_t adc_get(adc_channel_map_t ch);
extern float adc_get_volt(adc_channel_map_t ch);

#endif //!__ACD_DEF_H__
