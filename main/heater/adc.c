#include "driver/adc.h"

#include "mainloop_timer.h"
#include "adc.h"

#define SAMPLING_INTERVAL     50        // 50ms
#define DEFAULT_VREF          3300

typedef struct
{
  adcsample_t     sample;
  adc_listener    listener;
  void*           arg;
} adc_channel_struct_t;

////////////////////////////////////////////////////////////////////////////////
//
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static adc_channel_map_t              _current_chnl = adc_channel_map_0;
static adc_channel_struct_t           _samples[ADC_MAX_CHANNELS];
static SoftTimerElem                  _sampling_tmr;

////////////////////////////////////////////////////////////////////////////////
//
// private utilities
//
////////////////////////////////////////////////////////////////////////////////
static void
adc_sampling_tmr_callback(SoftTimerElem* te)
{
  adc_channel_t chnl = 0;

  switch(_current_chnl)
  {
  case adc_channel_map_0:
    chnl = ADC_CHANNEL_6;
    break;
  case adc_channel_map_1:
    chnl = ADC_CHANNEL_7;
    break;
  }

  _samples[_current_chnl].sample = adc1_get_raw((adc1_channel_t)chnl);

  // FIXME callback
  if(_samples[_current_chnl].listener != NULL)
  {
    _samples[_current_chnl].listener(_current_chnl,
        _samples[_current_chnl].sample,
        _samples[_current_chnl].arg);
  }

  _current_chnl++;
  if(_current_chnl >= ADC_MAX_CHANNELS)
  {
    _current_chnl = adc_channel_map_0;
  }

  mainloop_timer_schedule(&_sampling_tmr, SAMPLING_INTERVAL);
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
adc_init(void)
{
  adc1_config_width(ADC_WIDTH_BIT_12);

  adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_0);
  adc1_config_channel_atten(ADC_CHANNEL_7, ADC_ATTEN_DB_0);

  for(uint8_t i = 0; i < ADC_MAX_CHANNELS; i++)
  {
    _samples[i].sample    = 0;
    _samples[i].listener  = NULL;
    _samples[i].arg       = 0;
  }

  soft_timer_init_elem(&_sampling_tmr);
  _sampling_tmr.cb = adc_sampling_tmr_callback;

  mainloop_timer_schedule(&_sampling_tmr, SAMPLING_INTERVAL);
}

void
adc_listen(adc_channel_map_t ch, adc_listener listener, void* arg)
{
  _samples[ch].listener = listener;
  _samples[ch].arg      = arg;
}

adcsample_t
adc_get(adc_channel_map_t ch)
{
  return _samples[ch].sample;
}

float
adc_get_volt(adc_channel_map_t ch)
{
  float v;
  adcsample_t   adc;

  adc = adc_get(ch);

  v = 3.3f * adc / 4095.0f;
  return v;
}
