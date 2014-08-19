#include "BallControl.h"

uint16_t PWM_MINPOS_ADD;

uint8_t IRDetectors_Flags = IR_NONE;

uint8_t BallControl_ScoreLeft = 0;
uint8_t BallControl_ScoreRight = 0;

uint8_t BallsReleased = 0;
uint32_t BallControl_Cooldown;

void BallControl_Init(void)
{
	IRDetectors_Init();
	//BallControl_PWM_Config(50, 100, 500, 2450);
	BallControl_PWM_Config(4000, 100);
	BallControl_PWM_Set(0);
}

/* Take care of Ball-faults and such */
void BallControl_Task(void * pvParameters)
{
	while (1) {
		if (CurrentPage == USERGUI_SCORE) {
			if (IRDetectors_GetFlag(LEFT_GOAL) && BallControl_ScoreLeft == 0 && BallControl_Cooldown == 0) { // Do not clear flag, as GOAL-Flags are supposed to be handled in main
				if (BallsReleased > 0) {
					BallsReleased--;
					BallControl_ScoreLeft = 1;
					Buzzer_Set(1);
					vTaskDelay(500);
					Buzzer_Set(0);
				} else {
					IRDetectors_ClearFlag(LEFT_GOAL);
					vTaskDelay(500);
					IRDetectors_ClearFlag(LEFT_GOAL);
				}
			} else if (IRDetectors_GetFlag(RIGHT_GOAL) && BallControl_ScoreRight == 0 && BallControl_Cooldown == 0) {
				if (BallsReleased > 0) {
					BallsReleased--;
					BallControl_ScoreRight = 1;
					Buzzer_Set(1);
					vTaskDelay(500);
					Buzzer_Set(0);
				} else {
					IRDetectors_ClearFlag(RIGHT_GOAL);
					vTaskDelay(500);
					IRDetectors_ClearFlag(RIGHT_GOAL);
				}
			}

			if (BallControl_ScoreLeft == 1 && !IRDetectors_GetFlag(LEFT_GOAL)) {
				BallControl_ScoreLeft = 0;
			} else if (BallControl_ScoreRight == 1 && !IRDetectors_GetFlag(RIGHT_GOAL)) {
				BallControl_ScoreRight = 0;
			}

			if (BallControl_Cooldown > 0) {
				if (millis() > BallControl_Cooldown) {
					BallControl_Cooldown = 0;
				}
			}


			/*if (IRDetectors_GetFlag(LEFT_DROP)) {
				WaitForLeftBall = 0;
				IRDetectors_ClearFlag(LEFT_DROP);
			} else if (IRDetectors_GetFlag(RIGHT_DROP)) {
				WaitForRightBall = 0;
				IRDetectors_ClearFlag(RIGHT_DROP);
			}*/

			/*if (IRDetectors_GetFlag(BALL2)) {
			BallsReleased++;
			IRDetectors_ClearFlag(BALL2);
			}*/

			vTaskDelay(10); // Task delay
		} else {
			IRDetectors_ClearAllFlags();
			vTaskDelay(1000);
		}
	}
}

void BallControl_ServeBall(void)
{
	uint32_t timeout;
	if (BallsReleased == 0) {
		BallControl_PWM_Set(50);
		timeout = millis()+3000; // 100 ms max
		while (!IRDetectors_Read(BALL1) && millis() < timeout) vTaskDelay(1);
		BallControl_PWM_Set(0);
		if (millis() >= timeout) { // timeout reached - send warn sms
			if (selectedGameMode != GAME_NONBLAST) TheBlast_SMS(ADMIN_USER_ID, "FOOTBALL: Ball release failed!");
		} else {
			BallsReleased++; // a new ball has been released to the table!
		}
	}
}

/* Servo left = 256 ---- Servo right = 358 */
void BallControl_PWM_Config2(uint16_t OutputClock, uint16_t PWM_Resolution, uint16_t MinPosUs, uint16_t MaxPosUs)
{
	uint16_t PrescalerValue = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t posDifference = MaxPosUs - MinPosUs;

	uint16_t periodValue = (PWM_Resolution * 20 * 1000) / posDifference;
	PWM_MINPOS_ADD = (MinPosUs * periodValue) / (20 * 1000);

	  /* TIM3 clock enable */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	  /* GPIOC clock enable */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	  /* GPIOC Configuration: TIM13 CH1 (PF8) */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	  GPIO_Init(GPIOF, &GPIO_InitStructure);

	  /* Connect TIM13 pins to AF3 */
	  GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);

	  /* Time base configuration */
	  TIM_TimeBaseStructure.TIM_Period = periodValue;

	  /* Compute the prescaler value */
	  PrescalerValue = (uint16_t) (((SystemCoreClock) / (OutputClock*(TIM_TimeBaseStructure.TIM_Period+1))) - 1);

	  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	  /* PWM1 Mode configuration: Channel1 */
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = PWM_MINPOS_ADD+100; // Set to max position
	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	  TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);

	  TIM_ARRPreloadConfig(TIM14, ENABLE);

	  /* TIM9 enable counter */
	  TIM_Cmd(TIM14, ENABLE);
}

