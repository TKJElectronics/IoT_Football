#ifndef __SSD1963_GRADIENT_H
#define __SSD1963_GRADIENT_H

#include "stm32f4xx.h"
#include "SSD1963.h"

void LCD_GradientFill(u16 x0, u16 y0, u16 x1, u16 y1, u8 PercentageFilled, const u16 *GradientStops, u8 GradientStopsCount);


#endif /* __SSD1963_GRADIENT_H */
