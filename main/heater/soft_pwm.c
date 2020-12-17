#include "soft_pwm.h"

static inline void
soft_pwm_calc_on_time(soft_pwm_t* pwm, float freq, uint8_t pct)
{
  uint16_t one_cycle = (uint16_t)(1000.0f / freq);
  uint16_t on_time = (uint16_t)((float)one_cycle * pct / 100.0f);

  pwm->on_time  = on_time;
  pwm->off_time = one_cycle - on_time;
}

static void
soft_pwm_tmr_callback(SoftTimerElem* te)
{
  soft_pwm_t* pwm = (soft_pwm_t*)te->priv;

  if(pwm->duty_state == soft_pwm_duty_state_on)
  {
    gpio_set(pwm->pin, false);
    pwm->duty_state = soft_pwm_duty_state_off;
    mainloop_timer_schedule(&pwm->tmr, pwm->off_time);
  }
  else
  {
    gpio_set(pwm->pin, true);
    pwm->duty_state = soft_pwm_duty_state_on;
    mainloop_timer_schedule(&pwm->tmr, pwm->on_time);
  }
}

void
soft_pwm_init(soft_pwm_t* pwm, gpio_out_pin_t pin, float freq, uint8_t pct)
{
  pwm->running    = false;
  pwm->pin        = pin;
  pwm->duty_state = soft_pwm_duty_state_on;

  soft_timer_init_elem(&pwm->tmr);
  pwm->tmr.priv = pwm;
  pwm->tmr.cb = soft_pwm_tmr_callback;

  soft_pwm_calc_on_time(pwm, freq, pct);
}

void
soft_pwm_start(soft_pwm_t* pwm)
{
  if(pwm->running == true)
  {
    return;
  }

  pwm->duty_state = soft_pwm_duty_state_on;

  gpio_set(pwm->pin, true);
  mainloop_timer_schedule(&pwm->tmr, pwm->on_time);
  pwm->running = true;
}

void
soft_pwm_stop(soft_pwm_t* pwm)
{
  if(pwm->running == false)
  {
    return;
  }

  gpio_set(pwm->pin, false);
  mainloop_timer_cancel(&pwm->tmr);
  pwm->running = false;
}

void
soft_pwm_change(soft_pwm_t* pwm, float freq, uint8_t pct)
{
  soft_pwm_calc_on_time(pwm, freq, pct);
}
