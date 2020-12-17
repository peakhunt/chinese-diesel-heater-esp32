#ifndef __GLOW_PLUG_DEF_H__
#define __GLOW_PLUG_DEF_H__

#include <stdint.h>
#include "gpio.h"
#include "soft_pwm.h"

#define GLOW_PLUG_PWM_FREQ            10      // 10 Hz
#define GLOW_PLUG_PWM_DUTY            35      // 35ms duty out of 100ms

#define GLOW_PLUG_MIN_DUTY            10
#define GLOW_PLUG_MAX_DUTY            80

typedef struct
{
  bool                    on;
  soft_pwm_t              pwm;
} glow_plug_t;

extern void glow_plug_init(glow_plug_t* gp, gpio_out_pin_t pin);
extern void glow_plug_on(glow_plug_t* gp);
extern void glow_plug_off(glow_plug_t* gp);

#endif /* !__GLOW_PLUG_DEF_H__ */
