/**
  ******************************************************************************
  * @file    STM32F4-Discovery FreeRTOS demo\main.c
  * @author  T.O.M.A.S. Team
  * @version V1.1.0
  * @date    14-October-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "main.h"
#include <stdio.h>

#include "netconf.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"

#include "http_client.h"
#include "Debugging.h"
#include "RGB_Strip.h"
#include "Accelerometer.h"
#include "SSD1963.h"
#include "BarcodeScanner.h"
#include "BallControl.h"
#include "UserGUI.h"

#include "sdio_sd.h"
#include "ff.h"


/** @addtogroup STM32F4-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAIN_TASK_PRIO   ( tskIDLE_PRIORITY + 1 )
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 4 )

/* Private macro -------------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Task functions declarations */
void API_Request_Task(void * pvParameters);
void LED_Task( void *pvParameters );
void SystemInit_Task(void * pvParameters);
void Watchdog_Task(void * pvParameters);

void Debugging_Test_Task(void *pvParameters);
void LCD_Task(void *pvParameters);

static FATFS filesystem;
static FIL file_SD, file_CR;
static DIR dir_1, dir_2;


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
uint32_t time, time2, timedif;
int main(void)
{
	  	/* Check if the system has resumed from WWDG reset */
	  	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	  	{
		  /* WWDGRST flag set */
		  /* Clear reset flags */
		  RCC_ClearFlag();
	  	}

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

        Debugging_Init(115200);
        printf("Booting...\n\r");
        xTaskCreate(LED_Task, ( signed portCHAR * ) "LED", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);

        LCD_Init();
        LCD_Clear(BLACK);

    	GUI_SetDisplay(LCD2);
        GUI_InitializeConsole();
        LCD_FadeBacklight(FADE_UP);
        GUI_ConsolePrint("Booting...");

        RGB_Strip_Init(); // Avoid flickering

        GUI_ConsolePrint("  ");
        GUI_ConsolePrint("Initializing Ethernet...");

        // initialize hardware...
        // configure ethernet (GPIOs, clocks, MAC, DMA)
        ETH_BSP_Config();

        // Initilaize the LwIP stack
        LwIP_Init();

        // Initialize webserver demo
        //http_server_socket_init();

		#ifdef USE_DHCP
        	// Start DHCPClient
        	xTaskCreate(LwIP_DHCP_task, (int8_t *) "DHCP", configMINIMAL_STACK_SIZE * 2, NULL, DHCP_TASK_PRIO, NULL);
		#endif

        xTaskCreate(SystemInit_Task, ( signed portCHAR * ) "SystemInit", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);

        //WATCHDOG VIRKER IKKKE???  xTaskCreate(Watchdog_Task, ( signed portCHAR * ) "Watchdog", configMINIMAL_STACK_SIZE, NULL, WATCHDOG_TASK_PRIO, NULL);

        /* Start the scheduler. */
        vTaskStartScheduler();

        /* Will only get here if there was not enough heap space to create the idle task. */
        return 0;
}

/*-----------------------------------------------------------*/

