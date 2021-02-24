#ifndef __UTILITIES_DEF_H__
#define __UTILITIES_DEF_H__

#include <stdint.h>

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

#endif /* !__UTILITIES_DEF_H__ */
