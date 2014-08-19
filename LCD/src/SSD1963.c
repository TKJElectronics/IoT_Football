#include "stm32f4xx.h"
#include "SSD1963.h"

/* RTOS includes. */
#include "Os_Wrap.h"
OS_SemaphoreHandle gLCD_Mutex;			//lcd²Ù×÷»¥³âÁ¿

// Color
u16  _color;
// Clipping region control
u16 _clipRgn;
// Clipping region borders
u16 _clipLeft;
u16 _clipTop;
u16 _clipRight;
u16 _clipBottom;

// Active Page
u8  _activePage;
// Visual Page
u8  _visualPage;

// ssd1963 specific
u8 _gpioStatus = 0;

uint16_t LCD1_Current_CS_Pin, LCD2_Current_CS_Pin;


void LCD_SetArea(u16 start_x, u16 start_y, u16 end_x, u16 end_y);



/*********************************************************************
* Macros:  PMPWaitBusy()
*
* Overview: waits for PMP cycle end.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Note: 
********************************************************************/
#define PMPWaitBusy()  Nop();


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
void LCD_SetArea(u16 start_x, u16 start_y, u16 end_x, u16 end_y)
{
	uint16_t temp;
	uint16_t start_x1, end_x1;
	uint16_t start_y1, end_y1;
	long offset;

	offset = (u16)_activePage*(GetMaxY()+1);

#if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
	start_x1 = start_x;
	end_x1 = end_x;
	start_y1 = start_y;
	end_y1 = end_y;
#elif (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
	start_x1 = GetMaxY() - start_y;
	end_x1 = GetMaxY() - end_y;
	start_y1 = start_x;
	end_y1 = end_x;
#endif

	if (start_x1 > end_x1) {
		temp = end_x1;
		end_x1 = start_x1;
		start_x1 = temp;
	}
	if (start_y1 > end_y1) {
		temp = end_y1;
		end_y1 = start_y1;
		start_y1 = temp;
	}

	start_y1 = offset + start_y1;
	end_y1   = offset + end_y1;

	LCD_WriteCommand(CMD_SET_COLUMN);
	LCD_WriteData(start_x1>>8);
	LCD_WriteData(start_x1);
	LCD_WriteData(end_x1>>8);
	LCD_WriteData(end_x1);
	LCD_WriteCommand(CMD_SET_PAGE);
	LCD_WriteData(start_y1>>8);
	LCD_WriteData(start_y1);
	LCD_WriteData(end_y1>>8);
	LCD_WriteData(end_y1);
}

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
********************************************************************/
void LCD_SetScrollArea(u16 top, u16 scroll, u16 bottom)
{
	LCD_WriteCommand(CMD_SET_SCROLL_AREA);
	LCD_WriteData(top>>8);
	LCD_WriteData(top);
	LCD_WriteData(scroll>>8);
	LCD_WriteData(scroll);
	LCD_WriteData(bottom>>8);
	LCD_WriteData(bottom);	
}

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
*		SetScrollArea(0,272,0);
*		for(line=0;line<272;line++) {SetScrollStart(line);Delay_Ms(100);}
*		
*		Code above scrolls the whole page upwards in 100ms interval 
*		with page 2 replacing the first page in scrolling
********************************************************************/
void LCD_SetScrollStart(u16 line)
{
	LCD_WriteCommand(CMD_SET_SCROLL_START);
	LCD_WriteData(line>>8);
	LCD_WriteData(line);	
}

/*********************************************************************
* Function:  void EnterSleepMode (void)
* PreCondition: none
* Input:  none
* Output: none
* Side Effects: none
* Overview: SSD1963 enters sleep mode
* Note: Host must wait 5mS after sending before sending next command
********************************************************************/
void LCD_EnterSleepMode (void)
{
	LCD_WriteCommand(CMD_ENT_SLEEP);
}

