#ifndef __APP_HEATER_DEF_H__
#define __APP_HEATER_DEF_H__

#include <stdint.h>
#include <stdbool.h>

#include "app_config.h"
#include "pwm.h"
#include "gpio.h"
#include "adc.h"
#include "heater.h"

#define HEATER_MSG_TIMER_TICK               0
#define HEATER_MSG_PIN_CHANGE               1
#define HEATER_MSG_GPIO_OUT_REQ             2
#define HEATER_MSG_GPIO_IN_REQ              3
#define HEATER_MSG_PWM_REQ                  4
#define HEATER_MSG_ADC_REQ                  5

#define HEATER_MSG_CONTROL_GLOW             6
#define HEATER_MSG_CONTROL_OIL              7
#define HEATER_MSG_CONTROL_FAN              8
#define HEATER_MSG_CONTROL_OIL_FREQ         9
#define HEATER_MSG_CONTROL_FAN_POWER        10

#define HEATER_MSG_START                    11
#define HEATER_MSG_STOP                     12
#define HEATER_MSG_STATUS                   13

#define HEATER_MSG_SETTINGS_GET             14
#define HEATER_MSG_SETTINGS_MOD             15
#define HEATER_MSG_SAVE                     16
#define HEATER_MSG_RESET                    17
#define HEATER_MSG_STEP                     18
#define HEATER_MSG_SET                      19

typedef struct
{
  uint8_t         msg;
  union
  {
    void*           arg;
    uint32_t        i;
    float           f;
    bool            b;
  };
} app_heater_msg_t;

typedef struct
{
  gpio_out_pin_t    pin;
  bool              v;
} app_heater_gpio_out_req_t;

typedef struct
{
  bool                v[GPIO_MAX_INPUT];
  gpio_input_state_t  s[GPIO_MAX_INPUT];
} app_heater_gpio_in_rsp_t;

typedef struct
{
  pwm_channel_t   chnl;
  uint8_t         pct;
} app_heater_pwm_req_t;

typedef struct
{
  adcsample_t       samples[ADC_MAX_CHANNELS];
  float             volt[ADC_MAX_CHANNELS];
} app_heater_adc_rsp_t;

typedef struct
{
  heater_state_t        state;
  bool                  flame_detected;
  bool                  oil_on;
  bool                  glow_on;
  bool                  fan_on;
  fan_motor_state_t     fan_motor_state;
  uint8_t               fan_pwr;
  uint8_t               step;
  float                 oil_freq;
  float                 outlet_temp;
} app_heater_status_rsp_t;

typedef struct
{
  uint8_t               item;
  union
  {
    uint8_t             u8;
    uint32_t            u32;
    float               f;
  };
} app_heater_settings_req_t;

typedef struct
{
  uint8_t     ndx;
  uint8_t     pwr;
  float       freq;
} app_heater_step_req_t;

extern void app_heater_init(void);

extern void app_heater_notify_tmr_tick(void);
extern void app_heater_notify_gpio_change(void);
extern void app_heater_request_gpio_out(app_heater_gpio_out_req_t* r);
extern void app_heater_request_gpio_in(app_heater_gpio_in_rsp_t* r);
extern void app_heater_request_pwm(app_heater_pwm_req_t* r);
extern void app_heater_request_adc(app_heater_adc_rsp_t* r);

extern void app_heater_control_glow(bool on_off);
extern void app_heater_control_oil_pump(bool on_off);
extern void app_heater_control_fan(bool on_off);
extern void app_heater_control_oil_pump_freq(float freq);
extern void app_heater_control_fan_power(uint8_t pwr);

extern void app_heater_start();
extern void app_heater_stop();
extern void app_heater_status(app_heater_status_rsp_t* r);

extern void app_heater_get_settings(settings_t* r);
extern void app_heater_save(void);
extern void app_heater_reset(void);
extern void app_heater_mod_settings(app_heater_settings_req_t* r);
extern void app_heater_step(app_heater_step_req_t* r);
extern void app_heater_set(uint8_t ndx);

#endif /* !__APP_HEATER_DEF_H__ */
