#ifndef __HTTP_CLIENT_H
#define __HTTP_CLIENT_H

#include "stm32f4xx.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/tcpip.h"
#include "stm32f4x7_eth.h"
#include "netconf.h"

#include "string.h"

/* RTOS includes. */
#include "Os_Wrap.h"

/*#include "sdio_sd.h"
#include "ff.h"
#include "diskio.h"*/

extern uint8_t UserGUI_Hours, UserGUI_Minutes, UserGUI_Seconds;
extern char DHCP_Finished;

extern uint8_t HTTP_Asynchronous_Feedback[100];
#define HTTP_SLOT_AVAILABLE 0
#define HTTP_SLOT_PENDING 1
#define HTTP_SLOT_FINISH_OK 2
#define HTTP_SLOT_FINISH_ERR 3

void HTTP_Client_Test(uint16_t x, uint16_t y, uint8_t * fileName, uint8_t taskID, uint32_t * CallbackVariablePointer);
void GetTime(void * pvParameters);
void DisplayOnlineBMP(void * pvParameters);

void TheBlast_SMS(char * receiver, char * message);
void TheBlast_SMS_BackgroundTask(void * pvParameters);
void TheBlast_HTTP_Get(void * pvParameters);
uint8_t DisplayUserImage(uint8_t alignment, uint16_t y, uint8_t * fileName, uint8_t taskID);

#define GET_TRY_TIMES	3
uint8_t TheBlast_Get_Synchronous(char * getRequest, uint16_t charactersToReturn, uint8_t * returnBuffer);

int8_t TheBlast_Get_Asynchronous(char * getRequest, uint16_t charactersToReturn, uint8_t * returnBuffer);
void TheBlast_Get_Asynchronous_Task(uint8_t * pvParameters);


#endif /* __HTTP_CLIENT_H */
