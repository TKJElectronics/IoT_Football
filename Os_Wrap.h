#ifndef OS_WRAP_H
#define OS_WRAP_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define millis()	(xTaskGetTickCount() / portTICK_RATE_MS)


/*--------------- Tasks Priority -------------*/
/*
#define BALLCONTROL_TASK_PRIO    ( tskIDLE_PRIORITY + 7 )
#define CLOCK_TASK_PRIO    ( tskIDLE_PRIORITY + 9 )
#define TCPIP_THREAD_PRIO   ( tskIDLE_PRIORITY + 10 )
*/

#define LED_TASK_PRIO    ( tskIDLE_PRIORITY )
#define ACCELEROMETER_TASK_PRIO    ( tskIDLE_PRIORITY + 1 )
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 3 )
#define BALLCONTROL_PRIO    ( tskIDLE_PRIORITY + 4 )
#define GUI_TASK_PRIO    ( tskIDLE_PRIORITY + 5 )
#define RGB_STRIP_TASK_PRIO    ( tskIDLE_PRIORITY + 6 )
#define HTTP_API_TASK_PRIO    ( tskIDLE_PRIORITY + 7 )
#define HTTP_CLIENT_TASK_PRIO    ( tskIDLE_PRIORITY + 8 )
#define TOUCHTASK_PRIO    ( tskIDLE_PRIORITY + 9 )
#define BARCODE_TASK_PRIO    ( tskIDLE_PRIORITY + 10 )
#define WATCHDOG_TASK_PRIO    ( tskIDLE_PRIORITY + 11 )


//os api wrap
#define OS_TaskCreate	xTaskCreate
#define OS_TaskDelete	vTaskDelete
#define OS_TaskDelay		vTaskDelay
#define OS_TaskDelayUntil		vTaskDelayUntil
#define OS_TaskPriorityGet	uxTaskPriorityGet
#define OS_TaskPrioritySet	vTaskPrioritySet
#define OS_TaskSuspend		vTaskSuspend
#define OS_TaskResume		vTaskResume
#define OS_TaskResumeFromISR		xTaskResumeFromISR

#define OS_StartScheduler		vTaskStartScheduler
#define OS_TaskEndScheduler	vTaskEndScheduler
#define OS_TaskSuspendAll	vTaskSuspendAll
#define OS_TaskResumeAll	xTaskResumeAll

#if 0 //不常用到，可以省掉
#define OS_TaskGetCurrentTaskHandle	xTaskGetCurrentTaskHandle
#define OS_TaskGetTickCount	 xTaskGetTickCount
#define OS_TaskGetSchedulerState 	xTaskGetSchedulerState
#define OS_TaskGetNumberOfTasks 	uxTaskGetNumberOfTasks
#define OS_TaskList 	vTaskList
#define OS_TaskStartTrace 	vTaskStartTrace
#define OS_TaskEndTrace 	ulTaskEndTrace
#endif

#define OS_QueueCreate 		xQueueCreate
#define OS_QueueSend			xQueueSend
#define OS_QueueSendToBack 		xQueueSendToBack
#define OS_QueueSendToFront		xQueueSendToFront
#define OS_QueueReceive 		xQueueReceive
#define OS_QueuePeek		xQueuePeek
#define OS_QueueSendFromISR		xQueueSendFromISR
#define OS_QueueSendToBackFromISR		xQueueSendToBackFromISR
#define OS_QueueSendToFrontFromISR		xQueueSendToFrontFromISR
#define OS_QueueReceiveFromISR 		xQueueReceiveFromISR
#define OS_QueueAddToRegistry 			vQueueAddToRegistry
#define OS_QueueUnregisterQueue 		vQueueUnregisterQueue

#define OS_SemaphoreCreateBinary			vSemaphoreCreateBinary
#define OS_SemaphoreCreateCounting 	xSemaphoreCreateCounting
#define OS_SemaphoreCreateMutex 		xSemaphoreCreateMutex
#define OS_SemaphoreCreateRecursiveMutex 	xSemaphoreCreateRecursiveMutex
#define OS_SemaphoreTake 			xSemaphoreTake
#define OS_SemaphoreTakeRecursive 		xSemaphoreTakeRecursive
#define OS_SemaphoreGive 			xSemaphoreGive
#define OS_SemaphoreGiveRecursive 		xSemaphoreGiveRecursive
#define OS_SemaphoreGiveFromISR		xSemaphoreGiveFromISR

#define OS_Mallco pvPortMalloc
#define OS_Free vPortFree
//os marco
#define OS_YIELD			taskYIELD
#define OS_YIELD_FROM_ISR			vPortYieldFromISR
#define OS_ENTER_CRITICAL		taskENTER_CRITICAL
#define OS_EXIT_CRITICAL			taskEXIT_CRITICAL
#define OS_ENABLE_INTERRUPTS 	taskENABLE_INTERRUPTS
#define OS_DISABLE_INTERRUPTS	taskDISABLE_INTERRUPTS
#define OS_ENABLE_INTERRUPTS_FROM_ISR 	vPortClearInterruptMask
#define OS_DISABLE_INTERRUPTS_FROM_ISR	vPortSetInterruptMask

//os config
#define OS_MINI_STACK_SIZE		configMINIMAL_STACK_SIZE
#define OS_MAX_PRIORITIES		configMAX_PRIORITIES
#define OS_MIN_PRIORITIES		tskIDLE_PRIORITY
#define OS_TICK_RATE_HZ			configTICK_RATE_HZ
#define OS_TICK_RATE_MS			portTICK_RATE_MS
#define OS_MAX_DELAY					portMAX_DELAY

//os variable type
#define OS_CHAR	portCHAR
#define OS_FLOAT	portFLOAT
#define OS_DOUBLE	portDOUBLE
#define OS_LONG	portLONG
#define OS_SHORT	portSHORT
#define OS_STACK_TYPE	portSTACK_TYPE
#define OS_BASE_TYPE	portBASE_TYPE
#define OS_PASS	pdPASS
#define OS_FAIL		pdFAIL
#define OS_QUEUE_EMPTY	errQUEUE_EMPTY
#define OS_QUEUE_FULL 		errQUEUE_FULL




#define OS_QueueHandle	xQueueHandle
#define OS_SemaphoreHandle	xSemaphoreHandle



#endif				 

