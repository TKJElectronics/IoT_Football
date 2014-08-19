#ifndef __BALLCONTROL_H
#define __BALLCONTROL_H

#include "stm32f4xx.h"

#include "BarcodeScanner.h" // For Buzzer control only!
#include "http_client.h" // SMS message
#include "UserGUI.h" // for CurrentPage checking for ball/detector control

/* RTOS includes. */
#include "Os_Wrap.h"

#define IRDetectors_Read(detector)		GPIO_ReadInputDataBit(GPIOG, detector)

typedef enum {
	IR_NONE = 0,
	IR2 = 1,
	IR3 = 2,
	IR1 = 4,
	IR4 = 8,
	IR5 = 16,
	IR6 = 32
} IR_Detectors;


#define LEFT_GOAL	IR1
#define RIGHT_DROP	IR2
#define LEFT_DROP	IR3
#define RIGHT_GOAL	IR4
#define BALL1		IR5
#define BALL2		IR6


#define	BallTimeOut	5000 // 5 seconds

extern uint8_t WaitForLeftBall;
extern uint32_t LeftBallTimeOut;
extern uint8_t WaitForRightBall;
extern uint32_t RightBallTimeOut;

extern uint8_t BallControl_ScoreLeft;
extern uint8_t BallControl_ScoreRight;

extern uint32_t BallControl_Cooldown;


void BallControl_Init(void);
void BallControl_Task(void * pvParameters);
void BallControl_ServeBall(void);

//void BallControl_PWM_Config(uint16_t OutputClock, uint16_t PWM_Resolution, uint16_t MinPosUs, uint16_t MaxPosUs);
void BallControl_PWM_Config(uint16_t OutputClock, uint16_t PWM_Resolution);
void BallControl_PWM_Set(uint8_t value);

void IRDetectors_Init(void);
void IRDetectors_SetFlag(IR_Detectors flag);
void IRDetectors_ClearFlag(IR_Detectors flag);
void IRDetectors_ClearAllFlags(void);
uint8_t IRDetectors_GetFlag(IR_Detectors flag);

#endif /* __BALLCONTROL_H */
