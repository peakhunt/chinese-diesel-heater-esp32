#ifndef __OIL_PUMP_DEF_H__
#define __OIL_PUMP_DEF_H__

#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"
#include "soft_pwm.h"

#define OIL_PUMP_DEF_FREQ     1.0f      // 1.6Hz
#define OIL_PUMP_PULSE_LENGTH 50        // 50ms

#define OIL_PUMP_MIN_FREQ     0.8f
#define OIL_PUMP_MAX_FREQ     5.0f

typedef struct
{
  bool        on;
  float       freq;
  soft_pwm_t  pwm;
} oil_pump_t;

extern void oil_pump_init(oil_pump_t* op, gpio_out_pin_t pin);
extern void oil_pump_on(oil_pump_t* op);
extern void oil_pump_off(oil_pump_t* op);
extern void oil_pump_change_freq(oil_pump_t* op, float freq);

#endif /* !__OIL_PUMP_DEF_H__ */
