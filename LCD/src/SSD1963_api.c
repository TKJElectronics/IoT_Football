#include "stm32f4xx.h"
#include "SSD1963_api.h"


void LCD_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color)
{
 	u16 x,y;
 	u16 dx;// = abs(x1 - x0);
 	u16 dy;// = abs(y1 - y0);

	if(y0==y1)
	{
		if(x0<=x1)
		{
			x=x0;
		}
		else
		{
			x=x1;
			x1=x0;
		}
  		while(x <= x1)
  		{
   			LCD_PutPixel(x,y0,color);
   			x++;
  		}
  		return;
	}
	else if(y0>y1)
	{
		dy=y0-y1;
	}
	else
	{
		dy=y1-y0;
	}
 
 	if(x0==x1)
	{
		if(y0<=y1)
		{
			y=y0;
		}
		else
		{
			y=y1;
			y1=y0;
		}
  		while(y <= y1)
  		{
   			LCD_PutPixel(x0,y,color);
   			y++;
  		}
  		return;
	}
	else if(x0 > x1)
 	{
		dx=x0-x1;
  		x = x1;
  		x1 = x0;
  		y = y1;
  		y1 = y0;
 	}
 	else
 	{
		dx=x1-x0;
  		x = x0;
  		y = y0;
 	}

 	if(dx == dy)
 	{
  		while(x <= x1)
  		{

   			x++;
			if(y>y1)
			{
				y--;
			}
			else
			{
   				y++;
			}
   			LCD_PutPixel(x,y,color);
  		}
 	}
 	else
 	{
 		LCD_PutPixel(x, y, color);
  		if(y < y1)
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
    			s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
     				{
      					p += twoDy;
     				}
     				else
     				{
      					y++;
      					p += twoDyMinusDx;
     				}
     				LCD_PutPixel(x, y,color);
    			}
   			}
   			else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
    			s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y < y1)
    			{
     				y++;
     				if(p < 0)
     				{
      					p += twoDx;
     				}
     				else
     				{
      					x++;
      					p+= twoDxMinusDy;
     				}
     				LCD_PutPixel(x, y, color);
    			}
   			}
  		}
  		else
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
	    		s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
	     			{
    	  				p += twoDy;
     				}
     				else
     				{
      					y--;
	      				p += twoDyMinusDx;
    	 			}
     				LCD_PutPixel(x, y,color);
    			}
   			}
	   		else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
	    		s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y1 < y)
    			{
     				y--;
     				if(p < 0)
	     			{
    	  				p += twoDx;
     				}
     				else
     				{
      					x++;
	      				p+= twoDxMinusDy;
    	 			}
     				LCD_PutPixel(x, y,color);
    			}
   			}
  		}
 	}
}


void LCD_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
{
	if (cx > GetMaxX() || cy > GetMaxY() || (cx+r) > GetMaxX() || (cy+r) > GetMaxY()) return;
	u16 x,y;
	s16 delta,tmp;
	x=0;
	y=r;
	delta=3-(r<<1);

	while(y>x)
	{
		if(fill)
		{
			LCD_Line(cx+x,cy+y,cx-x,cy+y,color);
			LCD_Line(cx+x,cy-y,cx-x,cy-y,color);
			LCD_Line(cx+y,cy+x,cx-y,cy+x,color);
			LCD_Line(cx+y,cy-x,cx-y,cy-x,color);
		}
		else
		{
			LCD_PutPixel(cx+x,cy+y,color);
			LCD_PutPixel(cx-x,cy+y,color);
			LCD_PutPixel(cx+x,cy-y,color);
			LCD_PutPixel(cx-x,cy-y,color);
			LCD_PutPixel(cx+y,cy+x,color);
			LCD_PutPixel(cx-y,cy+x,color);
			LCD_PutPixel(cx+y,cy-x,color);
			LCD_PutPixel(cx-y,cy-x,color);
		}
		x++;
		if(delta>=0)
		{
			y--;
			tmp=(x<<2);
			tmp-=(y<<2);
			delta+=(tmp+10);
		}
		else
		{
			delta+=((x<<2)+6);
		}
	}
}

/*********************************************************************
* Function: WORD LCD_Rectangle(SHORT left, SHORT top, SHORT right, SHORT bottom)
*
* PreCondition: none
*
* Input: left,top - top left corner coordinates,
*        right,bottom - bottom right corner coordinates
*
* Output: For Blocking configuration:
*         - Always return 1.
*
* Side Effects: none
*
* Overview: draws rectangle filled with current color
*
* Note: none
*
********************************************************************/

