#ifndef __SETTINGS_DEF_H__
#define __SETTINGS_DEF_H__

#include <stdint.h>

#define MAX_OIL_PUMP_FAN_STEPS      5
typedef struct
{
  float       pump_freq;
  uint8_t     fan_pwr;
} oil_pump_fan_step_t;

typedef struct
{
  uint32_t        magic;

  uint32_t        glow_plug_on_duration_for_start; 
  uint32_t        oil_pump_priming_duration;
  uint32_t        glow_plug_on_duration_for_stop;
  uint32_t        cooling_down_period;

  uint8_t         startup_fan_power;
  uint8_t         stop_fan_power;

  uint8_t         glow_plug_pwm_freq;
  uint8_t         glow_plug_pwm_duty;

  float           oil_pump_startup_freq;
  uint8_t         oil_pump_pulse_length;

  oil_pump_fan_step_t steps[MAX_OIL_PUMP_FAN_STEPS];
} settings_t;

extern void settings_init(void);
extern void settings_reset(void);
extern void settings_update(void);
extern settings_t* settings_get(void);

#endif /* !__SETTINGS_DEF_H__ */
