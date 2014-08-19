#include "Debugging.h"

#include <stdio.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


void Debugging_Init(uint32_t baud)
{
	Debugging_USART_Config(baud);
	Debugging_USART_NVIC_Config();
	Debugging_LED_Config();
	xTaskCreate( Debugging_Task, "Debugging", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
}

void Debugging_USART_Config(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd(DEBUGGING_USART_TX_GPIO_CLK,ENABLE);
  RCC_AHB1PeriphClockCmd(DEBUGGING_USART_RX_GPIO_CLK,ENABLE);

  RCC_APB2PeriphClockCmd(DEBUGGING_USART_CLK,ENABLE);


  GPIO_PinAFConfig(DEBUGGING_USART_TX_GPIO_PORT, DEBUGGING_USART_TX_SOURCE, DEBUGGING_USART_TX_AF);
  GPIO_PinAFConfig(DEBUGGING_USART_RX_GPIO_PORT, DEBUGGING_USART_RX_SOURCE, DEBUGGING_USART_RX_AF);

  /*
  *  DEBUGGING_USART_TX -> PB10 , DEBUGGING_USART_RX -> PB11
  */
  GPIO_InitStructure.GPIO_Pin = DEBUGGING_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(DEBUGGING_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = DEBUGGING_USART_RX_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(DEBUGGING_USART_RX_GPIO_PORT, &GPIO_InitStructure);

/*
   		 USARTx configured as follow:
         - BaudRate = 115200 baud
		 - Word Length = 8 Bits
         - One Stop Bit
         - No parity
         - Hardware flow control disabled (RTS and CTS signals)
         - Receive and transmit
 */

  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUGGING_USART, &USART_InitStructure);
  /* Enable the DEBUGGING_USART Transmit interrupt: this interrupt is generated when the
     DEBUGGING_USART transmit data register is empty */
  USART_ITConfig(DEBUGGING_USART,USART_IT_RXNE,ENABLE);

  USART_Cmd(DEBUGGING_USART, ENABLE);
}

void Debugging_USART_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUGGING_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Debugging_LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(DEBUGGING_LED_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DEBUGGING_LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(DEBUGGING_LED_GPIO_PORT, &GPIO_InitStructure);

	DEBUGGING_LED_OFF;
}

uint8_t receiveBuffer[100];
uint8_t bufferWritePos = 0;
uint8_t bufferReadPos = 0;
void Debugging_Task(void * pvParameters)
{
	uint8_t * DebuggingBuffer = malloc(200);
	uint8_t ch;

	while (1) {
		if (bufferReadPos != bufferWritePos) {
			bufferReadPos++;
			ch = receiveBuffer[bufferReadPos];

		    if (ch == 'x') {
		    	  PrintChar('\n');
		    	  PrintChar('\r');
		    	  vTaskList((signed char *)(DebuggingBuffer));
		    	  ch = 0;
		    	  while (DebuggingBuffer[ch] != 0) {
		    		  PrintChar(DebuggingBuffer[ch++]);
		    	  }
		    	  printf("\n\rFree Heap: %d\n\r", xPortGetFreeHeapSize());
		    }
		}
		vTaskDelay(50/portTICK_RATE_MS);
	}
}

void DEBUGGING_IRQHANDLER(void)
{
  char ch;
  if(USART_GetITStatus(DEBUGGING_USART, USART_IT_RXNE) != RESET)
  {
    ch = USART_ReceiveData(DEBUGGING_USART);       // get received character
    if (ch=='\r') ch='\n';
    if ((bufferWritePos - bufferReadPos) < sizeof(receiveBuffer)) {
    	bufferWritePos++;
    	receiveBuffer[bufferWritePos] = ch;
    }
    PrintChar(ch);
  }
}


/**
 * @brief  Transmit a char, if you want to use printf(),
 *         you need implement this function
 *
 * @param  pStr	Storage string.
 * @param  c    Character to write.
 */
void PrintChar(char c)
{
	  if (c == 0x0A) {
		  USART_SendData(DEBUGGING_USART, 0x0D);
		  while (USART_GetFlagStatus(DEBUGGING_USART, USART_FLAG_TXE) == RESET);
	  }
	  USART_SendData(DEBUGGING_USART, (uint8_t) c);
	  while (USART_GetFlagStatus(DEBUGGING_USART, USART_FLAG_TXE) == RESET);
}
