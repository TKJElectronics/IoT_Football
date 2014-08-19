#include "stm32f4xx.h"
#include "SSD1963_touch.h"

OS_SemaphoreHandle SPI_Mutex;

/**
  * @brief  Displays a Cross.
  * @param  Xpos: specifies the left X position.
  * @param  Ypos: specifies the botton Y position.
  * @retval None
  */
void LCD_Cross(uint16_t Ypos, uint16_t Xpos)
{
  LCD_Line(Xpos - 30, Ypos, Xpos + 30, Ypos, BLACK); /* Horizontal Line */
  LCD_Line(Xpos, Ypos - 30, Xpos, Ypos + 30, BLACK);   /* Vertical Line   */
}


void LCD_Touch_Config(void)
{
  uint8_t i;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  SPI_Mutex = OS_SemaphoreCreateMutex();

  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // SCK
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // MOSI
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // CS
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // CS
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // MISO
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // IRQ1
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // IRQ2
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  TOUCH_CS_SET(LCD1_TOUCH_CS_PIN);
  TOUCH_CS_SET(LCD2_TOUCH_CS_PIN);
}

void Touch_Write(uint8_t out, uint16_t CS_Pin)
{
	uint8_t i;

	TOUCH_CS_SET(CS_Pin);
	TOUCH_MOSI_RESET;
	TOUCH_SCK_RESET;
	TOUCH_CS_RESET(CS_Pin);

	for (i = 0; i < 8; i++) {
		if ((out >> (7-i)) & 0x1) {
			TOUCH_MOSI_SET;
		} else {
			TOUCH_MOSI_RESET;
		}
		LCD_Delay(5);
		TOUCH_SCK_SET;
		LCD_Delay(5);
		TOUCH_SCK_RESET;
	}
}

uint16_t Touch_Read(uint16_t CS_Pin)
{
	uint8_t i;
	uint16_t temp = 0;

	TOUCH_MOSI_RESET;
	TOUCH_SCK_SET;
	for (i = 0; i < 16; i++) {
		LCD_Delay(5);
        TOUCH_SCK_RESET;
        LCD_Delay(5);
        if (TOUCH_MISO == Bit_SET && i < 12) {
        	temp |= (1 << (11-i));
        }
        LCD_Delay(5);
        TOUCH_SCK_SET;
	}
	TOUCH_CS_SET(CS_Pin);

	return temp;
}

uint16_t Touch_ReadX(uint8_t LCD)
{
	uint16_t CS_Pin;
	Touch_Lock();

	if (LCD == 1) CS_Pin = LCD1_TOUCH_CS_PIN;
	else if (LCD == 2) CS_Pin = LCD2_TOUCH_CS_PIN;

	Touch_Write(CHX, CS_Pin);
	LCD_Delay(50);

	Touch_Unlock();

	return Touch_Read(CS_Pin);
}

uint16_t Touch_ReadY(uint8_t LCD)
{
	uint16_t CS_Pin;

	Touch_Lock();

	if (LCD == 1) CS_Pin = LCD1_TOUCH_CS_PIN;
	else if (LCD == 2) CS_Pin = LCD2_TOUCH_CS_PIN;

	Touch_Write(CHY, CS_Pin);
	LCD_Delay(50);

	Touch_Unlock();

	return Touch_Read(CS_Pin);
}

uint16_t Touch_ReadAvgX(uint8_t measurements, uint8_t LCD)
{
	uint8_t i;
	uint32_t avg = 0;
	for (i = 0; i < measurements; i++) {
		avg += Touch_ReadX(LCD);
		LCD_Delay(100);
	}
	avg /= measurements;

	return avg;
}

uint16_t Touch_ReadAvgY(uint8_t measurements, uint8_t LCD)
{
	uint8_t i;
	uint32_t avg = 0;
	for (i = 0; i < measurements; i++) {
		avg += Touch_ReadY(LCD);
		LCD_Delay(100);
	}
	avg /= measurements;

	return avg;
}