/*********************************************************************
* Function:  void ExitSleepMode (void)
* PreCondition: none
* Input:  none
* Output: none
* Side Effects: none
* Overview: SSD1963 enters sleep mode
* Note:   none
********************************************************************/
void LCD_ExitSleepMode (void)
{
	LCD_WriteCommand(CMD_EXIT_SLEEP);
}

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
void LCD_SetBacklight(u8 intensity)
{  
	LCD_WriteCommand(0xBE);			// Set PWM configuration for backlight control
	LCD_WriteData(0x0E);			// PWMF[7:0] = 2, PWM base freq = PLL/(256*(1+5))/256 = 
								// 300Hz for a PLL freq = 120MHz
	LCD_WriteData(intensity);		// Set duty cycle, from 0x00 (total pull-down) to 0xFF 
								// (99% pull-up , 255/256)
	LCD_WriteData(0x01);			// PWM enabled and controlled by host (mcu)
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);

    /*if (intensity > 0) {
        GPIO_SetBits(GPIOA,GPIO_Pin_8);
    } else {
        GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    }*/

    BacklightIntensity = intensity;
}

void LCD_FadeBacklight(u8 fade) 
{  
  u8 i;
  if (fade == FADE_DOWN) {
    for (i = 255; i > 0; i--) {
        LCD_SetBacklight(i);
        LCD_Delay(20000);
    }   
    LCD_SetBacklight(0);    
  } else if (fade == FADE_UP) {
    for (i = 0; i < 255; i++) {
        LCD_SetBacklight(i);
        LCD_Delay(20000);
    }  
    LCD_SetBacklight(255);
  } else if (fade == FADE_TO_NORMAL) {
    if (BacklightIntensity < 255) {
        for (i = BacklightIntensity; i < 255; i++) {
            LCD_SetBacklight(i);
            LCD_Delay(20000);
        }  
    } else if (BacklightIntensity > 255) {
        for (i = BacklightIntensity; i > 255; i--) {
            LCD_SetBacklight(i);
            LCD_Delay(20000);
        }  
    }
    LCD_SetBacklight(255);
  }  
}

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
void LCD_SetTearingCfg(bool state, bool mode)
{
	if(state == 1)
	{
		LCD_WriteCommand(0x35);
		LCD_WriteData(mode&0x01);
	}
	else
	{
		LCD_WriteCommand(0x34);
	}
}

void LCD_EnableDisplayControl(LCD_Control SelectedLCDs)
{
	if (SelectedLCDs & LCD1) LCD1_Current_CS_Pin = LCD1_CS_PIN;
	else LCD1_Current_CS_Pin = 0;

	if (SelectedLCDs & LCD2) LCD2_Current_CS_Pin = LCD2_CS_PIN;
	else LCD2_Current_CS_Pin = 0;

	GPIOD->BSRRL = LCD1_CS_PIN; // CS_HIGH
	GPIOG->BSRRL = LCD2_CS_PIN; // CS_HIGH
}


/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
 		  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

 		/*-- GPIO Configuration ------------------------------------------------------*/
 		  /* SRAM Data lines,  NOE and NWE configuration */
 		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
 		                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 |
 		                                GPIO_Pin_4 |GPIO_Pin_5;;
 		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 		  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 		  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 		  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

 		  GPIO_Init(GPIOD, &GPIO_InitStructure);

 		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
 		                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
 		                                GPIO_Pin_15;
 		  GPIO_Init(GPIOE, &GPIO_InitStructure);


 		  /* SRAM Address lines configuration (PD11 = A16) */
 		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
 		  GPIO_Init(GPIOD, &GPIO_InitStructure);

 		  /* NE1 configuration */
 		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
 		  GPIO_Init(GPIOD, &GPIO_InitStructure);

 		  /* NE2 configuration */
 		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 		  GPIO_Init(GPIOG, &GPIO_InitStructure);

 		  GPIOD->BSRRL = GPIO_Pin_4; // RD_HIGH
 		  GPIOD->BSRRL = GPIO_Pin_5; // WR_HIGH
 		  GPIOD->BSRRL = GPIO_Pin_11; // RS_HIGH
 		  GPIOD->BSRRL = LCD1_CS_PIN; // CS_HIGH
 		  GPIOG->BSRRL = LCD2_CS_PIN; // CS_HIGH
  
	/* LCD1 Reset line */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* LCD2 Reset line */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void LCD_Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}

