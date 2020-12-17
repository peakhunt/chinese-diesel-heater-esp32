#include "oil_pump.h"
#include "settings.h"

static uint8_t
oil_pump_calc_duty_pct(oil_pump_t* op)
{
  float  width = 1000.0f / op->freq;
  uint8_t pct = (uint8_t)((settings_get()->oil_pump_pulse_length / width) * 100);

  return pct;
}

void
oil_pump_init(oil_pump_t* op, gpio_out_pin_t pin)
{
  op->on      = false;
  op->freq    = settings_get()->oil_pump_startup_freq;

  soft_pwm_init(&op->pwm, pin, op->freq, oil_pump_calc_duty_pct(op));
}

void
oil_pump_on(oil_pump_t* op)
{
  if(op->on == true)
  {
    return;
  }

  op->on = true;
  soft_pwm_start(&op->pwm);
}

void
oil_pump_off(oil_pump_t* op)
{
  if(op->on == false)
  {
    return;
  }

  op->on = false;
  soft_pwm_stop(&op->pwm);
}

void
oil_pump_change_freq(oil_pump_t* op, float freq)
{
  op->freq    = freq;

  soft_pwm_change(&op->pwm, freq, oil_pump_calc_duty_pct(op));
}
