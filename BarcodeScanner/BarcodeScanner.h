#ifndef __BARCODESCANNER_H
#define __BARCODESCANNER_H

#include "stm32f4xx.h"

/* RTOS includes. */
#include "Os_Wrap.h"

/* USART Config parameters */
#define BARCODESCANNER_USART                        USART6
#define BARCODESCANNER_USART_CLK                    RCC_APB2Periph_USART6

#define BARCODESCANNER_USART_TX_PIN                 GPIO_Pin_6
#define BARCODESCANNER_USART_TX_GPIO_PORT           GPIOC
#define BARCODESCANNER_USART_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define BARCODESCANNER_USART_TX_SOURCE              GPIO_PinSource6
#define BARCODESCANNER_USART_TX_AF                  GPIO_AF_USART6

#define BARCODESCANNER_USART_RX_PIN                 GPIO_Pin_7
#define BARCODESCANNER_USART_RX_GPIO_PORT           GPIOC
#define BARCODESCANNER_USART_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define BARCODESCANNER_USART_RX_SOURCE              GPIO_PinSource7
#define BARCODESCANNER_USART_RX_AF                  GPIO_AF_USART6

#define BARCODESCANNER_USART_IRQn                   USART6_IRQn
#define BARCODESCANNER_IRQHANDLER  				    USART6_IRQHandler


/* BARCODESCANNER Control pin */
#define BARCODESCANNER_CONTROL_PIN              GPIO_Pin_3
#define BARCODESCANNER_CONTROL_GPIO_PORT        GPIOD
#define BARCODESCANNER_CONTROL_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define BARCODESCANNER_CONTROL_ON				GPIO_ResetBits(BARCODESCANNER_CONTROL_GPIO_PORT, BARCODESCANNER_CONTROL_PIN);
#define BARCODESCANNER_CONTROL_OFF				GPIO_SetBits(BARCODESCANNER_CONTROL_GPIO_PORT, BARCODESCANNER_CONTROL_PIN);


#define BARCODE_TIMEOUT		10000 		// 10 seconds

typedef enum {
	IDLE,
	WAITING_FOR_SCAN,
	SCAN_CONFIRM,
	SCAN_OK,
	SCAN_ERROR
} BS_Status;
extern BS_Status BarcodeScanner_Status;

extern uint8_t BarcodeScanner_Buffer[100];

void BarcodeScanner_Init(uint32_t baud);
void BarcodeScanner_USART_Config(uint32_t baud);
void BarcodeScanner_USART_NVIC_Config(void);
void BarcodeScanner_Control_Config(void);

void BARCODESCANNER_IRQHANDLER(void);

void BarcodeScanner_InitiateScan(void);
void BarcodeScanner_StopScan(void);
int8_t BarcodeScanner_GetScan(void);
void BarcodeScanner_ScanTask(void * pvParameters);

void Buzzer_Init(uint16_t OutputClock);
void Buzzer_Config(void);
void Buzzer_TIM11_Init(uint16_t OutputClock);
void Buzzer_Set(uint8_t state);
void Buzzer_Period(uint8_t period);

#endif /* __BARCODESCANNER_H */