/*********************************************************************
* Function:  void ResetDevice()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: Resets LCD, initializes PMP
*			Initialize low level IO port for mcu,
*			initialize SSD1963 for PCLK,
*			HSYNC, VSYNC etc
*
* Note: Need to set the backlight intensity by SetBacklight(BYTE intensity)
*		in main()
*
********************************************************************/
void LCD_Init(void)
{
  gLCD_Mutex=OS_SemaphoreCreateMutex();

  /* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();

  /* Configure the FSMC Parallel interface -------------------------------------*/
  //LCD_FSMCConfig();

  GPIOD->BSRRH = GPIO_Pin_6; // LCD1_RESET LOW
  GPIOE->BSRRH = GPIO_Pin_0; // LCD2_RESET LOW
  LCD_Delay(20000);
  GPIOD->BSRRL = GPIO_Pin_6; // LCD1_RESET HIGH
  GPIOE->BSRRL = GPIO_Pin_0; // LCD2_RESET HIGH
  LCD_Delay(50000); /* delay 50 ms */



  LCD_EnableDisplayControl(LCD_BOTH);

	//Set MN(multipliers) of PLL, VCO = crystal freq * (N+1)
	//PLL freq = VCO/M with 250MHz < VCO < 800MHz
	//The max PLL freq is around 120MHz. To obtain 120MHz as the PLL freq


	LCD_WriteCommand(0xE2);				// Set PLL with OSC = 10MHz (hardware)
									// Multiplier N = 35, VCO (>250MHz)= OSC*(N+1), VCO = 360MHz
	LCD_WriteData(0x23);
	LCD_WriteData(0x02);				// Divider M = 2, PLL = 360/(M+1) = 120MHz
	LCD_WriteData(0x04);				// Validate M and N values*/

	LCD_WriteCommand(0xE0);				// Start PLL command
	LCD_WriteData(0x01);				// enable PLL
    
	LCD_Delay(10000);						// wait stablize
	LCD_WriteCommand(0xE0);				// Start PLL command again
	LCD_WriteData(0x03);				// now, use PLL output as system clock	

	LCD_WriteCommand(0x01);				// Soft reset
	LCD_Delay(50000);

	//Set LSHIFT freq, i.e. the DCLK with PLL freq 120MHz set previously
	//Typical DCLK for display is between 9MHz and 15MHz
	//10MHz = 120MHz*(LCDC_FPR+1)/2^20
	//LCDC_FPR = 87380 (0x15554)
	/*LCD_WriteCommand(0xE6);
	LCD_WriteData(0x01);
	LCD_WriteData(0x55);
	LCD_WriteData(0x54);*/
	LCD_WriteCommand(0xE6);
	LCD_WriteData(0x03);
	LCD_WriteData(0xff);
	LCD_WriteData(0xff);
	
	//Set panel mode, varies from individual manufacturer
	LCD_WriteCommand(0xB0);
	LCD_WriteData(0x0027);
	LCD_WriteData(0x00);				// set TTL mode
	LCD_WriteData((HDP>>8)&0X00FF);  //Set HDP
	LCD_WriteData(HDP&0X00FF);
	LCD_WriteData((VDP>>8)&0X00FF);  //Set VDP
	LCD_WriteData(VDP&0X00FF);
	LCD_WriteData(0x0000);			//RGB sequence

	//Set horizontal period
	LCD_WriteCommand(0xB4); //HSYNC -> Set HT
	LCD_WriteData((HT>>8)&0X00FF);
	LCD_WriteData(HT&0X00FF);
	LCD_WriteData((HPS>>8)&0X00FF);  //Set HPS
	LCD_WriteData(HPS&0X00FF);
	LCD_WriteData(HPW);			   //Set HPW
	LCD_WriteData((LPS>>8)&0X00FF);  //Set HPS
	LCD_WriteData(LPS&0X00FF);
	LCD_WriteData(0x0000);

	//Set vertical period
	LCD_WriteCommand(0xB6); //VSYNC -> //Set VT
	LCD_WriteData((VT>>8)&0X00FF);
	LCD_WriteData(VT&0X00FF);
	LCD_WriteData((VPS>>8)&0X00FF);  //Set VPS
	LCD_WriteData(VPS&0X00FF);
	LCD_WriteData(VPW);			   //Set VPW
	LCD_WriteData((FPS>>8)&0X00FF);  //Set FPS
	LCD_WriteData(FPS&0X00FF);

	LCD_WriteCommand(0xBA);
	LCD_WriteData(0x0F);    //GPIO[3:0] out 1

	LCD_WriteCommand(0xB8);
	LCD_WriteData(0x07);    //GPIO3=input, GPIO[2:0]=output
	LCD_WriteData(0x01);    //GPIO0 normal	
	
	LCD_WriteCommand(0x0036);
	LCD_WriteData(ORIENTATION_REGISTER);

	//Set pixel data interface
	LCD_WriteCommand(0xF0);
	LCD_WriteData(0x03);				//16-bit(565 format) data for 16bpp PIC32MX only
	//LCD_WriteData(0x00);				//8-bit data for 16bpp, PIC24 series

	LCD_WriteCommand(0x29);				// Turn on display; show the image on display
	
	LCD_WriteCommand(0xd0);
	LCD_WriteData(0x0d);		

	LCD_Delay(50000);

	LCD_SetBacklight(0);
}

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* PreCondition: none
*
* Input: x,y - pixel coordinates
*
* Output: none
*
* Side Effects: none
*
* Overview: puts pixel
*
* Note: 
********************************************************************/
void LCD_PutPixel(u16 x, u16 y, u16 color)
{
	LCD_SetArea(x,y,GetMaxX(),GetMaxY());
	LCD_WriteCommand(CMD_WR_MEMSTART);
	LCD_WriteData(color);
}

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
void LCD_Clear(u16 color)
{
	u32 xcounter, ycounter;

	LCD_SetArea(0,0,GetMaxX(),GetMaxY());

	LCD_WriteCommand(CMD_WR_MEMSTART);

	for(ycounter=0;ycounter<(GetMaxY()+1);ycounter++)
	{
		for(xcounter=0;xcounter<(GetMaxX()+1);xcounter++)
		{
			LCD_WriteRAM_Inline(color);
		}
	}
}

