#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "driver/mcpwm.h"

#include "pwm.h"

//
// PWM clock is running at 16 Mhz using no clock divider out of 16 MHz system clock.
//
// 15.625 KHz is the target PWM frequency and we wanna be able to control the duty cycle from 0 to 100%
//

////////////////////////////////////////////////////////////////////////////////
//
// private definitions
//
////////////////////////////////////////////////////////////////////////////////
#define PWM_TARGET_FREQUENCY      15625                           // 15.625Kz Khz

#define GPIO_PWM0A_OUT 15       //Set GPIO 15 as PWM0A
#define GPIO_PWM0B_OUT 16       //Set GPIO 16 as PWM0B


////////////////////////////////////////////////////////////////////////////////
//
// pwm configuration 
//
////////////////////////////////////////////////////////////////////////////////
static void
pwm_init_config(void)
{
  mcpwm_config_t pwm_config;

  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);

  pwm_config.frequency = PWM_TARGET_FREQUENCY; 
  pwm_config.cmpr_a = 0;
  pwm_config.cmpr_b = 0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}

static void pwm_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
  mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
  mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
}

static void pwm_set_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
  mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
  mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
  mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces 
//
////////////////////////////////////////////////////////////////////////////////
void
pwm_init(void)
{
  pwm_init_config();
}

void
pwm_control(pwm_channel_t chnl, uint8_t pct)
{
  switch(chnl)
  {
  case pwm_channel_0:
    if(pct == 0)
    {
      // stop
      pwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    }
    else
    {
      pwm_set_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, pct);
    }
    break;
  }
}
