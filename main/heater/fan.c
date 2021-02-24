#include "fan.h"

static void
fan_motor_rotating_timer(SoftTimerElem* te)
{
  fan_t* fan = (fan_t*)te->priv;

  if(te == &(fan->tmr1))
  {
    // pulse detection timer expired
    switch(fan->motor_state)
    {
    case fan_motor_not_rotating:
      break;

    case fan_motor_starting:
      fan->motor_state = fan_motor_not_rotating;
      mainloop_timer_cancel(&fan->tmr2);
      break;

    case fan_motor_rotating:
      fan->motor_state = fan_motor_not_rotating;
      break;
    }
  }
  else if(te == &(fan->tmr2))
  {
    // long timer expired
    switch(fan->motor_state)
    {
    case fan_motor_starting:
      fan->motor_state = fan_motor_rotating;
      break;

    default:
      break;
    }
  }
}

static void
fan_motor_hall_input_callback(gpio_in_pin_t pin, gpio_input_state_t state, void* arg)
{
  fan_t*    fan = (fan_t*)arg;

  switch(fan->motor_state)
  {
  case fan_motor_not_rotating:
    fan->motor_state = fan_motor_starting;
    mainloop_timer_schedule(&fan->tmr1, FAN_HALL_SENSOR_TMR1_PERIOD);
    mainloop_timer_schedule(&fan->tmr2, FAN_HALL_SENSOR_TMR2_PERIOD);
    break;

  case fan_motor_starting:
    mainloop_timer_cancel(&fan->tmr1);
    mainloop_timer_schedule(&fan->tmr1, FAN_HALL_SENSOR_TMR1_PERIOD);
    break;

  case fan_motor_rotating:
    mainloop_timer_cancel(&fan->tmr1);
    mainloop_timer_schedule(&fan->tmr1, FAN_HALL_SENSOR_TMR1_PERIOD);
    break;
  }
}

void
fan_init(fan_t* fan, pwm_channel_t chnl, gpio_in_pin_t hall_input)
{
  fan->on = false;
  fan->pwm_chnl = chnl;
  fan->pwr = FAN_DEFAULT_POWER;

  pwm_control(fan->pwm_chnl, 0);


  fan->hall_input  = hall_input;
  fan->motor_state = fan_motor_not_rotating;

  soft_timer_init_elem(&fan->tmr1);
  soft_timer_init_elem(&fan->tmr2);
  fan->tmr1.cb    = fan_motor_rotating_timer;
  fan->tmr1.priv  = fan;
  fan->tmr2.cb    = fan_motor_rotating_timer;
  fan->tmr2.priv  = fan;

  gpio_set_debounce(fan->hall_input, false);
  gpio_listen(fan->hall_input,  fan_motor_hall_input_callback, fan);

  //
  // enable M_INA
  //
  gpio_set(gpio_out_pin_19, true);
}

void
fan_start(fan_t* fan)
{
  fan->on = true;
  pwm_control(fan->pwm_chnl, fan->pwr);
}

void
fan_stop(fan_t* fan)
{
  fan->on = false;
  pwm_control(fan->pwm_chnl, 0);
}

void
fan_set_power(fan_t* fan, uint8_t pwr)
{
  fan->pwr = pwr;
  if(fan->on)
  {
    pwm_control(fan->pwm_chnl, fan->pwr);
  }
}
