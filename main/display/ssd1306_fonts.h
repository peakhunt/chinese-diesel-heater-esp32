#ifndef __SSD1306_FONTS_DEF_H__
#define __SSD1306_FONTS_DEF_H__

#include <stdint.h>

#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_11x18
#define SSD1306_INCLUDE_FONT_16x26

typedef struct
{
  const uint8_t width; 
  uint8_t height;
  const uint16_t *data;
} ssd1306_font_t;

#ifdef SSD1306_INCLUDE_FONT_6x8
extern ssd1306_font_t Font_6x8;
#endif
#ifdef SSD1306_INCLUDE_FONT_7x10
extern ssd1306_font_t Font_7x10;
#endif
#ifdef SSD1306_INCLUDE_FONT_11x18
extern ssd1306_font_t Font_11x18;
#endif
#ifdef SSD1306_INCLUDE_FONT_16x26
extern ssd1306_font_t Font_16x26;
#endif

#endif // !__SSD1306_FONTS_DEF_H__