void LCD_Rectangle(u16 left, u16 top, u16 right, u16 bottom, u16 color, bool fill)
{
	register u16  x,y;


  if (!fill) {
    LCD_Line(left,top,left,bottom,color);
    LCD_Line(left,bottom,right,bottom,color);
    LCD_Line(right,bottom,right,top,color);
    LCD_Line(right,top,left,top,color);
  
  } else { // Filled Rectangle
	if(_clipRgn){
		if(left<_clipLeft)
			left = _clipLeft;
		if(right>_clipRight)
			right= _clipRight;
		if(top<_clipTop)
			top = _clipTop;
		if(bottom>_clipBottom)
			bottom = _clipBottom;
	}

	LCD_SetArea(left,top,right,bottom);
	LCD_WriteCommand(CMD_WR_MEMSTART);

	for(y=top; y<bottom+1; y++){
		for(x=left; x<right+1; x++){
			LCD_WriteData(color);
		}
	}
  }
}

void LCD_Square(u16 x0, u16 y0, u16 width, u16 color,u8 fill)
{
	LCD_Rectangle(x0, y0, x0+width, y0+width, color,fill);
}

void LCD_ClearCharBox(u16 x,u16 y,u16 color)
{
	LCD_Rectangle(x*8,y*16,x*8+8,y*16+16,color,true);
}



int power (int base, int n) {
    int     i,
            p;
    p = 1;
    for (i = 1; i <= n; ++i)
	p *= base;
    return p;
}
u16 TextWidth(u8 *str, const unsigned char *Font)
{
    u32 CharacterOffset;
    u16 Width = 0;
    
    while (*str != 0)
    {
        CharacterOffset = ((*str - Font[4]) * (((uint16_t)Font[0] << 8) + Font[1])) + 5;
        Width += Font[CharacterOffset];
        str++;        
    }

    return Width;
}

u16 LCD_PutChar(u16 x, u16 y, u8 Character, const unsigned char *Font, u16 ForeColor, u16 BackColor)
{
    u8 row = 0;
    u8 column = 0;
    u32 offset = ((Character - Font[4]) * (((uint16_t)Font[0] << 8) + Font[1])) + 5;
    u8 RowOffset = 1;
       
    u8 CharacterWidth = Font[2];
    u8 CharacterHeight = Font[3];
    u8 ReturnWidth = Font[offset];
        
    u8 BytesPrColumn = CharacterHeight / 8;
    if ((CharacterWidth % 8) > 0)
        BytesPrColumn += 1;
             
    if (Character == ' ') // This is just a space - return the width of the selected character set
        return ReturnWidth;
    
    LCD_SetArea(x, y, x+CharacterWidth-1, y+CharacterHeight-1);
    LCD_WriteCommand(CMD_WR_MEMSTART);      

    for (row = 0; row < CharacterHeight; row++)
    {
        for (column = 0; column < CharacterWidth; column++)
        {
            if (row == 0)
            {
                if ((Font[BytesPrColumn * column + RowOffset + offset] & 1) > 0)
                {
                    LCD_WriteData(ForeColor);
                } else {
                    LCD_WriteData(BackColor);
                }
            }
            else if ((row % 8) == 0)
            {
                if (column == 0) RowOffset++;                 
                if ((Font[BytesPrColumn * column + RowOffset + offset] & 1) > 0)
                {
                    LCD_WriteData(ForeColor);
                } else {
                    LCD_WriteData(BackColor);
                }
            }
            else if ((row % 8) > 0)
            {
                if ((Font[BytesPrColumn * column + RowOffset + offset] & (power(2, (row % 8)))) > 0)  // Math.Pow = ^ (power)
                {
                    LCD_WriteData(ForeColor);
                } else {
                    LCD_WriteData(BackColor);
                }
            }                               
        }
    }
    
    return ReturnWidth;
}    

