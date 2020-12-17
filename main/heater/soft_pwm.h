#ifndef __SOFT_PWM_DEF_H__
#define __SOFT_PWM_DEF_H__

#include <stdint.h>
#include <stdbool.h>

#include "mainloop_timer.h"
#include "gpio.h"

typedef enum
{
  soft_pwm_duty_state_on,
  soft_pwm_duty_state_off,
} soft_pwm_duty_state_t;

typedef struct
{
  bool                    running;
  soft_pwm_duty_state_t   duty_state;
  SoftTimerElem           tmr;
  gpio_out_pin_t          pin;

  uint16_t                on_time;
  uint16_t                off_time;
} soft_pwm_t;

extern void soft_pwm_init(soft_pwm_t* pwm, gpio_out_pin_t pin, float freq, uint8_t pct);
extern void soft_pwm_start(soft_pwm_t* pwm);
extern void soft_pwm_stop(soft_pwm_t* pwm);
extern void soft_pwm_change(soft_pwm_t* pwm, float freq, uint8_t pct);

#endif /* !__SOFT_PWM_DEF_H__ */
