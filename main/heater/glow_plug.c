#include "glow_plug.h"
#include "settings.h"

void
glow_plug_init(glow_plug_t* gp, gpio_out_pin_t pin)
{
  gp->on        = false;

  soft_pwm_init(&gp->pwm, pin,
      settings_get()->glow_plug_pwm_freq,
      settings_get()->glow_plug_pwm_duty);
}

void
glow_plug_on(glow_plug_t* gp)
{
  if(gp->on == true)
  {
    return;
  }

  gp->on = true;
  soft_pwm_start(&gp->pwm);
}

void
glow_plug_off(glow_plug_t* gp)
{
  if(gp->on == false)
  {
    return;
  }

  gp->on = false;
  soft_pwm_stop(&gp->pwm);
}