void BallControl_PWM_Config(uint16_t OutputClock, uint16_t PWM_Resolution)
{
	uint16_t PrescalerValue = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	  /* TIM3 clock enable */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	  /* GPIOC clock enable */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	  /* GPIOC Configuration: TIM13 CH1 (PF8) */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	  GPIO_Init(GPIOF, &GPIO_InitStructure);

	  /* Connect TIM13 pins to AF3 */
	  GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);

	  /* Time base configuration */
	  TIM_TimeBaseStructure.TIM_Period = PWM_Resolution-1; // 100 - 1

	  /* Compute the prescaler value */
	  PrescalerValue = (uint16_t) (((SystemCoreClock) / (OutputClock*(TIM_TimeBaseStructure.TIM_Period+1))) - 1);

	  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	  /* PWM1 Mode configuration: Channel1 */
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = PWM_MINPOS_ADD+100; // Set to max position
	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	  TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);

	  TIM_ARRPreloadConfig(TIM14, ENABLE);

	  /* TIM9 enable counter */
	  TIM_Cmd(TIM14, ENABLE);
}

void BallControl_PWM_Set(uint8_t value)
{
	//TIM_SetCompare1(TIM10, value+PWM_MINPOS_ADD);
	TIM_SetCompare1(TIM14, value);
}

void IRDetectors_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  EXTI_InitTypeDef EXTI_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;

	  /* Enable GPIO clocks */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* Connect IR EXTI Line to Button GPIO Pin */
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource0);
	  //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource1);
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource2);
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource3);
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource4);
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource5);
	  /* Configure Button EXTI line */
	  EXTI_InitStructure.EXTI_Line = EXTI_Line0/* | EXTI_Line1*/ | EXTI_Line2 | EXTI_Line3;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line5;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	  /*NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	  NVIC_Init(&NVIC_InitStructure);*/

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	  NVIC_Init(&NVIC_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	  NVIC_Init(&NVIC_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	  NVIC_Init(&NVIC_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	  NVIC_Init(&NVIC_InitStructure);
}

void IRDetectors_SetFlag(IR_Detectors flag)
{
	IRDetectors_Flags |= flag;
}

void IRDetectors_ClearFlag(IR_Detectors flag)
{
	IRDetectors_Flags &= ~flag;
}

uint8_t IRDetectors_GetFlag(IR_Detectors flag)
{
	return (((IRDetectors_Flags & flag) > 0) ? 1 : 0);
}

void IRDetectors_ClearAllFlags(void)
{
	IRDetectors_ClearFlag(IR1);
	IRDetectors_ClearFlag(IR2);
	IRDetectors_ClearFlag(IR3);
	IRDetectors_ClearFlag(IR4);
	IRDetectors_ClearFlag(IR5);
	IRDetectors_ClearFlag(IR6);
}


void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
	/* Clear pending interrupt flag bit */
	EXTI_ClearITPendingBit(EXTI_Line0);
	if (!IRDetectors_Read(IR2))
		IRDetectors_SetFlag(IR2);
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
  }
}
/*void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
	// Clear pending interrupt flag bit
	EXTI_ClearITPendingBit(EXTI_Line1);
	if (!IRDetectors_Read(IR3))
		IRDetectors_SetFlag(IR3);
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
  }
}*/
void EXTI2_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
	/* Clear pending interrupt flag bit */
	EXTI_ClearITPendingBit(EXTI_Line2);
	if (!IRDetectors_Read(IR1))
		IRDetectors_SetFlag(IR1);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
  }
}
void EXTI3_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
	/* Clear pending interrupt flag bit */
	EXTI_ClearITPendingBit(EXTI_Line3);
	if (!IRDetectors_Read(IR4))
		IRDetectors_SetFlag(IR4);
    NVIC_ClearPendingIRQ(EXTI3_IRQn);
  }
}
void EXTI4_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
	/* Clear pending interrupt flag bit */
	EXTI_ClearITPendingBit(EXTI_Line4);
	if (IRDetectors_Read(IR5))
		IRDetectors_SetFlag(IR5);
    NVIC_ClearPendingIRQ(EXTI4_IRQn);
  }
}
void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
	/* Clear pending interrupt flag bit */
	EXTI_ClearITPendingBit(EXTI_Line5);
	if (IRDetectors_Read(IR6))
		IRDetectors_SetFlag(IR6);
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
  }
}


