#ifndef __FAN_DEF_H__
#define __FAN_DEF_H__ 

#include <stdint.h>
#include "pwm.h"
#include "gpio.h"
#include "mainloop_timer.h"

#define FAN_DEFAULT_POWER     55

#define FAN_HALL_SENSOR_TMR1_PERIOD   200       // within 200ms, we should get hall sensor toggle
#define FAN_HALL_SENSOR_TMR2_PERIOD   3000      // for 3000ms, the toggle should be persistent

typedef enum
{
  fan_motor_not_rotating,
  fan_motor_starting,
  fan_motor_rotating,
} fan_motor_state_t;

typedef struct
{
  bool            on;
  pwm_channel_t   pwm_chnl;
  uint8_t         pwr;

  gpio_in_pin_t       hall_input;
  fan_motor_state_t   motor_state;
  SoftTimerElem       tmr1;
  SoftTimerElem       tmr2;
} fan_t;

extern void fan_init(fan_t* fan, pwm_channel_t chnl, gpio_in_pin_t hall_input);
extern void fan_start(fan_t* fan);
extern void fan_stop(fan_t* fan);
extern void fan_set_power(fan_t* fan, uint8_t pwr);

#endif /* !__FAN_DEF_H__ */
