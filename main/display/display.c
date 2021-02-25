#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "esp_log.h"

#include "ssd1306.h"

#include "app_heater.h"
#include "version.h"
#include "utilities.h"
#include "display.h"

typedef enum
{
  display_mode_welcome,
  display_mode_status,
  display_mode_glow_plug,
  display_mode_oil_pump,
  display_mode_fan,
  display_mode_dialog,
} display_mode_t;

typedef enum
{
  display_input_mode,
  display_input_select,
  display_input_up,
  display_input_down,
} display_input_t;

typedef void (*display_handler)(void);
typedef void (*input_handler)(display_input_t);
typedef void (*dialog_confirm)(void);

#define WELCOME_INTERVAL          2000
#define STATUS_UPDATE_INTERVAL    1000

static void welcome_display(void);
static void welcome_input(display_input_t input);
static void status_display(void);
static void status_input(display_input_t input);
static void glow_plug_display(void);
static void glow_plug_input(display_input_t input);
static void oil_pump_display(void);
static void oil_pump_input(display_input_t input);
static void fan_display(void);
static void fan_input(display_input_t input);
static void dialog_display(void);
static void dialog_input(display_input_t input);

static void display_switch(display_mode_t mode);
static void display_refresh(void);
static void popup_dialog(const char* msg, dialog_confirm confirm);

static const char* TAG = "display";

static display_mode_t   _mode = display_mode_welcome;
static dialog_confirm   _dialog_confirm = NULL;

static display_handler  _display_handlers[] = 
{
  welcome_display,
  status_display,
  glow_plug_display,
  oil_pump_display,
  fan_display,
  dialog_display,
};

static input_handler _input_handlers[] =
{
  welcome_input,
  status_input,
  glow_plug_input,
  oil_pump_input,
  fan_input,
  dialog_input,
};

static const char* on_off_str[] = 
{
  "off",
  "on",
};

static const char*  _dialog_msg = NULL;
static bool         _dialog_sel = false;
static display_mode_t   _prev_mode;
static app_heater_status_rsp_t    _r;

static QueueHandle_t      _event_q;

/////////////////////////////////////////////////////////////////////////////////////////
//
// welcome state
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
welcome_display(void)
{
  ssd1306_printf(30, 20, Font_7x10, ssd1306_color_white, "Chinese");
  ssd1306_printf(32, 30, Font_7x10, ssd1306_color_white, "Diesel");
  ssd1306_printf(32, 40, Font_7x10, ssd1306_color_white, "Heater");
  ssd1306_printf(32, 50, Font_7x10, ssd1306_color_white, "%s", VERSION);
}