void SystemInit_Task(void * pvParameters) {
	  uint8_t TextBuffer[50];

      LCD_Touch_Config();

      xTaskCreate( RGB_Strip_Task, ( signed portCHAR * ) "RGBStrip", configMINIMAL_STACK_SIZE*2, NULL, RGB_STRIP_TASK_PRIO, NULL );
      GUI_ConsolePrint("RGB Strip initialized");

      Accelerometer_Init();
      xTaskCreate( Accelerometer_Task, ( signed portCHAR * ) "Accelerometer", configMINIMAL_STACK_SIZE, NULL, ACCELEROMETER_TASK_PRIO, NULL);
      GUI_ConsolePrint("Accelerometer initialized");

	  BarcodeScanner_Init(9600);
	  Buzzer_Init(1000);
	  GUI_ConsolePrint("Barcode Scanner initialized");

	  IRDetectors_Init();
	  GUI_ConsolePrint("IR Detectors initialized");

      BallControl_Init();
      xTaskCreate(BallControl_Task, ( signed char * )"BallControl", 256, NULL, BALLCONTROL_PRIO, NULL);
      GUI_ConsolePrint("Ball Controller initialized");

      /* Prepare touch */
      Touch_ReadAvgX(10, 1);
      Touch_ReadAvgX(10, 2);
      Touch_ReadAvgY(10, 1);
      Touch_ReadAvgY(10, 2);
      xTaskCreate(UserGUI_TouchTask, ( signed char * )"Touch", 256, NULL, TOUCHTASK_PRIO, NULL);
      GUI_ConsolePrint("Touch Controller initialized");

      xTaskCreate( API_Ping_Task, ( signed portCHAR * ) "API_Ping", configMINIMAL_STACK_SIZE*2, NULL, HTTP_API_TASK_PRIO, NULL );

	  GUI_ConsolePrint("Waiting for DHCP...");
	  while (!DHCP_Finished) vTaskDelay(100);
	  if (DHCP_iptab[0] == 0 && DHCP_iptab[1] == 0 && DHCP_iptab[2] == 0 && DHCP_iptab[2] == 0) {
		  GUI_ConsolePrint("ERROR - No internet!");
		  GUI_ConsolePrint(" ");
		  vTaskDelay(500);
		  GUI_ConsolePrint("Booting standard mode...");
		  vTaskDelay(1000);
		  xTaskCreate( UserGUI_NonBlast_Task, ( signed portCHAR * ) "UserGUI", configMINIMAL_STACK_SIZE * 3, NULL, GUI_TASK_PRIO, NULL );
	  } else {
		  sprintf(TextBuffer, "DHCP Finished - IP: %d.%d.%d.%d", DHCP_iptab[3], DHCP_iptab[2], DHCP_iptab[1], DHCP_iptab[0]);
		  GUI_ConsolePrint(TextBuffer);
		  GUI_ConsolePrint("Checking API connection");

		  /*GUI_ConsolePrint("  ");
		  GUI_ConsolePrint("Initializing filesystem...");
		  // could not open filesystem
		  if (f_mount(0, &filesystem) != FR_OK)
		  {
			  printf("Filesystem could not be initialized!\r\n");
			  GUI_ConsolePrint("Filesystem could not be initialized!");
			  TheBlast_SMS(ADMIN_USER_ID, "FOOTBALL: Filesystem could not be initialized!");
			  while (1) vTaskDelay(100);
		  }
		  // could not open the selected directory
		  if (f_opendir(&dir_1, "/") != FR_OK)
		  {
			  printf("Root folder of SD card was not found!\r\n");
			  GUI_ConsolePrint("Root folder of SD card was not found!");
			  TheBlast_SMS(ADMIN_USER_ID, "FOOTBALL: Root folder of SD card was not found!");
			  while (1) vTaskDelay(100);
		  }

		  printf("SD card initialized\r\n");
		  GUI_ConsolePrint("Filesystem initialized");*/

		  while (waitForAPIrequest == 1) vTaskDelay(100);
		  vTaskDelay(1000);

		  //xTaskCreate( Debugging_Test_Task, ( signed portCHAR * ) "DEBUG", configMINIMAL_STACK_SIZE * 3, NULL, (tskIDLE_PRIORITY + 2), NULL );

		  xTaskCreate( UserGUI_Task, ( signed portCHAR * ) "UserGUI", configMINIMAL_STACK_SIZE * 3, NULL, GUI_TASK_PRIO, NULL );
      }
	  vTaskDelete(NULL); // End this task!
	  while(1);
}

void Watchdog_Task(void * pvParameters) {
	  /* WWDG configuration */
	  /* Enable WWDG clock */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	  /* WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)  */
	  WWDG_SetPrescaler(WWDG_Prescaler_8);

	  /* Set Window value to 80; WWDG counter should be refreshed only when the counter
	    is below 80 (and greater than 64) otherwise a reset will be generated */
	  WWDG_SetWindowValue(255); // 80

	  /* Enable WWDG and set counter value to 127, WWDG timeout = ~780 us * 64 = 49.92 ms
	     In this case the refresh window is:
	           ~780 * (127-80) = 36.6ms < refresh window < ~780 * 64 = 49.9ms
	  */
	  WWDG_Enable(255);  // 127

	  while (1)
	  {
	    /* Insert 40 ms delay */
	    vTaskDelay(10);

	    /* Update WWDG counter */
	    WWDG_SetCounter(255);
	  }
}

