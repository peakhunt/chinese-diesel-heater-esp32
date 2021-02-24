#ifndef __SSD_1306_DEF_H__
#define __SSD_1306_DEF_H__

#include <stdint.h>

#include "driver/i2c.h"
#include "ssd1306_fonts.h"

//
// configuration related
//
#define SSD1306_I2C_PORT          I2C_NUM_0
#define SSD1306_I2C_ADDR          (0x3C << 1)
#define SSD1306_HEIGHT            64
#define SSD1306_WIDTH             128
#define SSD1306_BUFFER_SIZE       (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

typedef enum 
{
  ssd1306_color_black = 0x00,
  ssd1306_color_white = 0x01,
} ssd1306_color_t;

extern void ssd1306_init(void);
extern void ssd1306_fill(ssd1306_color_t color);
extern void ssd1306_update_screen(void);
extern void ssd1306_set_contrast(const uint8_t value);
extern void ssd1306_set_display_on(const bool on);
extern bool ssd1306_get_display_on(void);
extern void ssd1306_set_cursor(uint8_t x, uint8_t y);
extern void ssd1306_draw_pixel(uint8_t x, uint8_t y, ssd1306_color_t color);
extern char ssd1306_write_char(char ch, ssd1306_font_t font, ssd1306_color_t color);
extern char ssd1306_write_string(char* str, ssd1306_font_t font, ssd1306_color_t color);
extern void ssd1306_printf(uint8_t x, uint8_t y, ssd1306_font_t font, ssd1306_color_t color, const char* fmt, ...);
extern void ssd1306_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ssd1306_color_t color);
extern void ssd1306_draw_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ssd1306_color_t color);

#endif /* !__SSD_1306_DEF_H__ */
