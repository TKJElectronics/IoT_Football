#ifndef _SSD1963_H
#define _SSD1963_H

#include "stm32f4xx.h"

//include the command table for SSD1963
#include "SSD1963_CMD.h"

//include the SSD1963 API
#include "SSD1963_api.h"
#include "SSD1963_GUI.h"
#include "SSD1963_gradient.h"
#include "SSD1963_touch.h"

#include <stdbool.h>

//#define LCD_LONG_WIRE 1

/* LCD Panel settings */
#define DISP_HOR_RESOLUTION 800
#define DISP_VER_RESOLUTION 480

#define DISP_ORIENTATION 90

#define HDP 799

#define HT 928
#define HPS 46
#define LPS 15
#define HPW 48

#define VDP 479

#define VT 525
#define VPS 16
#define FPS 8
#define VPW 16


#define LCD1_CS_PIN		GPIO_Pin_7	// PD7 NE1
#define LCD2_CS_PIN		GPIO_Pin_9	// PG9 NE2
extern uint16_t LCD1_Current_CS_Pin;
extern uint16_t LCD2_Current_CS_Pin;

typedef enum {
	LCD1 = 1,
	LCD2 = 2,
	LCD_BOTH = 3
} LCD_Control;

/*********************************************************************
* Overview: Clipping region control codes to be used with SetClip(...)
*           function. 
*********************************************************************/
#define CLIP_DISABLE       0 	// Disables clipping.
#define CLIP_ENABLE        1	// Enables clipping.


/*********************************************************************
* Macros: RGB565CONVERT(red, green, blue)
*
* Overview: Converts true color into 5:6:5 RGB format.
*
* PreCondition: none
*
* Input: Red, Green, Blue components.
*
* Output: 5 bits red, 6 bits green, 5 bits blue color.
*
* Side Effects: none
*
********************************************************************/
    #define RGB565CONVERT(red, green, blue) (u16) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))
    

/*********************************************************************
* Overview: Some basic colors definitions.
*********************************************************************/
#define BLACK               RGB565CONVERT(0,    0,      0)
#define BRIGHTBLUE          RGB565CONVERT(0,    0,      255)
#define BRIGHTGREEN         RGB565CONVERT(0,    255,    0)
#define BRIGHTCYAN          RGB565CONVERT(0,    255,    255)
#define BRIGHTRED           RGB565CONVERT(255,  0,      0)
#define BRIGHTMAGENTA       RGB565CONVERT(255,  0,      255)
#define BRIGHTYELLOW        RGB565CONVERT(255,  255,    0)
#define BLUE                RGB565CONVERT(0,    0,      128)
#define GREEN               RGB565CONVERT(0,    128,    0)
#define CYAN                RGB565CONVERT(0,    128,    128)
#define RED                 RGB565CONVERT(128,  0,      0)
#define MAGENTA             RGB565CONVERT(128,  0,      128)
#define BROWN               RGB565CONVERT(255,  128,    0)
#define LIGHTGRAY           RGB565CONVERT(128,  128,    128)
#define DARKGRAY            RGB565CONVERT(64,   64,     64)
#define LIGHTBLUE           RGB565CONVERT(128,  128,    255)
#define LIGHTGREEN          RGB565CONVERT(128,  255,    128)
#define LIGHTCYAN           RGB565CONVERT(128,  255,    255)
#define LIGHTRED            RGB565CONVERT(255,  128,    128)
#define LIGHTMAGENTA        RGB565CONVERT(255,  128,    255)
#define YELLOW              RGB565CONVERT(255,  255,    128)
#define WHITE               RGB565CONVERT(255,  255,    255)

/** 
  * @brief  LCD color  
  */ 
#define LCD_COLOR_WHITE          0xFFF0
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0
                            
