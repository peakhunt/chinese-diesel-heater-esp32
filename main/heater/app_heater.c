#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "esp_log.h"

#include "app_heater.h"
#include "shell.h"
#include "event_dispatcher.h"
#include "event_list.h"
#include "mainloop_timer.h"
#include "misc.h"

#include "display.h"

static const char* TAG = "heater";
static TimerHandle_t      _tick_tmr;
static QueueHandle_t      _heater_q;

//
// for response
//
static SemaphoreHandle_t  _rsp_q_mutex;
static QueueHandle_t      _rsp_q;

static inline void
app_heater_send_msg(uint8_t msg, void* arg)
{
  app_heater_msg_t    m;

  m.msg = msg;
  m.arg = arg;

  xQueueSend(_heater_q, (void*)&m, portMAX_DELAY);
}

static inline void
app_heater_send_msg_from_isr(uint8_t msg, void* arg)
{
  app_heater_msg_t    m;
  BaseType_t xHigherPriorityTaskWoken;

  m.msg = msg;
  m.arg = arg;

  xQueueSendFromISR(_heater_q, (void*)&m, &xHigherPriorityTaskWoken);

  if(xHigherPriorityTaskWoken)
  {
    portYIELD_FROM_ISR();
  }
}

static void
app_heater_tick_tmr_callback(TimerHandle_t xTimer)
{
  app_heater_notify_tmr_tick();
}

