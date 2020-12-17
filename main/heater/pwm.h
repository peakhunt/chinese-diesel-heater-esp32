#ifndef __PWM_DEF_H__
#define __PWM_DEF_H__

typedef enum
{
  pwm_channel_0,
} pwm_channel_t;

#define PWM_MAX_CHANNEL (pwm_channel_0 + 1)

extern void pwm_init(void);
extern void pwm_control(pwm_channel_t chnl, uint8_t pct);

#endif //!__PWM_DEF_H__
