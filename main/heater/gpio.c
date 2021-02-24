#include "driver/gpio.h"
#include "gpio.h"
#include "event_dispatcher.h"
#include "event_list.h"
#include "soft_timer.h"
#include "mainloop_timer.h"
#include "app_heater.h"

#include "esp_log.h"

#define PIN_CHANGE_DEBOUNCE_TIME          10    // 10ms

#define GPIO_PIN5           GPIO_NUM_5
#define GPIO_PIN18          GPIO_NUM_18
#define GPIO_PIN19          GPIO_NUM_19

#define GPIO_PIN25          GPIO_NUM_25
#define GPIO_PIN26          GPIO_NUM_26
#define GPIO_PIN27          GPIO_NUM_27


typedef struct
{
  gpio_in_pin_t           pin_name;
  gpio_input_state_t      state;
  SoftTimerElem           debounce_tmr;
  uint8_t                 pin;
  bool                    use_debounce;
  gpio_listener           listener;
  void*                   arg;
} gpio_input_t;

static gpio_input_t   _inputs[GPIO_MAX_INPUT] =
{
  {
    .pin_name = gpio_in_pin_25,
    .pin = GPIO_PIN25,
    .use_debounce = true,
  },
  {
    .pin_name = gpio_in_pin_26,
    .pin = GPIO_PIN26,
    .use_debounce = true,
  },
  {
    .pin_name = gpio_in_pin_27,
    .pin = GPIO_PIN27,
    .use_debounce = true,
  },
};

#define INVOKE_CALLBACK(i)                        \
  if(i->listener != NULL)                         \
  {                                               \
    i->listener(i->pin_name, i->state, i->arg);   \
  }

static void IRAM_ATTR
gpio_isr_handler(void* arg)
{
  // ets_printf("gpio_isr_handler\n");
  app_heater_notify_gpio_change();
}

static void
handle_pin_change(uint32_t event)
{
  gpio_input_t*   input;
  uint8_t         pv;

  for(uint8_t i = 0; i < GPIO_MAX_INPUT; i++)
  {
    input = &_inputs[i];

    pv = gpio_get_level(input->pin);
    // printf("handle_pin_change %d:%d\n", input->pin, pv);

    if(input->use_debounce)
    {
      switch(input->state)
      {
      case gpio_input_state_low:
        if(pv)
        {
          // goes high
          input->state = gpio_input_state_low_to_high;
          mainloop_timer_schedule(&input->debounce_tmr, PIN_CHANGE_DEBOUNCE_TIME);
        }
        break;

      case gpio_input_state_low_to_high:
        if(pv == 0)
        {
          // goes low again
          input->state = gpio_input_state_low;
          mainloop_timer_cancel(&input->debounce_tmr);
        }
        break;

      case gpio_input_state_high:
        if(pv == 0)
        {
          // goes low
          input->state = gpio_input_state_high_to_low;
          mainloop_timer_schedule(&input->debounce_tmr, PIN_CHANGE_DEBOUNCE_TIME);
        }
        break;

      case gpio_input_state_high_to_low:
        if(pv)
        {
          // goes high again
          input->state = gpio_input_state_high;
          mainloop_timer_cancel(&input->debounce_tmr);
        }
        break;
      }
    }
    else
    {
      switch(input->state)
      {
      case gpio_input_state_low:
        if(pv)
        {
          // goes high
          input->state = gpio_input_state_high;
          INVOKE_CALLBACK(input);
        }
        break;

      case gpio_input_state_high:
        if(pv == 0)
        {
          // goes low
          input->state = gpio_input_state_low;
          INVOKE_CALLBACK(input);
        }
        break;

      default:
        break;
      }
    }
  }
}

static void
debounce_tmr_callback(SoftTimerElem* te)
{
  gpio_input_t*   input = (gpio_input_t*)te->priv;

  if(input->state == gpio_input_state_low_to_high)
  {
    input->state = gpio_input_state_high;
    INVOKE_CALLBACK(input);
  }
  else if(input->state == gpio_input_state_high_to_low)
  {
    input->state = gpio_input_state_low;
    INVOKE_CALLBACK(input);
  }
}

static void
init_input_pins(void)
{
  gpio_config_t io_conf;

  io_conf.intr_type     = GPIO_PIN_INTR_ANYEDGE;
  io_conf.pin_bit_mask  = (1ULL << GPIO_PIN25) |
                          (1ULL << GPIO_PIN26) |
                          (1ULL << GPIO_PIN27);
  io_conf.mode          = GPIO_MODE_INPUT;
  io_conf.pull_down_en  = 1;
  io_conf.pull_up_en    = 0;
  gpio_config(&io_conf);

  for(uint8_t i = 0; i < GPIO_MAX_INPUT; i++)
  {
    _inputs[i].state = gpio_get_level(_inputs[i].pin) == 0 ? gpio_input_state_low : gpio_input_state_high;

    soft_timer_init_elem(&_inputs[i].debounce_tmr);
    _inputs[i].debounce_tmr.cb = debounce_tmr_callback;
    _inputs[i].debounce_tmr.priv = &_inputs[i];
    _inputs[i].listener = NULL;
    _inputs[i].arg = NULL;
  }

  gpio_install_isr_service(0);

  gpio_isr_handler_add(GPIO_PIN25, gpio_isr_handler, (void*) GPIO_PIN25);
  gpio_isr_handler_add(GPIO_PIN26, gpio_isr_handler, (void*) GPIO_PIN26);
  gpio_isr_handler_add(GPIO_PIN27, gpio_isr_handler, (void*) GPIO_PIN27);

  //
  // FIXME
  // how to route button event to display task???
  //
}

void
gpio_init(void)
{
  // out
  gpio_pad_select_gpio(GPIO_PIN5);
  gpio_set_direction(GPIO_PIN5, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_PIN5, 0);

  gpio_pad_select_gpio(GPIO_PIN18);
  gpio_set_direction(GPIO_PIN18, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_PIN18, 0);

  gpio_pad_select_gpio(GPIO_PIN19);
  gpio_set_direction(GPIO_PIN19, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_PIN19, 0);

  event_register_handler(handle_pin_change, DISPATCH_EVENT_PIN_CHANGE);
  init_input_pins();
}

void
gpio_set(gpio_out_pin_t pin, bool v)
{
  switch(pin)
  {
  case gpio_out_pin_5:
    gpio_set_level(GPIO_PIN5, v);
    break;

  case gpio_out_pin_18:
    gpio_set_level(GPIO_PIN18, v);
    break;

  case gpio_out_pin_19:
    gpio_set_level(GPIO_PIN19, v);
    break;

  default:
    return;
  }
}

bool
gpio_get(gpio_in_pin_t pin)
{
  switch(_inputs[pin].state)
  {
  case gpio_input_state_low:
  case gpio_input_state_low_to_high:
    return false;
  default:
    break;
  }
  return true;
}

void
gpio_set_debounce(gpio_in_pin_t pin, bool debounce)
{
  gpio_input_t*   input;

  input = &_inputs[pin];

  input->use_debounce = debounce;
  mainloop_timer_cancel(&input->debounce_tmr);

  if(input->state == gpio_input_state_low_to_high)
  {
    input->state = gpio_input_state_high;
  }
  else if(input->state == gpio_input_state_high_to_low)
  {
    input->state = gpio_input_state_low;
  }
}

gpio_input_state_t
gpio_get_state(gpio_in_pin_t pin)
{
  return _inputs[pin].state;
}

void
gpio_listen(gpio_in_pin_t pin, gpio_listener listener, void* arg)
{
  _inputs[pin].listener = listener;
  _inputs[pin].arg      = arg;
}