static void
welcome_input(display_input_t input)
{
  //
  // nothing to do
  //
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// status state
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
status_dialog_confirm(void)
{
	app_heater_status(&_r);

  if (_r.state == heater_state_off)
  {
    app_heater_start();
  }
  else if(_r.state == heater_state_running)
  {
    app_heater_stop();
  }

  display_switch(display_mode_status);
}

static void
status_display(void)
{
  static const char* heater_state_str[] =
  {
    "off",
    "sglow",
    "priming",
    "running",
    "eglow",
    "cooling",
  };

	app_heater_status(&_r);

  ssd1306_printf(5, 3, Font_7x10, ssd1306_color_white,  "State: %s", heater_state_str[_r.state]);
  ssd1306_printf(5, 12, Font_7x10, ssd1306_color_white, "Pump : %s %.1fHz", on_off_str[_r.oil_on], _r.oil_freq);
  ssd1306_printf(5, 22, Font_7x10, ssd1306_color_white, "Glow : %s", on_off_str[_r.glow_on]);
  ssd1306_printf(5, 32, Font_7x10, ssd1306_color_white, "Fan  : %s %d%%", on_off_str[_r.fan_on], _r.fan_pwr);
  ssd1306_printf(5, 42, Font_7x10, ssd1306_color_white, "OT   : %.1fC", _r.outlet_temp);
  ssd1306_printf(5, 52, Font_7x10, ssd1306_color_white, "Room : %.1fC", _r.room_temp);
}

static void
status_input(display_input_t input)
{
	app_heater_status(&_r);

  switch(input)
  {
  case display_input_mode:
    display_switch(display_mode_glow_plug);
    break;

  case display_input_select:
    if (_r.state == heater_state_off)
    {
      popup_dialog("Start Heater?", status_dialog_confirm);
    }
    else if(_r.state == heater_state_running)
    {
      popup_dialog("Stop Heater?", status_dialog_confirm);
    }
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// glow plug state
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
glow_plug_confirm(void)
{
	app_heater_status(&_r);

  if(_r.glow_on)
  {
    app_heater_control_glow(false);
  }
  else
  {
    app_heater_control_glow(true);
  }

  display_switch(display_mode_glow_plug);
}

static void
glow_plug_display(void)
{
	app_heater_status(&_r);

  ssd1306_printf(5, 5, Font_7x10, ssd1306_color_white, "Glow Plug");
  ssd1306_printf(5, 15, Font_7x10, ssd1306_color_white, "State: %s", on_off_str[_r.glow_on]);
}

static void
glow_plug_input(display_input_t input)
{
	app_heater_status(&_r);

  switch(input)
  {
  case display_input_mode:
    display_switch(display_mode_oil_pump);
    break;

  case display_input_select:
    if(_r.glow_on)
    {
      popup_dialog("Turn Off Glow?", glow_plug_confirm);
    }
    else
    {
      popup_dialog("Turn On Glow?", glow_plug_confirm);
    }
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// oil pump state
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
oil_pump_confirm(void)
{
	app_heater_status(&_r);

  if(_r.oil_on)
  {
    heater_oil_pump_off();
  }
  else
  {
    heater_oil_pump_on();
  }

  display_switch(display_mode_oil_pump);
}

static void
oil_pump_display(void)
{
	app_heater_status(&_r);

  ssd1306_printf(5, 5, Font_7x10, ssd1306_color_white, "Oil Pump");
  ssd1306_printf(5, 15, Font_7x10, ssd1306_color_white, "State: %s", on_off_str[_r.oil_on]);
  ssd1306_printf(5, 25, Font_7x10, ssd1306_color_white, "Freq : %.1fHz", _r.oil_freq);
}

static void
oil_pump_input(display_input_t input)
{
	app_heater_status(&_r);
  float freq = _r.oil_freq;

  switch(input)
  {
  case display_input_mode:
    display_switch(display_mode_fan);
    break;

  case display_input_select:
    if(_r.oil_on)
    {
      popup_dialog("Turn Off Pump?", oil_pump_confirm);
    }
    else
    {
      popup_dialog("Turn On Pump?", oil_pump_confirm);
    }
    break;

  case display_input_up:
    freq += 0.1f;
    if(fcompare(freq, OIL_PUMP_MAX_FREQ) <= 0)
    {
      app_heater_control_oil_pump_freq(freq);
      display_refresh();
    }
    break;

  case display_input_down:
    freq -= 0.1f;
    if(fcompare(freq, OIL_PUMP_MIN_FREQ) >= 0)
    {
      app_heater_control_oil_pump_freq(freq);
      display_refresh();
    }
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// fan state
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
fan_confirm(void)
{
	app_heater_status(&_r);

  if(_r.fan_on)
  {
    app_heater_control_fan(false);
    heater_fan_off();
  }
  else
  {
    app_heater_control_fan(true);
  }

  display_switch(display_mode_fan);
}
static void
fan_display(void)
{
	app_heater_status(&_r);

  ssd1306_printf(5, 5, Font_7x10, ssd1306_color_white, "Fan");
  ssd1306_printf(5, 15, Font_7x10, ssd1306_color_white, "State: %s", on_off_str[_r.fan_on]);
  ssd1306_printf(5, 25, Font_7x10, ssd1306_color_white, "Power: %d%%", _r.fan_pwr);
}

static void
fan_input(display_input_t input)
{
	app_heater_status(&_r);
  uint8_t pwr = _r.fan_pwr;

  switch(input)
  {
  case display_input_mode:
    display_switch(display_mode_status);
    break;

  case display_input_select:
    if(_r.fan_on)
    {
      popup_dialog("Turn Off Fan?", fan_confirm);
    }
    else
    {
      popup_dialog("Turn On Fan?", fan_confirm);
    }
    break;

  case display_input_up:
    if(pwr < 100)
    {
      pwr++;
      app_heater_control_fan_power(pwr);
      display_refresh();
    }

    break;

  case display_input_down:
    if(pwr > 0)
    {
      pwr--;
      app_heater_control_fan_power(pwr);
      display_refresh();
    }
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// dialog state
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
popup_dialog(const char* msg, dialog_confirm confirm)
{
  _dialog_msg = msg;
  _dialog_sel = false;
  _dialog_confirm = confirm;
  _prev_mode = _mode;

  display_switch(display_mode_dialog);
}

static void
dialog_display(void)
{
  ssd1306_printf(10, 22, Font_7x10, ssd1306_color_white, "%s", _dialog_msg);

  ssd1306_printf(22, 42, Font_7x10, ssd1306_color_white, "Yes");
  ssd1306_printf(87, 42, Font_7x10, ssd1306_color_white, "No");

  if (_dialog_sel)
  {
    ssd1306_draw_rectangle(19, 39, 45, 54, ssd1306_color_white);
  }
  else
  {
    ssd1306_draw_rectangle(84, 39, 103, 54, ssd1306_color_white);
  }
}

static void
dialog_input(display_input_t input)
{
  switch(input)
  {
  case display_input_select:
    if(_dialog_sel)
    {
      _dialog_confirm();
    }
    else
    {
      display_switch(_prev_mode);
    }
    break;

  case display_input_up:
  case display_input_down:
    _dialog_sel = !_dialog_sel;
    display_refresh();
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// utilities
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
display_refresh(void)
{
  ssd1306_fill(ssd1306_color_black);

  _display_handlers[_mode]();

  ssd1306_update_screen();
}

static void
display_switch(display_mode_t mode)
{
  _mode = mode;
  display_refresh();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// task
//
/////////////////////////////////////////////////////////////////////////////////////////
static void
display_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting display task");

  ssd1306_init();

  display_refresh();
  vTaskDelay(WELCOME_INTERVAL / portTICK_RATE_MS);
  display_switch(display_mode_status);

  while(1)
  {
    display_event_t   evt;

    if(xQueueReceive(_event_q, (void*)&evt, STATUS_UPDATE_INTERVAL / portTICK_RATE_MS) == pdTRUE)
    {
      // FIXME
      // handle event
    }

    display_refresh();
    (void)_input_handlers;
  }
}

void
display_init(void)
{
  _event_q = xQueueCreate(8, sizeof(display_event_t));
  xTaskCreate(display_task, "display", 4096, NULL, 3, NULL);
}

void
display_feed_event(display_event_t e)
{
  xQueueSend(_event_q, (void*)&e, portMAX_DELAY);
}
