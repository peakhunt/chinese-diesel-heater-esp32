#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "list.h"
#include "shell.h"
#include "io_driver.h"
#include "esp_log.h"

#include "app_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cli.h"

#include "esp_system.h"
#include "esp_spi_flash.h"

#include "app_heater.h"

////////////////////////////////////////////////////////////////////////////////
//
// private prototypes
//
////////////////////////////////////////////////////////////////////////////////
static void cli_command_sysinfo(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_systime(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_restart(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_gpio_out(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_gpio_in(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_pwm(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_adc(cli_intf_t* intf, int argc, const char** argv);

static void cli_command_glow(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_oil(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_fan(cli_intf_t* intf, int argc, const char** argv);

static void cli_command_set_oil_pump_freq(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_set_fan_power(cli_intf_t* intf, int argc, const char** argv);

static void cli_command_start(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_stop(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_status(cli_intf_t* intf, int argc, const char** argv);

static void cli_command_settings(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_save(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_reset(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_mod(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_step(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_set(cli_intf_t* intf, int argc, const char** argv);

static const char* TAG   = "shell";

static io_driver_t      _io_driver;


static cli_command_t    _app_commands[] =
{
  {
    "sysinfo",
    "show system info",
    cli_command_sysinfo,
  },
  {
    "systime",
    "show system uptime",
    cli_command_systime,
  },
  {
    "restart",
    "restart the chip",
    cli_command_restart,
  },
  {
    "gpio_out",
    "control gpio out",
    cli_command_gpio_out,
  },
  {
    "gpio_in",
    "show gpio in",
    cli_command_gpio_in,
  },
  {
    "pwm",
    "control pwm",
    cli_command_pwm,
  },
  {
    "adc",
    "show adc",
    cli_command_adc,
  },
  {
    "glow",
    "control glow plug",
    cli_command_glow,
  },
  {
    "oil",
    "control oil pump",
    cli_command_oil,
  },
  {
    "fan",
    "control fan",
    cli_command_fan,
  },
  {
    "pump_freq",
    "change oil pump frequency",
    cli_command_set_oil_pump_freq,
  },
  {
    "fan_power",
    "change fan power",
    cli_command_set_fan_power,
  },
  {
    "start",
    "start heater",
    cli_command_start,
  },
  {
    "stop",
    "stop heater",
    cli_command_stop,
  },
  {
    "status",
    "show heater status",
    cli_command_status,
  },
  {
    "settings",
    "show settings",
    cli_command_settings,
  },
  {
    "save",
    "save settings",
    cli_command_save,
  },
  {
    "reset",
    "reset settings",
    cli_command_reset,
  },
  {
    "mod",
    "modify settings",
    cli_command_mod,
  },
  {
    "step",
    "modify step parameters",
    cli_command_step,
  },
  {
    "set",
    "change control step",
    cli_command_set,
  },
};

static inline int
fcompare(float a, float b)
{
  //
  // returns -1 when a < b
  //          1 when a > b
  //          0 when equal
  //
#define FLT_EPSILON 0.000001f

  float d = a - b;

  if (d >= -FLT_EPSILON && d <= FLT_EPSILON)
  {
    return 0;
  }

  if(d < -FLT_EPSILON)
  {
    // a is less than b
    return -1;
  }

  // a is bigger than b
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
// application specific CLI commands implementation
//
////////////////////////////////////////////////////////////////////////////////
static void
cli_command_sysinfo(cli_intf_t* intf, int argc, const char** argv)
{
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  cli_printf(intf, CLI_EOL);
  cli_printf(intf, "This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
      chip_info.cores,
      (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
      (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  cli_printf(intf, "silicon revision %d, ", chip_info.revision);
  cli_printf(intf, "%dMB %s flash"CLI_EOL, spi_flash_get_chip_size() / (1024 * 1024),
      (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

static void
cli_command_systime(cli_intf_t* intf, int argc, const char** argv)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);

  cli_printf(intf, CLI_EOL);
  cli_printf(intf, "uptime: %d"CLI_EOL, tv.tv_sec);
}

static void
cli_command_restart(cli_intf_t* intf, int argc, const char** argv)
{
  esp_restart();
}

static void
cli_command_gpio_out(cli_intf_t* intf, int argc, const char** argv)
{
  static app_heater_gpio_out_req_t   r;

  cli_printf(intf, CLI_EOL);

  if(argc != 3)
  {
    cli_printf(intf, "invalid %s <pin> <value>"CLI_EOL, argv[0]);
    return;
  }

  r.pin = atoi(argv[1]);
  r.v = atoi(argv[2]);

  if(r.pin >= GPIO_MAX_OUTPUT)
  {
    cli_printf(intf, "Invalid pin %d"CLI_EOL, r.pin);
    return;
  }

  app_heater_request_gpio_out(&r);
  cli_printf(intf, "set gpio out %d to %d"CLI_EOL, r.pin, r.v);
}

static void
cli_command_gpio_in(cli_intf_t* intf, int argc, const char** argv)
{
  static app_heater_gpio_in_rsp_t r;
  static const char* pin_states[] =
  {
    "low",
    "low to high",
    "high",
    "high to low",
  };

  app_heater_request_gpio_in(&r);

  cli_printf(intf, CLI_EOL);

  for(uint8_t i = 0; i < GPIO_MAX_INPUT; i++)
  {
    cli_printf(intf, "gpio in %d - %d, %s"CLI_EOL,
        i,
        r.v[i],
        pin_states[r.s[i]]
    );
  }
}

static void
cli_command_pwm(cli_intf_t* intf, int argc, const char** argv)
{
  static app_heater_pwm_req_t     r;

  cli_printf(intf, CLI_EOL);

  if(argc != 3)
  {
    cli_printf(intf, "invalid %s <chnl> <percent>"CLI_EOL, argv[0]);
    return;
  }

  r.chnl = (pwm_channel_t)atoi(argv[1]);
  r.pct = atoi(argv[2]);

  if(r.chnl < 0 || r.chnl >= PWM_MAX_CHANNEL)
  {
    cli_printf(intf, "Invalid channel %d"CLI_EOL, r.chnl);
    return;
  }

  if(r.pct > 100)
  {
    cli_printf(intf, "Invalid percent %d"CLI_EOL, r.pct);
    return;
  }

  app_heater_request_pwm(&r);

  cli_printf(intf, "set pwm channel %d to %d percent"CLI_EOL, r.chnl, r.pct);
}

static void
cli_command_adc(cli_intf_t* intf, int argc, const char** argv)
{
  static app_heater_adc_rsp_t r;

  app_heater_request_adc(&r);

  cli_printf(intf, CLI_EOL);

  for(uint8_t i = 0; i < ADC_MAX_CHANNELS; i++)
  {
    cli_printf(intf, "adc %d - %d, %.2f"CLI_EOL,
        i,
        r.samples[i],
        r.volt[i]
    );
  }
}

static void
cli_command_glow(cli_intf_t* intf, int argc, const char** argv)
{
  cli_printf(intf, CLI_EOL);

  if(argc != 2)
  {
    cli_printf(intf, "invalid %s on|off"CLI_EOL, argv[0]);
    return;
  }

  if(strcmp(argv[1], "on") == 0)
  {
    app_heater_control_glow(true);
  }
  else if(strcmp(argv[1], "off") == 0)
  {
    app_heater_control_glow(false);
  }
  else
  {
    cli_printf(intf, "invalid %s on|off"CLI_EOL, argv[0]);
    return;
  }
  cli_printf(intf, "turned %s glow plug"CLI_EOL, argv[1]);
}

static void
cli_command_oil(cli_intf_t* intf, int argc, const char** argv)
{
	cli_printf(intf, CLI_EOL);

	if(argc != 2)
	{
		cli_printf(intf, "invalid %s on|off"CLI_EOL, argv[0]);
		return;
	}

	if(strcmp(argv[1], "on") == 0)
	{
    app_heater_control_oil_pump(true);
	}
	else if(strcmp(argv[1], "off") == 0)
	{
    app_heater_control_oil_pump(false);
	}
	else
	{
		cli_printf(intf, "invalid %s on|off"CLI_EOL, argv[0]);
		return;
	}
	cli_printf(intf, "turned %s oil pump"CLI_EOL, argv[1]);
}

static void
cli_command_fan(cli_intf_t* intf, int argc, const char** argv)
{
  cli_printf(intf, CLI_EOL);

  if(argc != 2)
  {
    cli_printf(intf, "invalid %s on|off"CLI_EOL, argv[0]);
    return;
  }

  if(strcmp(argv[1], "on") == 0)
  {
    app_heater_control_fan(true);
  }
  else if(strcmp(argv[1], "off") == 0)
  {
    app_heater_control_fan(false);
  }
  else
  {
    cli_printf(intf, "invalid %s on|off"CLI_EOL, argv[0]);
    return;
  }
  cli_printf(intf, "turned %s fan"CLI_EOL, argv[1]);
}

static void
cli_command_set_oil_pump_freq(cli_intf_t* intf, int argc, const char** argv)
{
  float freq;

  cli_printf(intf, CLI_EOL);

  if(argc != 2)
  {
    cli_printf(intf, "invalid %s <freq>"CLI_EOL, argv[0]);
    return;
  }

  freq = atof(argv[1]);

  if(fcompare(freq, OIL_PUMP_MIN_FREQ) < 0 || fcompare(freq, OIL_PUMP_MAX_FREQ) > 0)
  {
    cli_printf(intf, "Invalid frequency"CLI_EOL);
    return;
  }

  app_heater_control_oil_pump_freq(freq);
}

static void
cli_command_set_fan_power(cli_intf_t* intf, int argc, const char** argv)
{
  uint8_t pwr;

  cli_printf(intf, CLI_EOL);

  if(argc != 2)
  {
    cli_printf(intf, "invalid %s <pwr>"CLI_EOL, argv[0]);
    return;
  }

  pwr = atoi(argv[1]);

  if(pwr > 100)
  {
    cli_printf(intf, "invalid: should be between 0 and 100"CLI_EOL);
    return;
  }

  app_heater_control_fan_power(pwr);

  cli_printf(intf, "set power to %d"CLI_EOL, pwr);
}

static void
cli_command_start(cli_intf_t* intf, int argc, const char** argv)
{
  app_heater_start();
}

static void
cli_command_stop(cli_intf_t* intf, int argc, const char** argv)
{
  app_heater_stop();
}

static void
cli_command_status(cli_intf_t* intf, int argc, const char** argv)
{
	static const char* motor_state_str[] =
	{
		"not rotating",
		"starting",
		"rotating",
	};

	static const char*
  heater_state_desc[] =
  {
    "off",
    "glowing for start",
    "priming",
    "running",
    "glowing for stop",
    "cooling",
  };

	static const char*
  on_off_str[] =
  {
    "off",
    "on",
  };
	static app_heater_status_rsp_t    r;

	app_heater_status(&r);

	cli_printf(intf, CLI_EOL);

	cli_printf(intf, "state : %s"CLI_EOL, heater_state_desc[r.state]);
	cli_printf(intf, "pump  : %s, freq %.1f Hz"CLI_EOL,
			on_off_str[r.oil_on],
      r.oil_freq);
	cli_printf(intf, "glow  : %s"CLI_EOL, on_off_str[r.glow_on]);
	cli_printf(intf, "fan   : %s, %s, power %d%%"CLI_EOL,
			on_off_str[r.fan_on],
			motor_state_str[r.fan_motor_state],
			r.fan_pwr);
	cli_printf(intf, "step  : %d"CLI_EOL, r.step);

  cli_printf(intf, "outlet : %.1f C"CLI_EOL, r.outlet_temp);
  cli_printf(intf, "room   : %.1f C"CLI_EOL, r.room_temp);
}

static void
cli_command_settings(cli_intf_t* intf, int argc, const char** argv)
{
	static settings_t s;

	app_heater_get_settings(&s);

	cli_printf(intf, CLI_EOL);

	cli_printf(intf, "0. glow plug on duration for start %ld sec"CLI_EOL, s.glow_plug_on_duration_for_start / 1000);
	cli_printf(intf, "1. oil pump priming duration       %ld sec"CLI_EOL, s.oil_pump_priming_duration / 1000);
	cli_printf(intf, "2. glow plug on duration for stop  %ld sec"CLI_EOL, s.glow_plug_on_duration_for_stop / 1000);
	cli_printf(intf, "3. cooling down period             %ld sec"CLI_EOL, s.cooling_down_period / 1000);
	cli_printf(intf, "4. start-up fan power              %d %%"CLI_EOL, s.startup_fan_power);
	cli_printf(intf, "5. stop fan power                  %d %%"CLI_EOL, s.stop_fan_power);
	cli_printf(intf, "6. glow plug PWM frequency         %d Hz"CLI_EOL, s.glow_plug_pwm_freq);
	cli_printf(intf, "7. glow plug PWM duty              %d %%"CLI_EOL, s.glow_plug_pwm_duty);

	cli_printf(intf, "8. oil pump startup frequency      %.1f Hz"CLI_EOL, s.oil_pump_startup_freq);
	cli_printf(intf, "9. oil pump pulse length           %d ms"CLI_EOL, s.oil_pump_pulse_length);

	cli_printf(intf, CLI_EOL);
	for(uint8_t i = 0; i < MAX_OIL_PUMP_FAN_STEPS; i++)
	{
    cli_printf(intf, "step %d, oil pump freq %.1f Hz, Fan %d%%"CLI_EOL,
				i,
        s.steps[i].pump_freq,
				s.steps[i].fan_pwr);
	}
}

static void
cli_command_save(cli_intf_t* intf, int argc, const char** argv)
{
  cli_printf(intf, CLI_EOL"saving changes to EEPROM..."CLI_EOL);
  app_heater_save();
  cli_printf(intf, "done saving changes to EEPROM..."CLI_EOL);
}

static void
cli_command_reset(cli_intf_t* intf, int argc, const char** argv)
{
  cli_printf(intf, CLI_EOL"resettinging settings..."CLI_EOL);
  app_heater_reset();
  cli_printf(intf, "done resetting settings..."CLI_EOL);
}

static void
cli_command_mod(cli_intf_t* intf, int argc, const char** argv)
{
  uint8_t       num;
  uint32_t      iv;
  float         fv;
  static app_heater_settings_req_t    r;

  cli_printf(intf, CLI_EOL);

  if(argc != 3)
  {
    cli_printf(intf, "invalid %s <setting number> <value>"CLI_EOL, argv[0]);
    return;
  }

  num = atoi(argv[1]);
  if(num > 9)
  {
    cli_printf(intf, "invalid: setting number should be between 0 and 10"CLI_EOL);
    return;
  }

  r.item = num;

  switch(num)
  {
  case 0: // glow plug on duration for start
    iv = atol(argv[2]);
    if(iv < 30 || iv > 150)
    {
      cli_printf(intf, "invalid: value should be between  30 and 150"CLI_EOL);
      return;
    }
    r.u32 = iv * 1000;
    break;

  case 1: // oil pump priming duration
    iv = atol(argv[2]);
    if(iv < 5 || iv > 60)
    {
      cli_printf(intf, "invalid: value should be between  5 and 60"CLI_EOL);
      return;
    }
    r.u32 = iv * 1000;
    break;

  case 2: // glow plug on duration for stop
    iv = atol(argv[2]);
    if(iv < 20 || iv > 60)
    {
      cli_printf(intf, "invalid: value should be between  20 and 60"CLI_EOL);
      return;
    }
    r.u32 = iv * 1000;
    break;

  case 3: // cooling down period
    iv = atol(argv[2]);
    if(iv < 60 || iv > 180)
    {
      cli_printf(intf, "invalid: value should be between  60 and 180"CLI_EOL);
      return;
    }
    r.u32 = iv * 1000;
    break;

  case 4: // start up fan power
    iv = atol(argv[2]);
    if(iv < 10 || iv > 100)
    {
      cli_printf(intf, "invalid: value should be between  10 and 100"CLI_EOL);
      return;
    }
    r.u8 = iv;
    break;

  case 5: // stop fan power
    iv = atol(argv[2]);
    if(iv < 10 || iv > 100)
    {
      cli_printf(intf, "invalid: value should be between  10 and 100"CLI_EOL);
      return;
    }
    r.u8 = iv;
    break;

  case 6: // glow plug PWM frequency
    iv = atol(argv[2]);
    if(iv < 2 || iv > 20)
    {
      cli_printf(intf, "invalid: value should be between 2 and 20"CLI_EOL);
      return;
    }
    r.u8 = iv;
    break;

  case 7: // glow plug PWM duty
    iv = atol(argv[2]);
    if(iv < 5 || iv > 95)
    {
      cli_printf(intf, "invalid: value should be between 5 and 95"CLI_EOL);
      return;
    }
    r.u8 = iv;
    break;

  case 8: // oil pump frequency
    fv = atof(argv[2]);
    if(fcompare(fv, OIL_PUMP_MIN_FREQ) < 0 || fcompare(fv, OIL_PUMP_MAX_FREQ) > 0)
    {
      cli_printf(intf, "invalid: value should be between 0.8 and 5.0"CLI_EOL);
      return;
    }
    r.f = fv;
    break;

  case 9: // oil pump pulse length
    iv = atol(argv[2]);
    if(iv < 10 || iv > 100)
    {
      cli_printf(intf, "invalid: value should be between  10 and 100"CLI_EOL);
      return;
    }
    r.u8 = iv;
    break;

  default:
    return;
  }

  app_heater_mod_settings(&r);
  cli_printf(intf, "done changing setting #%d. Be sure to save"CLI_EOL, num);
}

static void
cli_command_step(cli_intf_t* intf, int argc, const char** argv)
{
  float   freq;
  uint8_t pwr;
  uint8_t step;
  static app_heater_step_req_t    r;

  cli_printf(intf, ""CLI_EOL);

  if(argc != 4)
  {
    cli_printf(intf, "invalid %s <step number> <pump freq> <fan pwr>"CLI_EOL, argv[0]);
    return;
  }

  step = atoi(argv[1]);
  if(step >= MAX_OIL_PUMP_FAN_STEPS)
  {
    cli_printf(intf, "invalid step number"CLI_EOL);
    return;
  }

  freq = atof(argv[2]);
  if(fcompare(freq, OIL_PUMP_MIN_FREQ) < 0 || fcompare(freq, OIL_PUMP_MAX_FREQ) > 0)
  {
    cli_printf(intf, "Invalid frequency"CLI_EOL);
    return;
  }

  pwr = atoi(argv[3]);
  if(pwr > 100)
  {
    cli_printf(intf, "Invalid power"CLI_EOL);
    return;
  }

  r.ndx = step;
  r.pwr = pwr;
  r.freq = freq;

  app_heater_step(&r);

  cli_printf(intf, "changed settings of step #%d"CLI_EOL, step);
}

static void
cli_command_set(cli_intf_t* intf, int argc, const char** argv)
{
  uint8_t step;

  cli_printf(intf, ""CLI_EOL);

  if(argc != 2)
  {
    cli_printf(intf, "invalid %s <step number>"CLI_EOL, argv[0]);
    return;
  }

  step = atoi(argv[1]);
  if(step >= MAX_OIL_PUMP_FAN_STEPS)
  {
    cli_printf(intf, "invalid step number"CLI_EOL);
    return;
  }

  app_heater_set(step);

  cli_printf(intf, "changed current step to %d"CLI_EOL, step);
}

////////////////////////////////////////////////////////////////////////////////
//
// global singleton for CLI io driver
//
////////////////////////////////////////////////////////////////////////////////
io_driver_t*
cli_io_driver(void)
{
  return &_io_driver;
}

////////////////////////////////////////////////////////////////////////////////
//
// shell task. a simple I/O driver.
//
////////////////////////////////////////////////////////////////////////////////
static void
shell_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting shell task");

  io_driver_init(&_io_driver);

  cli_init(_app_commands, sizeof(_app_commands)/sizeof(cli_command_t), 7000);

  while(1)
  {
    io_driver_run(&_io_driver);
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
shell_init(void)
{
  ESP_LOGI(TAG, "initializing shell");

  xTaskCreate(&shell_task, "shell_task", 4096, NULL, 3, NULL);
}
