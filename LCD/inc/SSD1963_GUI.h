#ifndef __SSD1963_GUI_H
#define __SSD1963_GUI_H

#include "stm32f4xx.h"
#include "SSD1963.h"
#include "SSD1963_api.h"
#include "SSD1963_font.h"
#include "images.h"

#include <stdbool.h>

/* RTOS includes. */
#include "Os_Wrap.h"

extern OS_SemaphoreHandle gLCD_Mutex;			//lcd²Ù×÷»¥³âÁ¿

#define LCD_Lock() OS_SemaphoreTake(gLCD_Mutex,OS_MAX_DELAY)
#define LCD_UnLock() OS_SemaphoreGive(gLCD_Mutex)

#define GUI_SetDisplay(LCD) LCD_Lock(); LCD_EnableDisplayControl(LCD); LCD_UnLock();

#define GUI_Line(x0, y0, x1, y1, color) LCD_Lock(); LCD_Line(x0, y0, x1, y1, color); LCD_UnLock();
#define GUI_Circle(cx, cy, r, color, fill) LCD_Lock(); LCD_Circle(cx, cy, r, color, fill); LCD_UnLock();

#define GUI_Clear(color) LCD_Lock(); LCD_Clear(color); LCD_UnLock();
#define GUI_ClearArea(x0, y0, x1, y1, color) LCD_Lock(); LCD_ClearArea(x0, y0, x1, y1, color); LCD_UnLock();
#define GUI_PutPixel(x, y, color) LCD_Lock(); LCD_PutPixel(x, y, color); LCD_UnLock();

#define GUI_Text(x, y, str, Font, ForeColor, BackColor) LCD_Lock(); LCD_Text(x, y, str, Font, ForeColor, BackColor); LCD_UnLock();
#define GUI_Text_Center(x0, y0, x1, y1, str, Font, ForeColor, BackColor) LCD_Lock(); LCD_Text_Center(x0, y0, x1, y1, str, Font, ForeColor, BackColor); LCD_UnLock();
#define GUI_Text_Transparent(x, y, str, Font, ForeColor) LCD_Lock(); LCD_TransparentText(x, y, str, Font, ForeColor); LCD_UnLock();
#define GUI_Text_Transparent_Center(x0, y0, x1, y1, str, Font, ForeColor) LCD_Lock(); LCD_TransparentText_Center(x0, y0, x1, y1, str, Font, ForeColor); LCD_UnLock();
#define GUI_LargeText_Transparent(x, y, str, Font, ForeColor) LCD_Lock(); LCD_TransparentLargeText(x, y, str, Font, ForeColor); LCD_UnLock();
#define GUI_LargeText_Transparent_Center(x0, y0, x1, y1, str, Font, ForeColor) LCD_Lock(); LCD_TransparentLargeText_Center(x0, y0, x1, y1, str, Font, ForeColor); LCD_UnLock();
#define GUI_Line(x0, y0, x1, y1, color) LCD_Lock(); LCD_Line(x0, y0, x1, y1, color); LCD_UnLock();
#define GUI_Circle(cx, cy, r, color, fill) LCD_Lock(); LCD_Circle(cx, cy, r, color, fill); LCD_UnLock();
#define GUI_Rectangle(left, top, right, bottom, color, fill) LCD_Lock(); LCD_Rectangle(left, top, right, bottom, color, fill); LCD_UnLock();
#define GUI_Square(x0, y0, width, color, fill) LCD_Lock(); LCD_Square(x0, y0, width, color, fill); LCD_UnLock();
#define GUI_ClearCharBox(x, y, color) LCD_Lock(); LCD_ClearCharBox(x, y, color); LCD_UnLock();

#define GUI_DisplayRAW(x0, y0, str) LCD_Lock(); LCD_DisplayRAW(x0, y0, str); LCD_UnLock();
#define GUI_DisplayRAW_Transparent(x0, y0, str) LCD_Lock(); LCD_DisplayRAW_Transparent(x0, y0, str); LCD_UnLock();

#define GUI_DisplayBMP_FAT(Xpos, Ypos, BmpName) LCD_Lock(); LCD_DisplayBMP_FAT(Xpos, Ypos, BmpName); LCD_UnLock();
#define GUI_DisplayBMP_FAT_Transparent(Xpos, Ypos, BmpName) LCD_Lock(); LCD_DisplayBMP_FAT_Transparent(Xpos, Ypos, BmpName); LCD_UnLock();

#define GUI_SetBacklight(intensity) LCD_Lock(); LCD_SetBacklight(intensity); LCD_UnLock();
#define GUI_FadeBacklight(fade) LCD_Lock(); LCD_FadeBacklight(fade); LCD_UnLock();


void GUI_InitializeConsole(void);
void GUI_ConsolePrint(uint8_t * text);

#endif /* __SSD1963_GUI_H */