void LCD_Text(u16 x, u16 y, u8 *str, const unsigned char *Font, u16 ForeColor, u16 BackColor)
{
  while (*str != 0)
  {
    x += LCD_PutChar(x,y,*str,Font,ForeColor,BackColor);
    str++;
  }
}
void LCD_Text_Center(u16 x0, u16 y0, u16 x1, u16 y1, u8 *str, const unsigned char *Font, u16 ForeColor, u16 BackColor)
{
  u8 i = 0;
  u16 x, y;

  u8 CharacterCount = 0;
  u8 *tmpPtr = str;
  while (*tmpPtr != 0)
  {
    tmpPtr++;
    CharacterCount++;
  }
  
  if (x0 != x1)
    x = (((x1-x0) - TextWidth(str, Font)) / 2) + x0;
  else
    x = x0;
   
  if (y0 != y1)
    y = (((y1-y0) - Font[3]) / 2) + y0;
  else
    y = y0;


  for (i = 0; i < CharacterCount; i++)
  {
    x += LCD_PutChar(x,y,*str,Font,ForeColor,BackColor);
    str++;
  }
}

u16 LCD_PutTransparentChar(u16 x, u16 y, u8 Character, const unsigned char *Font, u16 ForeColor)
{
    u32 offset;
    u8 row = 0;
    u8 column = 0;
    offset = ((Character - Font[4]) * (((uint16_t)Font[0] << 8) + Font[1])) + 5;
    u8 RowOffset = 1;
    u8 ReturnWidth = Font[offset];
    bool SetPosition = false;
       
    u8 CharacterWidth = Font[2];
    u8 CharacterHeight = Font[3];
        
    u8 BytesPrColumn = CharacterHeight / 8;
    if ((CharacterWidth % 8) > 0)
        BytesPrColumn += 1;
        
    if (Character == ' ') // This is just a space - return the width of the selected character set
        return ReturnWidth;
    
    LCD_SetArea(x, y, x+CharacterWidth-1, y+CharacterHeight-1);
    LCD_WriteCommand(CMD_WR_MEMSTART);      

    for (row = 0; row < CharacterHeight; row++)
    {
        for (column = 0; column < CharacterWidth; column++)
        {
            if (SetPosition == true) {
                LCD_SetArea(x+column, y+row, x+Font[2]-1, y+Font[3]-1);
                LCD_WriteCommand(CMD_WR_MEMSTART);
                SetPosition = false;
            }
                
            if (row == 0)
            {
                if ((Font[BytesPrColumn * column + RowOffset + offset] & 1) > 0)
                {
                    LCD_WriteData(ForeColor);
                } else {
                    SetPosition = true;
                }
            }
            else if ((row % 8) == 0)
            {
                if (column == 0) RowOffset++;                 
                if ((Font[BytesPrColumn * column + RowOffset + offset] & 1) > 0)
                {
                    LCD_WriteData(ForeColor);
                } else {
                    SetPosition = true;
                }
            }
            else if ((row % 8) > 0)
            {
                if ((Font[BytesPrColumn * column + RowOffset + offset] & (power(2, (row % 8)))) > 0)  // Math.Pow = ^ (power)
                {
                    LCD_WriteData(ForeColor);
                } else {
                    SetPosition = true;
                }
            }                               
        }
    }
    
    return ReturnWidth;
}    

void LCD_TransparentText(u16 x, u16 y, u8 *str, const unsigned char *Font, u16 ForeColor)
{
  while (*str != 0)
  {
    x += LCD_PutTransparentChar(x,y,*str,Font,ForeColor);
    str++;
  }
}
void LCD_TransparentText_Center(u16 x0, u16 y0, u16 x1, u16 y1, u8 *str, const unsigned char *Font, u16 ForeColor)
{
  u8 i = 0;
  u16 x, y;
  u8 CharacterCount = 0;

  u8 *tmpPtr = str;
  while (*tmpPtr != 0)
  {
    tmpPtr++;
    CharacterCount++;
  }
  
  if (x0 != x1)
    x = (((x1-x0) - TextWidth(str, Font)) / 2) + x0;
  else
    x = x0;
   
  if (y0 != y1)
    y = (((y1-y0) - Font[3]) / 2) + y0;
  else
    y = y0;


  for (i = 0; i < CharacterCount; i++)
  {
    x += LCD_PutTransparentChar(x,y,*str,Font,ForeColor);
    str++;
  }
}





