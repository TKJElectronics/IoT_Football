#include "BarcodeScanner.h"

BS_Status BarcodeScanner_Status = IDLE;
uint8_t BarcodeScanner_Buffer[100];
uint8_t BarcodeBufferPos = 0;
xTaskHandle BarcodeTask = NULL;

void BarcodeScanner_Init(uint32_t baud)
{
	BarcodeScanner_Control_Config();
	BarcodeScanner_USART_Config(baud);
	BarcodeScanner_USART_NVIC_Config();
}

void BarcodeScanner_USART_Config(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd(BARCODESCANNER_USART_TX_GPIO_CLK,ENABLE);
  RCC_AHB1PeriphClockCmd(BARCODESCANNER_USART_RX_GPIO_CLK,ENABLE);

  RCC_APB2PeriphClockCmd(BARCODESCANNER_USART_CLK,ENABLE);


  GPIO_PinAFConfig(BARCODESCANNER_USART_TX_GPIO_PORT, BARCODESCANNER_USART_TX_SOURCE, BARCODESCANNER_USART_TX_AF);
  GPIO_PinAFConfig(BARCODESCANNER_USART_RX_GPIO_PORT, BARCODESCANNER_USART_RX_SOURCE, BARCODESCANNER_USART_RX_AF);

  /*
  *  BARCODESCANNER_USART_TX -> PB10 , BARCODESCANNER_USART_RX -> PB11
  */
  GPIO_InitStructure.GPIO_Pin = BARCODESCANNER_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BARCODESCANNER_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = BARCODESCANNER_USART_RX_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BARCODESCANNER_USART_RX_GPIO_PORT, &GPIO_InitStructure);

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
  USART_Init(BARCODESCANNER_USART, &USART_InitStructure);
  /* Enable the BARCODESCANNER_USART Transmit interrupt: this interrupt is generated when the
     BARCODESCANNER_USART transmit data register is empty */
  USART_ITConfig(BARCODESCANNER_USART,USART_IT_RXNE,ENABLE);

  USART_Cmd(BARCODESCANNER_USART, ENABLE);
}

void BarcodeScanner_USART_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = BARCODESCANNER_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void BarcodeScanner_Control_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(BARCODESCANNER_CONTROL_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BARCODESCANNER_CONTROL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(BARCODESCANNER_CONTROL_GPIO_PORT, &GPIO_InitStructure);

	BARCODESCANNER_CONTROL_OFF;
}

void BARCODESCANNER_IRQHANDLER(void)
{
  char ch;
  if(USART_GetITStatus(BARCODESCANNER_USART, USART_IT_RXNE) != RESET)
  {
    ch = USART_ReceiveData(BARCODESCANNER_USART);       // get received character
    if (BarcodeScanner_Status == WAITING_FOR_SCAN && BarcodeBufferPos < 100) {
    	if (ch == '\r') { // New line
    		BarcodeScanner_Status = SCAN_CONFIRM;
    	} else {
    		BarcodeScanner_Buffer[BarcodeBufferPos++] = ch;
    	}
    }
  }
}

void BarcodeScanner_InitiateScan(void)
{
	uint8_t i;
	if (BarcodeTask != NULL) return; // Barcode scanner task is already running

	for (i = 0; i < 100; i++) BarcodeScanner_Buffer[i] = 0;
	BarcodeBufferPos = 0;

	BARCODESCANNER_CONTROL_ON;
	BarcodeScanner_Status = WAITING_FOR_SCAN;
	xTaskCreate(BarcodeScanner_ScanTask, ( signed char * )"BarcodeTask", 256, NULL, BARCODE_TASK_PRIO, &BarcodeTask);
}

void BarcodeScanner_StopScan(void)
{
	if (BarcodeTask == NULL) return; // Barcode scanner task not running

	BARCODESCANNER_CONTROL_OFF;
	Buzzer_Set(0);
	BarcodeScanner_Status = IDLE;

	vTaskDelete(BarcodeTask); // End the task!
	BarcodeTask = NULL;
}

int8_t BarcodeScanner_GetScan(void) {
	if (BarcodeScanner_Status == WAITING_FOR_SCAN || BarcodeScanner_Status == SCAN_CONFIRM || BarcodeScanner_Status == IDLE) {
		return 0;
	} else if (BarcodeScanner_Status == SCAN_OK) {
		BarcodeScanner_Status = IDLE;
		return (BarcodeBufferPos - 1);
	} else {
		return -1; // Error
	}

}


void BarcodeScanner_ScanTask(void * pvParameters)
{
	while (BarcodeScanner_Status == WAITING_FOR_SCAN) {
		vTaskDelay(100);
	}

	Buzzer_Set(1);
	vTaskDelay(50); // Delay for the buzzer to sound
	BARCODESCANNER_CONTROL_OFF;
	Buzzer_Set(0);

	if (BarcodeScanner_Status == SCAN_CONFIRM) {
		// Confirm the received scan
		if (BarcodeBufferPos < 20) { // Barcodes is less than 20 characters
			BarcodeScanner_Status = SCAN_OK;
		} else {
			BarcodeScanner_Status = SCAN_ERROR;
		}
	}

	BarcodeTask = NULL;
	vTaskDelete(NULL); // End the task!
	while(1);
}



void Buzzer_Init(uint16_t OutputClock)
{
	Buzzer_Config();
	Buzzer_TIM11_Init(OutputClock);

}

void Buzzer_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /* TIM11 clock enable */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);

	  /* GPIOC clock enable */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	  /* GPIOC Configuration: TIM11 CH1 (PF7) */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	  GPIO_Init(GPIOF, &GPIO_InitStructure);

	  /* Connect TIM11 pins to AF3 */
	  GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_TIM11);
}

void Buzzer_TIM11_Init(uint16_t OutputClock)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t PrescalerValue = 0;

	  /* Time base configuration */
	  TIM_TimeBaseStructure.TIM_Period = 255;

	  /* Compute the prescaler value */
	  PrescalerValue = (uint16_t) (((SystemCoreClock/2) / (OutputClock*(TIM_TimeBaseStructure.TIM_Period+1))) - 1);

	  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	  TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

	  /* PWM1 Mode configuration: Channel1 */
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = 0;
	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  TIM_OC1Init(TIM11, &TIM_OCInitStructure);
	  TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);

	  TIM_ARRPreloadConfig(TIM11, ENABLE);

	  /* TIM9 enable counter */
	  TIM_Cmd(TIM11, ENABLE);
}

void Buzzer_Set(uint8_t state)
{
	TIM_SetCompare1(TIM11, (state*128));
}

void Buzzer_Period(uint8_t period)
{
	TIM_SetAutoreload(TIM11, (255-period));
}
