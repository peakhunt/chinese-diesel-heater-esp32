#ifndef __DISPLAY_DEF_H__
#define __DISPLAY_DEF_H__

#include <stdint.h>

typedef enum
{
  display_input_mode,
  display_input_select,
  display_input_up,
  display_input_down,
} display_input_t;

extern void display_init(void);
extern void display_feed_event(display_input_t e);

#endif /* !__DISPLAY_DEF_H__ */