#define GRAY0       	    RGB565CONVERT(224,  224,    224)
#define GRAY1         	    RGB565CONVERT(192,  192,    192)   
#define GRAY2               RGB565CONVERT(160,  160,    160)   
#define GRAY3               RGB565CONVERT(128,  128,    128)
#define GRAY4               RGB565CONVERT(96,   96,     96)
#define GRAY5               RGB565CONVERT(64,   64,     64)
#define GRAY6	            RGB565CONVERT(32,   32,     32)


/*********************************************************************
* Overview: Clipping region control and border settings.
*********************************************************************/
// Clipping region enable control
extern u16 _clipRgn;

// Left clipping region border
extern u16 _clipLeft;
// Top clipping region border
extern u16 _clipTop;
// Right clipping region border
extern u16 _clipRight;
// Bottom clipping region border
extern u16 _clipBottom;


// Active and Visual Pages 
extern u8 _activePage;
extern u8	_visualPage;


/*----- Low layer function -----*/
void LCD_EnableDisplayControl(LCD_Control SelectedLCDs);
void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);
void LCD_Init(void);

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteIndex(uint16_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
inline void LCD_WriteRAM_Inline(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);

/*********************************************************************
* Macros:  LCD_WriteCommand(cmd)
*
* PreCondition: 
*
* Input: cmd - controller command
*
* Output: none
*
* Side Effects: none
*
* Overview: writes command
*
* Note: none
********************************************************************/
//#define LCD_WriteCommand(cmd) {Clr_Rs; GPIOB->ODR=((GPIOB->ODR&0x00ff)|(cmd<<8)); GPIOC->ODR=((GPIOC->ODR&0xff00)|(cmd>>8)); Clr_Cs; Clr_nWr; Set_nWr; Set_Cs;};
#define LCD_WriteCommand(cmd) {LCD_WriteIndex(cmd);}
	
	

/*********************************************************************
* Function:  void  LCD_WriteData(WORD data)
*
* PreCondition: 
*
* Input:  value - value to be written in WORD format
*
* Output: none
*
* Side Effects: none
*
* Overview: 
********************************************************************/
//#define LCD_WriteData(data) {Set_Rs; GPIOB->ODR=((GPIOB->ODR&0x00ff)|(data<<8)); GPIOC->ODR=((GPIOC->ODR&0xff00)|(data>>8));  Clr_nWr;  Set_nWr;};
#define LCD_WriteData(data) {LCD_WriteRAM(data);}

/*********************************************************************
* Function:  void ResetDevice()
*
* Overview: Initializes LCD module.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void LCD_ResetDevice(void);

/*********************************************************************
* Macros:  GetMaxX()
*
* Overview: Returns maximum horizontal coordinate for visual display
*
* PreCondition: none
*
* Input: none
*
* Output: Maximum horizontal coordinates.
*
* Side Effects: none
*
* Note: 
********************************************************************/
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)

#define	GetMaxX() 			(DISP_VER_RESOLUTION-1)
	

#elif (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)

#define	GetMaxX() 			(DISP_HOR_RESOLUTION-1)

#endif

/*********************************************************************
* Macros:  GetMaxY()
*
* Overview: Returns maximum vertical coordinate for visual display
*			
* PreCondition: none
*
* Input: none
*
* Output: Maximum vertical coordinates.
*
* Side Effects: none
*
* Note: 
********************************************************************/
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)

#define	GetMaxY() 			(DISP_HOR_RESOLUTION-1)
#define ORIENTATION_REGISTER	0x0060  // Vertical rotation	 - A[5]=1; A[6]=0; A[7]=1;

#elif (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)

#define	GetMaxY() 			(DISP_VER_RESOLUTION-1)
#define ORIENTATION_REGISTER	0x0000  // Horizontal rotation   - A[5]=0; A[6]=0; A[7]=0;

#endif

// Definition for memory page size with virtual display taken into account
// The maximum memory space of SSD1928 is 262kbyte
#ifndef PAGE_MEM_SIZE
// Memory allocation for a single page 
#define PAGE_MEM_SIZE 	(GetMaxX()+1)*(GetMaxY()+1)*COLOR_DEPTH/8