void LCD_DisplayRAW(u16 x0, u16 y0, const unsigned char *str)
{

	u32 temp, i, difference;    
    u16 x1, y1;    
    u16 imageWidth, imageHeight;        
    uint16_t data;
    
	data = str[1];
	data |= (str[0] << 8);
    imageWidth = data;
    x1 = imageWidth-1 + x0;

	data = str[3];
	data |= (str[2] << 8);
    imageHeight = data;
    y1 = imageHeight-1 + y0;
    
	LCD_SetArea(x0,y0,x1,y1);
	LCD_WriteCommand(CMD_WR_MEMSTART);

	for (temp = 2; temp < (imageWidth*imageHeight)+2; temp++)
	{  
		data = str[(2 * temp) + 1];
		data |= (str[(2 * temp)] << 8);
	
		LCD_WriteData(data);
	}

//==============================  
}

void LCD_DisplayRAW_Transparent(u16 x0, u16 y0, const unsigned char *str)
{

	u32 temp, i, difference;    
    u16 x1, y1;  
    u16 x, y;
    bool Reposition = false;
    u16 imageWidth, imageHeight;        
    ColorTypeDef color;
    
    color.U8[1] =*(unsigned short *)(&str[ 0]);
	color.U8[0]=*(unsigned short *)(&str[ 1]);
    imageWidth = color.U16;
    x1 = imageWidth-1 + x0;

    color.U8[1] =*(unsigned short *)(&str[ 2]);
	color.U8[0]=*(unsigned short *)(&str[ 3]);
    imageHeight = color.U16;
    y1 = imageHeight-1 + y0;
    
    x = x0;
    y = y0;

	LCD_SetArea(x0,y0,x1,y1);
	LCD_WriteCommand(CMD_WR_MEMSTART);

	for (temp = 2; temp < (imageWidth*imageHeight)+2; temp++)
	{  
		color.U8[1] =*(unsigned short *)(&str[ 2 * temp]);
		color.U8[0]=*(unsigned short *)(&str[ 2 * temp+1]);
		
        if (color.U16 != BRIGHTMAGENTA) {
            if (Reposition == true) {
                LCD_SetArea(x,y,x1,y1); // Set the pixel position to the next pixel, so we skip this "transparent" one
                LCD_WriteCommand(CMD_WR_MEMSTART);     // Then we also have to re-enable the direct LCD writes
                Reposition = false;
            }
            LCD_WriteData(color.U16);                
        } else {        
            Reposition = true;
        }    
        x++; // Next Pixel Pos X
        if (x > x1) {
          y++;  // Next Pixel Pos Y
          x = x0;
        }     
	}

//==============================  
}


#include "ff.h"
#include "diskio.h"
#include "sdio_sd.h"
static unsigned char BMP_buffer[1024];
/*******************************************************************************
* Function Name  : LCD_WriteBMP_FAT_Transparent
* Description    : Displays a bitmap picture loaded from FAT system from SD
                 : Xpoz - X position
                 : Ypoz - Y position
                 : BmpName - filename of BMP file
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayBMP_FAT(uint16_t Xpos, uint16_t Ypos, const char* BmpName)
{
  uint32_t index = 0, size = 0, width=0, height=0, i1 = 0;
  uint32_t BmpAddress;
  uint16_t data;
  FATFS fs;
  FIL F;
  unsigned int bytesRead;

  SD_Lock();
  if (f_mount(0, &fs) != FR_OK) return;
  if (f_open (&F, BmpName, FA_READ) != FR_OK) return;
  if (f_read (&F, BMP_buffer, 26, &bytesRead) != FR_OK) return;

  BmpAddress = (uint32_t)BMP_buffer;

  /* Read bitmap size */
  size = *(uint16_t *) (BmpAddress + 2);
  size |= (*(uint16_t *) (BmpAddress + 4)) << 16;

  /* Get bitmap data address offset */
  index = *(uint16_t *) (BmpAddress + 10);
  index |= (*(uint16_t *) (BmpAddress + 12)) << 16;

  /* Read bitmap width*/
  width = *(uint16_t *) (BmpAddress + 18);
  width |= (*(uint16_t *) (BmpAddress + 20)) << 16;
  /*read bitmap height*/
  height = *(uint16_t *) (BmpAddress + 22);
  height |= (*(uint16_t *) (BmpAddress + 24)) << 16;

  size = (size - index)/2;
  size = size - 1; // Adobe Photoshop Anomally

  // Synchronize f_read right in front of the image data
  f_read (&F, BMP_buffer, index-26, &bytesRead);

  /* Set GRAM write direction to "Bottom to Top" */
  LCD_SetAddressOrder(DIR_BOTTOM_TO_TOP);
  /*Setup the window*/
  LCD_SetArea(Xpos, Ypos, Xpos+width-1, Ypos+height-1);
  /*Prepare for direct LCD writes*/
  LCD_WriteCommand(CMD_WR_MEMSTART);

  do  {
    if (size*2 < 512) {
        i1 = size*2;
    } else {
        i1 = 512;
    }
    size -= i1/2;
    f_read (&F, BMP_buffer, i1, &bytesRead);

    for(index = 0; index < i1/2; index++)
    {
        LCD_WriteData(*(uint16_t *)BmpAddress);
        BmpAddress += 2;	 //jump on next pixel
    }
    BmpAddress = (uint32_t)BMP_buffer;
  } while (size > 0);

  f_close (&F);

  SD_UnLock();

  /* Set GRAM write direction to "Top to Bottom" */
  LCD_SetAddressOrder(DIR_TOP_TO_BOTTOM);
}