static void
app_heater_handle_cmd(app_heater_msg_t* m)
{
  switch(m->msg)
  {
  case HEATER_MSG_TIMER_TICK:
    event_set(1 << DISPATCH_EVENT_TIMER_TICK);
    break;

  case HEATER_MSG_PIN_CHANGE:
    event_set(1 << DISPATCH_EVENT_PIN_CHANGE);
    break;

  case HEATER_MSG_GPIO_OUT_REQ:
    {
      app_heater_gpio_out_req_t*    r = (app_heater_gpio_out_req_t*)m->arg;
      gpio_set(r->pin, r->v);
    }
    break;

  case HEATER_MSG_GPIO_IN_REQ:
    {
      app_heater_gpio_in_rsp_t*   r = (app_heater_gpio_in_rsp_t*)m->arg;
      bool ret = true;

      for(uint8_t i = 0; i < GPIO_MAX_INPUT; i++)
      {
        r->v[i] = gpio_get(i);
        r->s[i] = gpio_get_state(i);
      }
      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;

  case HEATER_MSG_PWM_REQ:
    {
      app_heater_pwm_req_t*   r = (app_heater_pwm_req_t*)m->arg;

      pwm_control(r->chnl, r->pct);
    }
    break;

  case HEATER_MSG_ADC_REQ:
    {
      app_heater_adc_rsp_t*   r = (app_heater_adc_rsp_t*)m->arg;
      bool ret = true;

      for(uint8_t i = 0; i < ADC_MAX_CHANNELS; i++)
      {
        r->samples[i] = adc_get(i);
        r->volt[i] = adc_get_volt(i);
      }
      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;

  case HEATER_MSG_CONTROL_GLOW:
    if(m->b)
    {
      heater_glow_plug_on();
    }
    else
    {
      heater_glow_plug_off();
    }
    break;

  case HEATER_MSG_CONTROL_OIL:
    if(m->b)
    {
      heater_oil_pump_on();
    }
    else
    {
      heater_oil_pump_off();
    }
    break;

  case HEATER_MSG_CONTROL_FAN:
    if(m->b)
    {
      heater_fan_on();
    }
    else
    {
      heater_fan_off();
    }
    break;

  case HEATER_MSG_CONTROL_OIL_FREQ:
    heater_oil_pump_freq(m->f);
    break;

  case HEATER_MSG_CONTROL_FAN_POWER:
    heater_fan_power((uint8_t)m->i);
    break;

  case HEATER_MSG_START:
    heater_start();
    break;

  case HEATER_MSG_STOP:
    heater_stop();
    break;

  case HEATER_MSG_STATUS:
    {
      app_heater_status_rsp_t* r = (app_heater_status_rsp_t*)m->arg;
      const heater_t* heater = heater_get();
      bool ret = true;

      r->state              = heater->state;
      r->oil_on             = heater->oil_pump.on;
      r->oil_freq           = heater->oil_pump.freq;
      r->glow_on            = heater->glow_plug.on;
      r->fan_on             = heater->fan.on;
      r->fan_motor_state    = heater->fan.motor_state;
      r->fan_pwr            = heater->fan.pwr;
      r->step               = heater->step;
      r->outlet_temp        = heater->outlet_temp.temp;
      r->room_temp          = heater->room_temp.temp;
      r->flame_detected     = heater->state == heater_state_running ? true : false;     // FIXME

      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;

  case HEATER_MSG_SETTINGS_GET:
    {
      settings_t* r = (settings_t*)m->arg;
      settings_t* s = settings_get();
      bool ret = true;

      memcpy(r, s, sizeof(settings_t));
      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;

  case HEATER_MSG_SETTINGS_MOD:
    {
      app_heater_settings_req_t* r = (app_heater_settings_req_t*)m->arg;
      settings_t* s = settings_get();
      bool ret = true;

      switch(r->item)
      {
        case 0:
          s->glow_plug_on_duration_for_start = r->u32;
          break;
        case 1:
          s->oil_pump_priming_duration = r->u32;
          break;
        case 2:
          s->glow_plug_on_duration_for_stop = r->u32;
          break;
        case 3:
          s->cooling_down_period = r->u32;
          break;
        case 4:
          s->startup_fan_power = r->u8;
          break;
        case 5:
          s->stop_fan_power = r->u8;
          break;
        case 6:
          s->glow_plug_pwm_freq = r->u8;
          break;
        case 7:
          s->glow_plug_pwm_duty = r->u8;
          break;
        case 8:
          s->oil_pump_startup_freq = r->f;
          break;
        case 9:
          s->oil_pump_pulse_length = r->u8;
          break;
        default:
          break;

      }
      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;

  case HEATER_MSG_SAVE:
    {
      bool ret = true;

      settings_update();
      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;

  case HEATER_MSG_RESET:
    {
      bool ret = true;

      settings_reset();
      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;
  case HEATER_MSG_STEP:
    {
      app_heater_step_req_t* r = (app_heater_step_req_t*)m->arg;
      settings_t* s = settings_get();
      bool ret = true;

      s->steps[r->ndx].pump_freq = r->freq;
      s->steps[r->ndx].fan_pwr = r->pwr;

      xQueueSend(_rsp_q, (void*)&ret, portMAX_DELAY);
    }
    break;

  case HEATER_MSG_SET:
    {
      heater_set_step((uint8_t)m->i);
    }
    break;
  }
}

static void
app_heater_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting heater task");

#if 0
  _heater_q = xQueueCreate(16, sizeof(app_heater_msg_t));

  _rsp_q_mutex = xSemaphoreCreateMutex();
  _rsp_q    = xQueueCreate(1, sizeof(bool));
#endif

  _tick_tmr = xTimerCreate
    (
     "tick timer",
     pdMS_TO_TICKS(APP_CONFIG_TARGET_TICK_RATE),
     pdTRUE,
     NULL,
     app_heater_tick_tmr_callback
    );

  event_dispatcher_init();
  mainloop_timer_init();

  gpio_init();
  pwm_init();
  adc_init();
  heater_init();
  misc_init();

  display_init();

  xTimerStart(_tick_tmr, 0);

  while(1)
  {
    app_heater_msg_t    m;

    if(xQueueReceive(_heater_q, (void*)&m, portMAX_DELAY) == pdFALSE)
    {
      continue;
    }

    app_heater_handle_cmd(&m);
    event_dispatcher_dispatch();
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// heater task api wrapper
//
////////////////////////////////////////////////////////////////////////////////
void
app_heater_init(void)
{
  _heater_q = xQueueCreate(16, sizeof(app_heater_msg_t));

  _rsp_q_mutex = xSemaphoreCreateMutex();
  _rsp_q    = xQueueCreate(1, sizeof(bool));

  xTaskCreate(app_heater_task, "heater_task", 4096, NULL, 7, NULL);
  shell_init();
}

void
app_heater_notify_tmr_tick(void)
{
  app_heater_send_msg(HEATER_MSG_TIMER_TICK, NULL);
}

void
app_heater_notify_gpio_change(void)
{
  app_heater_send_msg_from_isr(HEATER_MSG_PIN_CHANGE, NULL);
}

void
app_heater_request_gpio_out(app_heater_gpio_out_req_t* r)
{
  app_heater_send_msg(HEATER_MSG_GPIO_OUT_REQ, (void*)r);
}

void
app_heater_request_gpio_in(app_heater_gpio_in_rsp_t* r)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_GPIO_IN_REQ, (void*)r);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_request_pwm(app_heater_pwm_req_t* r)
{
  app_heater_send_msg(HEATER_MSG_PWM_REQ, (void*)r);
}

void
app_heater_request_adc(app_heater_adc_rsp_t* r)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_ADC_REQ, (void*)r);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_control_glow(bool on_off)
{
  app_heater_msg_t    m;

  m.msg = HEATER_MSG_CONTROL_GLOW;
  m.b = on_off;

  xQueueSend(_heater_q, (void*)&m, portMAX_DELAY);
}

void
app_heater_control_oil_pump(bool on_off)
{
  app_heater_msg_t    m;

  m.msg = HEATER_MSG_CONTROL_OIL;
  m.b = on_off;

  xQueueSend(_heater_q, (void*)&m, portMAX_DELAY);
}

void
app_heater_control_fan(bool on_off)
{
  app_heater_msg_t    m;

  m.msg = HEATER_MSG_CONTROL_FAN;
  m.b = on_off;

  xQueueSend(_heater_q, (void*)&m, portMAX_DELAY);
}

void
app_heater_control_oil_pump_freq(float freq)
{
  app_heater_msg_t    m;

  m.msg = HEATER_MSG_CONTROL_OIL_FREQ;
  m.f = freq;

  xQueueSend(_heater_q, (void*)&m, portMAX_DELAY);
}

void
app_heater_control_fan_power(uint8_t pwr)
{
  app_heater_msg_t    m;

  m.msg = HEATER_MSG_CONTROL_FAN_POWER;
  m.i = pwr;

  xQueueSend(_heater_q, (void*)&m, portMAX_DELAY);
}

void
app_heater_start()
{
  app_heater_send_msg(HEATER_MSG_START, NULL);
}

void
app_heater_stop()
{
  app_heater_send_msg(HEATER_MSG_STOP, NULL);
}

void
app_heater_status(app_heater_status_rsp_t* r)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_STATUS, (void*)r);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_get_settings(settings_t* r)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_SETTINGS_GET, (void*)r);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_save(void)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_SAVE, NULL);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_reset(void)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_RESET, NULL);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_mod_settings(app_heater_settings_req_t* r)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_SETTINGS_MOD, (void*)r);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_step(app_heater_step_req_t* r)
{
  bool    ok;

  xSemaphoreTake(_rsp_q_mutex, portMAX_DELAY);

  app_heater_send_msg(HEATER_MSG_STEP, (void*)r);

  xQueueReceive(_rsp_q, (void*)&ok, portMAX_DELAY);

  xSemaphoreGive(_rsp_q_mutex);
}

void
app_heater_set(uint8_t ndx)
{
  app_heater_msg_t    m;

  m.msg = HEATER_MSG_SET;
  m.i = ndx;

  xQueueSend(_heater_q, (void*)&m, portMAX_DELAY);
}
