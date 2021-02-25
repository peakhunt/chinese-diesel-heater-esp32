#ifndef __DISPLAY_DEF_H__
#define __DISPLAY_DEF_H__

#include <stdint.h>

typedef enum
{
  display_event_btn_0,
  display_event_btn_1,
  display_event_btn_2,
  display_event_btn_3,
} display_event_t;

extern void display_init(void);
extern void display_feed_event(display_event_t e);

#endif /* !__DISPLAY_DEF_H__ */
