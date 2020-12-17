#include "nvs_flash.h"

#include "settings.h"
#include "heater.h"
#include "glow_plug.h"
#include "oil_pump.h"

#define SETTINGS_MAGIC          0x1122334f

static settings_t   _settings;

void
settings_init(void)
{
  nvs_handle  nvs_handle;
  esp_err_t   err;
  size_t      len = sizeof(_settings);

  err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if(err != ESP_OK)
  {
    goto invalid;
  }

  err = nvs_get_blob(nvs_handle, "heater", &_settings, &len);
  nvs_close(nvs_handle);

  if(err != ESP_OK || len != sizeof(_settings))
  {
    goto invalid;
  }

  if(_settings.magic != SETTINGS_MAGIC)
  {
    goto invalid;
  }

  return;

invalid:
  settings_reset();
}

void
settings_reset(void)
{
  _settings.magic                             = SETTINGS_MAGIC;
  _settings.glow_plug_on_duration_for_start   = HEATER_HEATING_GLOW_PLUG_BEFORE_OIL_PUMP_PRIMING;
  _settings.oil_pump_priming_duration         = HEATER_OIL_PUMP_PRIMING_BEFORE_GLOW_PLUG_TURN_OFF;
  _settings.glow_plug_on_duration_for_stop    = HEATER_HEATING_GLOW_PLUG_BEFORE_COOLING_DOWN;
  _settings.cooling_down_period               = HEATER_COOLING_DOWN_AFTER_GLOW_PLUG_TURN_OFF;

  _settings.startup_fan_power                 = HEATER_STARTUP_FAN_POWER;
  _settings.stop_fan_power                    = HEATER_STOP_FAN_POWER;

  _settings.glow_plug_pwm_freq                = GLOW_PLUG_PWM_FREQ;
  _settings.glow_plug_pwm_duty                = GLOW_PLUG_PWM_DUTY;

  _settings.oil_pump_startup_freq             = OIL_PUMP_DEF_FREQ;
  _settings.oil_pump_pulse_length             = OIL_PUMP_PULSE_LENGTH;

  // optimal settings I found after some experiment
  _settings.steps[0].pump_freq  = 1.0f;
  _settings.steps[0].fan_pwr    = 45;

  _settings.steps[1].pump_freq  = 1.8f;
  _settings.steps[1].fan_pwr    = 50;

  _settings.steps[2].pump_freq  = 2.5f;
  _settings.steps[2].fan_pwr    = 60;

  _settings.steps[3].pump_freq  = 3.5f;
  _settings.steps[3].fan_pwr    = 70;

  _settings.steps[4].pump_freq  = 4.5f;
  _settings.steps[4].fan_pwr    = 75;

  settings_update();
}

void
settings_update(void)
{
  nvs_handle  nvs_handle;

  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) != ESP_OK)
  {
    // something wrong. bug!
    return;
  }

  nvs_set_blob(nvs_handle, "heater", &_settings, sizeof(_settings));

  nvs_close(nvs_handle);
}

settings_t*
settings_get(void)
{
  return &_settings;
}