/*******************************************************************************
* Function Name  : LCD_WriteBMP_FAT
* Description    : Displays a transparent (MAGENTA = 255,0,255) bitmap picture
                 : loaded from FAT system from SD
                 : Xpoz - X position
                 : Ypoz - Y position
                 : BmpName - filename of BMP file
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayBMP_FAT_Transparent(uint16_t Xpos, uint16_t Ypos, const char* BmpName)
{
  uint32_t index = 0, size = 0, width=0, height=0, i1 = 0;
  uint16_t x, y;
  bool Reposition = false;
  uint32_t BmpAddress;
  FATFS fs;
  FIL F;
  unsigned int bytesWritten;

  SD_Lock();
  f_mount(0, &fs);
  f_open (&F, BmpName, FA_READ);
  f_read (&F, BMP_buffer, 26, &bytesWritten);

  BmpAddress = (uint32_t)BMP_buffer;

  /* Read bitmap size */
  size = *(uint16_t *) (BmpAddress + 2);
  size |= (*(uint16_t *) (BmpAddress + 4)) << 16;

  /* Get bitmap data address offset */
  index = *(uint16_t *) (BmpAddress + 10);
  index |= (*(uint16_t *) (BmpAddress + 12)) << 16;

  /* Read bitmap width*/
  width = *(uint16_t *) (BmpAddress + 18);
  width |= (*(uint16_t *) (BmpAddress + 20)) << 16;
  /*read bitmap height*/
  height = *(uint16_t *) (BmpAddress + 22);
  height |= (*(uint16_t *) (BmpAddress + 24)) << 16;

  size = (size - index)/2;
  size = size - 1; // Adobe Photoshop Anomally

  // Synchronize f_read right in front of the image data
  f_read (&F, BMP_buffer, index-26, &bytesWritten);

  /* Set GRAM write direction to "Bottom to Top" */
  LCD_SetAddressOrder(DIR_BOTTOM_TO_TOP);
  /*Setup the window*/
  LCD_SetArea(Xpos, Ypos, Xpos+width-1, Ypos+height-1);
  /*Prepare for direct LCD writes*/
  LCD_WriteCommand(CMD_WR_MEMSTART);

  x = Xpos;
  y = Ypos+height-1;
  do  {
    if (size*2 < 512) {
        i1 = size*2;
    } else {
        i1 = 512;
    }
    size -= i1/2;
    f_read (&F, BMP_buffer, i1, &bytesWritten);

    for(index = 0; index < i1/2; index++)
    {
        if ((*(uint16_t *)BmpAddress) != BRIGHTMAGENTA) {
            if (Reposition == true) {
                LCD_SetArea(x, Ypos, Xpos+width-1, y); // Set the pixel position to the next pixel, so we skip this "transparent" one
                LCD_WriteCommand(CMD_WR_MEMSTART);     // Then we also have to re-enable the direct LCD writes
                Reposition = false;
            }
            LCD_WriteData(*(uint16_t *)BmpAddress);
        } else {
            Reposition = true;
        }
        x++; // Next Pixel Pos X
        if (x == Xpos+width) {
          y--;  // Next Pixel Pos Y
          x = Xpos;
        }
        BmpAddress += 2;	 //jump on next pixel
    }
    BmpAddress = (uint32_t)BMP_buffer;
  } while (size > 0);

  f_close (&F);
  f_mount (0, 0);
  SD_UnLock();

  /* Set GRAM write direction to "Top to Bottom" */
  LCD_SetAddressOrder(DIR_TOP_TO_BOTTOM);
}
