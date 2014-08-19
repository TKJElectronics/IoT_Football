#ifndef __SSD1963_API_H
#define __SSD1963_API_H

#include "stm32f4xx.h"
#include "SSD1963.h"
#include "SSD1963_font.h"

#include <stdbool.h>


u16 Lcd_Color565(u32 RGB);  // RGB颜色转为16位(565)
typedef union
{
  u16 U16;
  u8 U8[2];
}ColorTypeDef;


u16 LCD_PutChar(u16 x, u16 y, u8 Character, const unsigned char *Font, u16 ForeColor, u16 BackColor);
void LCD_Text(u16 x, u16 y, u8 *str, const unsigned char *Font, u16 ForeColor, u16 BackColor);
void LCD_Text_Center(u16 x0, u16 y0, u16 x1, u16 y1, u8 *str, const unsigned char *Font, u16 ForeColor, u16 BackColor);
void LCD_TransparentText(u16 x, u16 y, u8 *str, const unsigned char *Font, u16 ForeColor);
void LCD_TransparentText_Center(u16 x0, u16 y0, u16 x1, u16 y1, u8 *str, const unsigned char *Font, u16 ForeColor);
void LCD_TransparentLargeText(u16 x, u16 y, u8 *str, const unsigned int *Font, u16 ForeColor);
void LCD_TransparentLargeText_Center(u16 x0, u16 y0, u16 x1, u16 y1, u8 *str, const unsigned int *Font, u16 ForeColor);
void LCD_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color);
void LCD_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill); 
void LCD_Rectangle(u16 left, u16 top, u16 right, u16 bottom, u16 color, bool fill);
void LCD_Square(u16 x0, u16 y0, u16 width, u16 color,u8 fill);
void LCD_ClearCharBox(u16 x,u16 y,u16 color);

void LCD_DisplayRAW(u16 x0, u16 y0, const unsigned char *str);
void LCD_DisplayRAW_Transparent(u16 x0, u16 y0, const unsigned char *str);

void LCD_DisplayBMP_FAT(uint16_t Xpos, uint16_t Ypos, const char* BmpName);
void LCD_DisplayBMP_FAT_Transparent(uint16_t Xpos, uint16_t Ypos, const char* BmpName);

int power (int m, int n);



#endif /* __SSD1963_API_H */
