#include "stm32f4xx.h"
#include "SSD1963_gradient.h"

/* Red-Yellow-Green Gradient color variable */
const u8 RedYellowGreenGradient_Stops = 3;
const u16 RedYellowGreenGradient[] = { RGB565CONVERT(228,25,3), RGB565CONVERT(239,154,19), RGB565CONVERT(86,189,19) };

u16 interpolate(u16 c1, u16 c2, float normalized_value){
    if( normalized_value <= 0.0 ){ return c1; }
    if( normalized_value >= 1.0 ){ return c2; }
    
    u8 c1_red = (c1 & 0xF800) >> 8;
    u8 c1_green = (c1 & 0x7E0) >> 3;
    u8 c1_blue = (c1 & 0x1F) << 3;
    u8 c2_red = (c2 & 0xF800) >> 8;
    u8 c2_green = (c2 & 0x7E0) >> 3;
    u8 c2_blue = (c2 & 0x1F) << 3;

    uint8_t red = (uint8_t)((1.0-normalized_value)*c1_red + normalized_value*c2_red);
    uint8_t green = (uint8_t)((1.0-normalized_value)*c1_green + normalized_value*c2_green);
    uint8_t blue = (uint8_t)((1.0-normalized_value)*c1_blue + normalized_value*c2_blue);
    
    return RGB565CONVERT(red, green, blue);
}


u16 getRgb(u16 min, u16 max, uint16_t value, const u16 *GradientStops, u8 GradientStopsCount){
      // Handle outliers
      if( value < min ) return 0;
      if( value > max ) return 0;
      
      // Find the "bin" that value falls in
      uint16_t range = max - min;
      uint16_t v = value - min;
      float step = range / (float)(GradientStopsCount-1);
      int bin = (int)(v / step);
      
      // Normalize value in the interval (0,1]
      float normalized_v = (v - bin*step) / step;
      
      return interpolate(GradientStops[bin], GradientStops[bin+1], normalized_v);
}

void LCD_GradientFill(u16 x0, u16 y0, u16 x1, u16 y1, u8 PercentageFilled, const u16 *GradientStops, u8 GradientStopsCount)
{
    if (y1 > y0 || PercentageFilled > 100) return;
    u16 i;
    u16 DeltaY = y0 - y1;
    u16 StopY = y0 - ((PercentageFilled*DeltaY)/100);   

    for (i = 0; i < DeltaY; i++) {
        if (y0 == StopY) break;
        LCD_Line(x0, y0, x1, y0, getRgb(0, DeltaY, i, GradientStops, GradientStopsCount));
        y0--;
    }

}