#if PAGE_MEM_SIZE > 1215000ul	//this is the memory boundary of ssd1963
#error MEMORY ALLOCATION FAILED. CHECK SSD1963.h
#endif

#endif

/*********************************************************************
* Macros:  SetActivePage(page)
*
* Overview: Sets active graphic page.
*
* PreCondition: none
*
* Input: page - Graphic page number.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define LCD_SetActivePage(page)	_activePage = page

/*********************************************************************
* Function: void Clear(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: clears screen with current color 
*
* Note: none
*
********************************************************************/
void LCD_Clear(u16 color);
void LCD_ClearArea(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* Overview: Puts pixel with the given x,y coordinate position.
*
* PreCondition: none
*
* Input: x - x position of the pixel.
*		 y - y position of the pixel.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void LCD_PutPixel(u16 x, u16 y, u16 color);

/*********************************************************************
* Macros: SetClipRgn(left, top, right, bottom)
*
* Overview: Sets clipping region.
*
* PreCondition: none
*
* Input: left - Defines the left clipping region border.
*		 top - Defines the top clipping region border.
*		 right - Defines the right clipping region border.
*	     bottom - Defines the bottom clipping region border.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define LCD_SetClipRgn(left,top,right,bottom) _clipLeft=left; _clipTop=top; _clipRight=right; _clipBottom=bottom;

/*********************************************************************
* Macros: GetClipLeft()
*
* Overview: Returns left clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Left clipping border.
*
* Side Effects: none
*
********************************************************************/
#define LCD_GetClipLeft() _clipLeft

/*********************************************************************
* Macros: GetClipRight()
*
* Overview: Returns right clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Right clipping border.
*
* Side Effects: none
*
********************************************************************/
#define LCD_GetClipRight() _clipRight

/*********************************************************************
* Macros: GetClipTop()
*
* Overview: Returns top clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Top clipping border.
*
* Side Effects: none
*
********************************************************************/
#define LCD_GetClipTop() _clipTop

/*********************************************************************
* Macros: GetClipBottom()
*
* Overview: Returns bottom clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Bottom clipping border.
*
* Side Effects: none
*
********************************************************************/
#define LCD_GetClipBottom() _clipBottom

/*********************************************************************
* Macros: SetClip(control)
*
* Overview: Enables/disables clipping.
*
* PreCondition: none
*
* Input: control - Enables or disables the clipping.
*			- 0: Disable clipping
*			- 1: Enable clipping
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define LCD_SetClip(control) _clipRgn=control;

/*********************************************************************
* Macros: IsDeviceBusy()
*
* Overview: Returns non-zero if LCD controller is busy 
*           (previous drawing operation is not completed).
*
* PreCondition: none
*
* Input: none
*
* Output: Busy status.
*
* Side Effects: none
*
********************************************************************/
#define LCD_IsDeviceBusy()  0

/*********************************************************************
* Macros: SetPalette(colorNum, color)
*
* Overview:  Sets palette register.
*
* PreCondition: none
*
* Input: colorNum - Register number.
*        color - Color.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define LCD_SetPalette(colorNum, color)

/*********************************************************************
* Function:  LCD_SetArea(start_x,start_y,end_x,end_y)
*
* PreCondition: SetActivePage(page)
*
* Input: start_x, end_x	- start column and end column
*		 start_y,end_y 	- start row and end row position (i.e. page address)
*
* Output: none
*
* Side Effects: none
*
* Overview: defines start/end columns and start/end rows for memory access
*			from host to SSD1963
* Note: none
********************************************************************/
void LCD_SetArea(u16 start_x, u16 start_y, u16 end_x, u16 end_y);

/*********************************************************************
* Function:  SetScrollArea(SHORT top, SHORT scroll, SHORT bottom)
*
* PreCondition: none
*
* Input: top - Top Fixed Area in number of lines from the top
*				of the frame buffer
*		 scroll - Vertical scrolling area in number of lines
*		 bottom - Bottom Fixed Area in number of lines
*
* Output: none
*
* Side Effects: none
*
* Overview:
*
* Note: Reference: section 9.22 Set Scroll Area, SSD1963 datasheet Rev0.20
*
********************************************************************/
void LCD_SetScrollArea(u16 top, u16 scroll, u16 bottom);

/*********************************************************************
* Function:  void  SetScrollStart(SHORT line)
*
* Overview: First, we need to define the scrolling area by SetScrollArea()
*			before using this function. 
*
* PreCondition: SetScrollArea(SHORT top, SHORT scroll, SHORT bottom)
*
* Input: line - Vertical scrolling pointer (in number of lines) as 
*		 the first display line from the Top Fixed Area defined in SetScrollArea()
*
* Output: none
*
* Note: Example -
*
*		SHORT line=0;
*		SetScrollArea(0,272,272);
*		for(line=0;line<272;line++) {SetScrollStart(line);DelayMs(100);}
*		
*		Code above scrolls the whole page upwards in 100ms interval 
*		with page 2 replacing the first page in scrolling
********************************************************************/
void LCD_SetScrollStart(u16 line);

/*********************************************************************
* Function:  void EnterSleepMode (void)
* PreCondition: none
* Input:  none
* Output: none
* Side Effects: none
* Overview: SSD1963 enters sleep mode
* Note: Host must wait 5mS after sending before sending next command
********************************************************************/
void LCD_EnterSleepMode (void);

/*********************************************************************
* Function:  void ExitSleepMode (void)
* PreCondition: none
* Input:  none
* Output: none
* Side Effects: none
* Overview: SSD1963 exits sleep mode
* Note:   cannot be called sooner than 15
********************************************************************/
void LCD_ExitSleepMode (void);


/*********************************************************************
* Function:  void  SetBacklight(BYTE intensity)
*
* Overview: This function makes use of PWM feature of ssd1963 to adjust
*			the backlight intensity. 
*
* PreCondition: Backlight circuit with shutdown pin connected to PWM output of ssd1963.
*
* Input: 	(BYTE) intensity from 
*			0x00 (total backlight shutdown, PWM pin pull-down to VSS)
*			0xff (99% pull-up, 255/256 pull-up to VDD)
*
* Output: none
*
* Note: The base frequency of PWM set to around 300Hz with PLL set to 120MHz.
*		This parameter is hardware dependent
********************************************************************/
u8 BacklightIntensity;
void LCD_SetBacklight(u8 intensity);


#define FADE_DOWN 0
#define FADE_UP 1
#define FADE_TO_NORMAL 2
void LCD_FadeBacklight(u8 fade);

/*********************************************************************
* Function:  void  SetTearingCfg(BOOL state, BOOL mode)
*
* Overview: This function enable/disable tearing effect
*
* PreCondition: none
*
* Input: 	BOOL state -	1 to enable
*							0 to disable
*			BOOL mode -		0:  the tearing effect output line consists
*								of V-blanking information only
*							1:	the tearing effect output line consists
*								of both V-blanking and H-blanking info.
* Output: none
*
* Note:
********************************************************************/
void LCD_SetTearingCfg(bool state, bool mode);


#define DIR_TOP_TO_BOTTOM 0
#define DIR_BOTTOM_TO_TOP 1
void LCD_SetAddressOrder(bool direction);


void LCD_Delay(vu32 nCount);


/************************************************************************
* Macro: Lo                                                             *
* Preconditions: None                                                   *
* Overview: This macro extracts a low byte from a 2 byte word.          *
* Input: None.                                                          *
* Output: None.                                                         *
************************************************************************/
#define Lo(X)   (u8)(X&0x00ff)

/************************************************************************
* Macro: Hi                                                             *
* Preconditions: None                                                   *
* Overview: This macro extracts a high byte from a 2 byte word.         *
* Input: None.                                                          *
* Output: None.                                                         *
************************************************************************/
#define Hi(X)   (u8)((X>>8)&0x00ff)


#endif // _SSD1963_H
