#ifndef __DEBUGGING_H
#define __DEBUGGING_H

#include "stm32f4xx.h"

#include <stdio.h>
#include <stdarg.h>

/* USART Config parameters */
#define DEBUGGING_USART                        USART1
#define DEBUGGING_USART_CLK                    RCC_APB2Periph_USART1

#define DEBUGGING_USART_TX_PIN                 GPIO_Pin_6
#define DEBUGGING_USART_TX_GPIO_PORT           GPIOB
#define DEBUGGING_USART_TX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define DEBUGGING_USART_TX_SOURCE              GPIO_PinSource6
#define DEBUGGING_USART_TX_AF                  GPIO_AF_USART1

#define DEBUGGING_USART_RX_PIN                 GPIO_Pin_7
#define DEBUGGING_USART_RX_GPIO_PORT           GPIOB
#define DEBUGGING_USART_RX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define DEBUGGING_USART_RX_SOURCE              GPIO_PinSource7
#define DEBUGGING_USART_RX_AF                  GPIO_AF_USART1

#define DEBUGGING_USART_IRQn                   USART1_IRQn
#define DEBUGGING_IRQHANDLER  				   USART1_IRQHandler


/* Debugging LED = PC.13 */
#define DEBUGGING_LED_PIN      		GPIO_Pin_13
#define DEBUGGING_LED_GPIO_PORT     GPIOC
#define DEBUGGING_LED_GPIO_CLK      RCC_AHB1Periph_GPIOC
#define DEBUGGING_LED_ON			GPIO_SetBits(DEBUGGING_LED_GPIO_PORT, DEBUGGING_LED_PIN);
#define DEBUGGING_LED_OFF			GPIO_ResetBits(DEBUGGING_LED_GPIO_PORT, DEBUGGING_LED_PIN);
#define DEBUGGING_LED_TOGGLE		GPIO_ToggleBits(DEBUGGING_LED_GPIO_PORT, DEBUGGING_LED_PIN);


void Debugging_Init(uint32_t baud);
void Debugging_USART_Config(uint32_t baud);
void Debugging_USART_NVIC_Config(void);
void Debugging_Task(void * pvParameters);

void Debugging_LED_Config(void);

void DEBUGGING_IRQHANDLER(void);
void PrintChar(char c);
int fputc(int ch, FILE *pStream);



#endif /* __DEBUGGING_H */