void LCD_ClearArea(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
	u32 width, height;
	u32 xcounter, ycounter;

	width = x1 - x0;
	height = y1 - y0;
	LCD_SetArea(x0, y0, x1, y1);

	LCD_WriteCommand(CMD_WR_MEMSTART);

	for(ycounter=0;ycounter<(height+1);ycounter++)
	{
		for(xcounter=0;xcounter<(width+1);xcounter++)
		{
			LCD_WriteRAM_Inline(color);
		}
	}
}

void LCD_SetAddressOrder(bool direction) // Down: direction == true - Up: direction == false
{
    uint8_t tmp;
    if (direction == DIR_TOP_TO_BOTTOM) {
        tmp = ORIENTATION_REGISTER; //LCD_ReadReg(CMD_SET_ADDR_MODE);
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
        tmp |= 0x40; // Set Bit 6 in CMD_SET_ADDR_MODE register
#elif (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
        tmp &= 0x7F; // Clear Bit 7 in CMD_SET_ADDR_MODE register
#endif
        LCD_WriteReg(CMD_SET_ADDR_MODE, tmp);
    } else if (direction == DIR_BOTTOM_TO_TOP) {      
        tmp = ORIENTATION_REGISTER; //LCD_ReadReg(CMD_SET_ADDR_MODE);
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
        tmp &= 0xBF; // Clear Bit 6 in CMD_SET_ADDR_MODE register
#elif (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
        tmp |= 0x80; // Set Bit 7 in CMD_SET_ADDR_MODE register
#endif
        LCD_WriteReg(CMD_SET_ADDR_MODE, tmp);
    }
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  //LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  //LCD_RAM = LCD_RegValue;
	  GPIOD->BSRRH = LCD1_Current_CS_Pin; // CS_LOW
	  GPIOG->BSRRH = LCD2_Current_CS_Pin; // CS_LOW
	  GPIOD->BSRRH = GPIO_Pin_11; // RS_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  // Set output
	  GPIOD->ODR = (GPIOD->ODR & 0x38FC) | ((LCD_Reg&1) << 14) | ((LCD_Reg&2) << 14) | ((LCD_Reg&4) >> 2) | ((LCD_Reg&8) >> 2) | ((LCD_Reg&0x8000) >> 5) | ((LCD_Reg&0x4000) >> 5) | ((LCD_Reg&0x2000) >> 5);
	  GPIOE->ODR = (GPIOE->ODR & 0x007F) | ((LCD_Reg&16) << 3) | ((LCD_Reg&32) << 3) | ((LCD_Reg&64) << 3) | ((LCD_Reg&128) << 3) | ((LCD_Reg&256) << 3) | ((LCD_Reg&512) << 3) | ((LCD_Reg&1024) << 3) | ((LCD_Reg&2048) << 3) | ((LCD_Reg&4096) << 3);
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRH = GPIO_Pin_5; // WR_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_5; // WR_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_11; // RS_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  // Set output
	  GPIOD->ODR = (GPIOD->ODR & 0x38FC) | ((LCD_RegValue&1) << 14) | ((LCD_RegValue&2) << 14) | ((LCD_RegValue&4) >> 2) | ((LCD_RegValue&8) >> 2) | ((LCD_RegValue&0x8000) >> 5) | ((LCD_RegValue&0x4000) >> 5) | ((LCD_RegValue&0x2000) >> 5);
	  GPIOE->ODR = (GPIOE->ODR & 0x007F) | ((LCD_RegValue&16) << 3) | ((LCD_RegValue&32) << 3) | ((LCD_RegValue&64) << 3) | ((LCD_RegValue&128) << 3) | ((LCD_RegValue&256) << 3) | ((LCD_RegValue&512) << 3) | ((LCD_RegValue&1024) << 3) | ((LCD_RegValue&2048) << 3) | ((LCD_RegValue&4096) << 3);
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRH = GPIO_Pin_5; // WR_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_5; // WR_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = LCD1_Current_CS_Pin; // CS_HIGH
	  GPIOG->BSRRL = LCD2_Current_CS_Pin; // CS_HIGH
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  //LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  //return (LCD_RAM);
	return 0;
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
  //LCD_REG = 0x2C;
	uint16_t LCD_Reg = 0x2C;
	  GPIOD->BSRRH = LCD1_Current_CS_Pin; // CS_LOW
	  GPIOG->BSRRH = LCD2_Current_CS_Pin; // CS_LOW
	  GPIOD->BSRRH = GPIO_Pin_11; // RS_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  // Set output
	  GPIOD->ODR = (GPIOD->ODR & 0x38FC) | ((LCD_Reg&1) << 14) | ((LCD_Reg&2) << 14) | ((LCD_Reg&4) >> 2) | ((LCD_Reg&8) >> 2) | ((LCD_Reg&0x8000) >> 5) | ((LCD_Reg&0x4000) >> 5) | ((LCD_Reg&0x2000) >> 5);
	  GPIOE->ODR = (GPIOE->ODR & 0x007F) | ((LCD_Reg&16) << 3) | ((LCD_Reg&32) << 3) | ((LCD_Reg&64) << 3) | ((LCD_Reg&128) << 3) | ((LCD_Reg&256) << 3) | ((LCD_Reg&512) << 3) | ((LCD_Reg&1024) << 3) | ((LCD_Reg&2048) << 3) | ((LCD_Reg&4096) << 3);
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRH = GPIO_Pin_5; // WR_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_5; // WR_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_11; // RS_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteIndex(uint16_t LCD_Reg)
{
  /* Write 16-bit Index */
  //LCD_REG = LCD_Reg;
	  GPIOD->BSRRH = LCD1_Current_CS_Pin; // CS_LOW
	  GPIOG->BSRRH = LCD2_Current_CS_Pin; // CS_LOW
	  GPIOD->BSRRH = GPIO_Pin_11; // RS_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  // Set output
	  GPIOD->ODR = (GPIOD->ODR & 0x38FC) | ((LCD_Reg&1) << 14) | ((LCD_Reg&2) << 14) | ((LCD_Reg&4) >> 2) | ((LCD_Reg&8) >> 2) | ((LCD_Reg&0x8000) >> 5) | ((LCD_Reg&0x4000) >> 5) | ((LCD_Reg&0x2000) >> 5);
	  GPIOE->ODR = (GPIOE->ODR & 0x007F) | ((LCD_Reg&16) << 3) | ((LCD_Reg&32) << 3) | ((LCD_Reg&64) << 3) | ((LCD_Reg&128) << 3) | ((LCD_Reg&256) << 3) | ((LCD_Reg&512) << 3) | ((LCD_Reg&1024) << 3) | ((LCD_Reg&2048) << 3) | ((LCD_Reg&4096) << 3);
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRH = GPIO_Pin_5; // WR_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_5; // WR_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_11; // RS_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM(uint16_t RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  //LCD_RAM = RGB_Code;

	  // Set output
	  GPIOD->ODR = (GPIOD->ODR & 0x38FC) | ((RGB_Code&1) << 14) | ((RGB_Code&2) << 14) | ((RGB_Code&4) >> 2) | ((RGB_Code&8) >> 2) | ((RGB_Code&0x8000) >> 5) | ((RGB_Code&0x4000) >> 5) | ((RGB_Code&0x2000) >> 5);
	  GPIOE->ODR = (GPIOE->ODR & 0x007F) | ((RGB_Code&16) << 3) | ((RGB_Code&32) << 3) | ((RGB_Code&64) << 3) | ((RGB_Code&128) << 3) | ((RGB_Code&256) << 3) | ((RGB_Code&512) << 3) | ((RGB_Code&1024) << 3) | ((RGB_Code&2048) << 3) | ((RGB_Code&4096) << 3);
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRH = GPIO_Pin_5; // WR_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_5; // WR_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif
}

inline void LCD_WriteRAM_Inline(uint16_t RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  //LCD_RAM = RGB_Code;

	  // Set output
	  GPIOD->ODR = (GPIOD->ODR & 0x38FC) | ((RGB_Code&1) << 14) | ((RGB_Code&2) << 14) | ((RGB_Code&4) >> 2) | ((RGB_Code&8) >> 2) | ((RGB_Code&0x8000) >> 5) | ((RGB_Code&0x4000) >> 5) | ((RGB_Code&0x2000) >> 5);
	  GPIOE->ODR = (GPIOE->ODR & 0x007F) | ((RGB_Code&16) << 3) | ((RGB_Code&32) << 3) | ((RGB_Code&64) << 3) | ((RGB_Code&128) << 3) | ((RGB_Code&256) << 3) | ((RGB_Code&512) << 3) | ((RGB_Code&1024) << 3) | ((RGB_Code&2048) << 3) | ((RGB_Code&4096) << 3);
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRH = GPIO_Pin_5; // WR_LOW
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif

	  GPIOD->BSRRL = GPIO_Pin_5; // WR_HIGH
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;");
#ifdef LCD_LONG_WIRE
	  asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
#endif
}

/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
*******************************************************************************/
uint16_t LCD_ReadRAM(void)
{
  /* Write 16-bit Index (then Read Reg) */
  //LCD_REG = 0x2C; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  //return LCD_RAM;
	return 0;
}