void API_Ping_Task(void * pvParameters) {
	uint8_t returnBuffer[25];
	uint8_t success;
	uint8_t i;
	uint32_t nextPingTime = 0;
	uint8_t failedRequestCount = 0;

	API_Network_Error = 0;
	waitForAPIrequest = 1;
	while (!DHCP_Finished) vTaskDelay(100); // wait for DHCP if not already finished!

	if (DHCP_iptab[0] != 0 && DHCP_iptab[1] != 0 && DHCP_iptab[2] != 0 && DHCP_iptab[2] != 0) {
		while (waitForAPIrequest == 1) {
			/* Do initial boot check - any ongoing matches? */
			for (i = 0; i < 25; i++) returnBuffer[i] = 0;
			success = TheBlast_Get_Synchronous("theblast.php?get=reboot", 25, returnBuffer);

			if (success) {
				if (strstr(returnBuffer, "ok") != 0) { // everything OK
					printf("Return from HTTP: %s\n", returnBuffer);
					waitForAPIrequest = 0;
					failedRequestCount = 0;
					nextPingTime = millis() + API_PING_TIMEOUT;
				} else {
					printf("Return from HTTP: %s\n", returnBuffer);
					// Ongoing match
					UserGUI_RebootMatch_Init(returnBuffer);
					failedRequestCount = 0;
					nextPingTime = millis() + API_PING_TIMEOUT;
				}
			} else {
				printf("Return from HTTP: %s\n", returnBuffer);
				waitForAPIrequest = 1; // stay in API request waiting loop!
				failedRequestCount++;
				nextPingTime = millis() + API_PING_TIMEOUT_RETRY;
			}
		}

		while (1) {
			if (millis() > nextPingTime) {
				for (i = 0; i < 25; i++) returnBuffer[i] = 0;
				success = TheBlast_Get_Synchronous("theblast.php?get=check", 25, returnBuffer);

				if (success) {
					if (strstr(returnBuffer, "ok") != 0) {
						printf("Return from HTTP: %s\n", returnBuffer);
						waitForAPIrequest = 0;
						failedRequestCount = 0;
						nextPingTime = millis() + API_PING_TIMEOUT;
					} else if (strstr(returnBuffer, "reboot") != 0) {
						NVIC_SystemReset();
					} else {
						printf("Return from HTTP: %s\n", returnBuffer);
						waitForAPIrequest = 1; // stay in API request waiting loop!
						failedRequestCount++;
						nextPingTime = millis() + API_PING_TIMEOUT_RETRY;
					}
				} else {
					printf("Return from HTTP: %s\n", returnBuffer);
					waitForAPIrequest = 1; // stay in API request waiting loop!
					failedRequestCount++;
					nextPingTime = millis() + API_PING_TIMEOUT_RETRY;
				}

				/*if (failedRequestCount >= (2*API_PING_ERROR_LEVEL)) { // If there has been too many errors, restart table
					NVIC_SystemReset();
				} else if (failedRequestCount >= API_PING_ERROR_LEVEL) {
					API_Network_Error = 1;
				} else {
					API_Network_Error = 0;
				}*/
				// When API error occours x-number of times, just restart!
				if (failedRequestCount >= API_PING_ERROR_LEVEL) {
					NVIC_SystemReset();
				}
			}
			vTaskDelay(1000);
		}
	}

	vTaskDelete(NULL); // End this task!
	while(1);
}


void LED_Task(void *pvParameters)
{
	for( ;; )
    {
		DEBUGGING_LED_TOGGLE;
		vTaskDelay(500);
    }

}


void Debugging_Test_Task(void *pvParameters)
{
	while (1)
	{
		if (IRDetectors_GetFlag(LEFT_GOAL)) {
			Buzzer_Set(1);
			vTaskDelay(50); // Delay for the buzzer to sound
			Buzzer_Set(0);
			vTaskDelay(1000);
			IRDetectors_ClearFlag(LEFT_GOAL);
		}

		if (IRDetectors_GetFlag(RIGHT_GOAL)) {
			Buzzer_Set(1);
			vTaskDelay(50); // Delay for the buzzer to sound
			Buzzer_Set(0);
			vTaskDelay(50); // Delay for the buzzer to sound
			Buzzer_Set(1);
			vTaskDelay(50); // Delay for the buzzer to sound
			Buzzer_Set(0);

			vTaskDelay(1000);
			IRDetectors_ClearFlag(RIGHT_GOAL);
		}
		vTaskDelay(100);
	}

}


void LCD_Task(void *pvParameters)
{
	uint32_t color;
	uint16_t x, y;
	GUI_SetDisplay(LCD1);
	GUI_DisplayBMP_FAT(10, 10, "logo.bmp");

	//vTaskDelay(500);
	GUI_SetDisplay(LCD2);
	GUI_DisplayBMP_FAT(50, 300, "test.bmp");

	GUI_SetDisplay(LCD_BOTH);

	while (1)
	{
		vTaskDelay(10);
		if (LCD2_TOUCH_IRQ) {
			x = (Touch_ReadAvgY(10, LCD2) * GetMaxX()) / 4096;
			y = (Touch_ReadAvgX(10, LCD2) * GetMaxY()) / 4096;
			y = GetMaxY() - y; // Invert y
			GUI_PutPixel(x, y, BRIGHTCYAN);
		}

		if (IRDetectors_GetFlag(IR1)) {
			Buzzer_Set(1);
			vTaskDelay(50); // Delay for the buzzer to sound
			Buzzer_Set(0);
			IRDetectors_ClearFlag(IR1);
		}
	}
}
