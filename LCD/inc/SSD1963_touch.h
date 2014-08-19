#ifndef __SSD1963_TOUCH_H
#define __SSD1963_TOUCH_H

#include "stm32f4xx.h"
#include "SSD1963.h"

#include "Os_Wrap.h"
extern OS_SemaphoreHandle SPI_Mutex;

#define Touch_Lock() OS_SemaphoreTake(SPI_Mutex,OS_MAX_DELAY)
#define Touch_Unlock() OS_SemaphoreGive(SPI_Mutex)

#define	CHX 	0x90
#define	CHY 	0xd0

#define LCD1_TOUCH_CS_PIN		GPIO_Pin_2	// PE2
#define LCD2_TOUCH_CS_PIN		GPIO_Pin_4	// PE4

#define TOUCH_SCK_SET		GPIO_SetBits(GPIOE, GPIO_Pin_6);
#define TOUCH_SCK_RESET		GPIO_ResetBits(GPIOE, GPIO_Pin_6);

#define TOUCH_MOSI_SET		GPIO_SetBits(GPIOC, GPIO_Pin_3);
#define TOUCH_MOSI_RESET	GPIO_ResetBits(GPIOC, GPIO_Pin_3);

#define TOUCH_CS_SET(x)		GPIOE->BSRRL = x; //GPIO_SetBits(GPIOE, LCD1_TOUCH_CS_PIN); GPIO_SetBits(GPIOE, LCD1_TOUCH_CS_PIN);
#define TOUCH_CS_RESET(x)	GPIOE->BSRRH = x; //GPIO_ResetBits(GPIOE, GPIO_Pin_4);

#define TOUCH_MISO			GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)

#define LCD1_TOUCH_IRQ		!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_10)
#define LCD2_TOUCH_IRQ		!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)

void LCD_Cross(uint16_t Ypos, uint16_t Xpos);

void LCD_Touch_Config(void);

void Touch_Write(uint8_t out, uint16_t CS_Pin);
uint16_t Touch_Read(uint16_t CS_Pin);

uint16_t Touch_ReadX(uint8_t LCD);
uint16_t Touch_ReadY(uint8_t LCD);
uint16_t Touch_ReadAvgX(uint8_t measurements, uint8_t LCD);
uint16_t Touch_ReadAvgY(uint8_t measurements, uint8_t LCD);

#endif /* __SSD1963_TOUCH_H */
