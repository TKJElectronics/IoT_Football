#include "http_client.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

static const char post_HTTP_header[] = {" HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 1.0.3705)\r\nHost: www.yourserver.com\r\n\r\n"}; // Connection: Keep-Alive\r\n
static const char TheBlast_HTTP_Get_Header[] = {" HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 1.0.3705)\r\nHost: yourserver.com\r\n\r\n"}; // Connection: Keep-Alive\r\n
static char send_buffer_client[500];
static char recv_buffer_client[1460];
//static char fileWriteBuffer[512];
static char timeBuffer[50];

uint8_t HTTP_Asynchronous_Feedback[100];


uint8_t SMS_Receiver_Current[8];
uint8_t SMS_Message_Current[50];
void TheBlast_SMS(char * userID, char * message)
{
	uint16_t charactersToReturn = 0;
	uint8_t returnBuffer[2]; // temp purpose

	strncpy(SMS_Receiver_Current, userID, 8);
	strcpy(SMS_Message_Current, message);

	xTaskCreate(TheBlast_SMS_BackgroundTask, "SMS_Task", 512, NULL, HTTP_API_TASK_PRIO, NULL);
}

void TheBlast_SMS_BackgroundTask(void * pvParameters)
{
	uint8_t tryTimes = GET_TRY_TIMES;
	uint16_t charactersToReturn = 0;
	uint8_t returnBuffer[2]; // temp purpose
	uint8_t getRequest[100];
	static uint8_t parameterVar[150];
	static uint8_t callbackVariableGet = 0;

	strcpy(getRequest, "theblast.php?get=sms/");
	strcat(getRequest, SMS_Receiver_Current);
	strcat(getRequest, "/");
	strcat(getRequest, SMS_Message_Current);

	parameterVar[0] = charactersToReturn & 0xFF;
	parameterVar[1] = (charactersToReturn & 0xFF00) >> 8;
	parameterVar[2] = (uint32_t)(&callbackVariableGet) & 0xFF;
	parameterVar[3] = ((uint32_t)(&callbackVariableGet) & 0xFF00) >> 8;
	parameterVar[4] = ((uint32_t)(&callbackVariableGet) & 0xFF0000) >> 16;
	parameterVar[5] = ((uint32_t)(&callbackVariableGet) & 0xFF000000) >> 24;
	parameterVar[6] = (uint32_t)returnBuffer & 0xFF;
	parameterVar[7] = ((uint32_t)returnBuffer & 0xFF00) >> 8;
	parameterVar[8] = ((uint32_t)returnBuffer & 0xFF0000) >> 16;
	parameterVar[9] = ((uint32_t)returnBuffer & 0xFF000000) >> 24;
	strcpy((parameterVar+10), getRequest);

	callbackVariableGet = 0;
	while (callbackVariableGet != 1 && tryTimes > 0) {
		callbackVariableGet = 0;
		if (tryTimes < GET_TRY_TIMES) vTaskDelay(500);
		xTaskCreate(TheBlast_HTTP_Get, "GetRequest", 4096, parameterVar, HTTP_CLIENT_TASK_PRIO, NULL);
		while (callbackVariableGet == 0) vTaskDelay(100);
		tryTimes--;
	}

   	vTaskDelete(NULL); // End this task!
   	while(1);
}

uint8_t TheBlast_Get_Synchronous(char * getRequest, uint16_t charactersToReturn, uint8_t * returnBuffer)
{
	uint8_t tryTimes = GET_TRY_TIMES;
	static uint8_t parameterVar[150];
	static uint8_t callbackVariableGet = 0;

	parameterVar[0] = charactersToReturn & 0xFF;
	parameterVar[1] = (charactersToReturn & 0xFF00) >> 8;
	parameterVar[2] = (uint32_t)(&callbackVariableGet) & 0xFF;
	parameterVar[3] = ((uint32_t)(&callbackVariableGet) & 0xFF00) >> 8;
	parameterVar[4] = ((uint32_t)(&callbackVariableGet) & 0xFF0000) >> 16;
	parameterVar[5] = ((uint32_t)(&callbackVariableGet) & 0xFF000000) >> 24;
	parameterVar[6] = (uint32_t)returnBuffer & 0xFF;
	parameterVar[7] = ((uint32_t)returnBuffer & 0xFF00) >> 8;
	parameterVar[8] = ((uint32_t)returnBuffer & 0xFF0000) >> 16;
	parameterVar[9] = ((uint32_t)returnBuffer & 0xFF000000) >> 24;
	strcpy((parameterVar+10), getRequest);

	callbackVariableGet = 0;
	while (callbackVariableGet != 1 && tryTimes > 0) {
		callbackVariableGet = 0;
		if (tryTimes < GET_TRY_TIMES) vTaskDelay(100);
		xTaskCreate(TheBlast_HTTP_Get, "GetRequest", 4096, parameterVar, HTTP_CLIENT_TASK_PRIO, NULL);
		while (callbackVariableGet == 0) vTaskDelay(100);
		tryTimes--;
	}
	if (tryTimes > 0) return 1;
	else return 0;
}



int8_t TheBlast_Get_Asynchronous(char * getRequest, uint16_t charactersToReturn, uint8_t * returnBuffer)
{
	uint8_t i;
	int8_t slot = -1;
	uint8_t tryTimes = GET_TRY_TIMES;
	uint8_t taskNameBuffer[20];
	static uint8_t parameterVar[150];
	static uint8_t callbackVariableGet = 0;

	for (i = 0; i < 100; i++) { // Find available slot
		if (HTTP_Asynchronous_Feedback[i] == HTTP_SLOT_AVAILABLE)
		{
			slot = i;
			break;
		}
	}
	if (slot == -1) return -1; // no slot available
	HTTP_Asynchronous_Feedback[slot] = HTTP_SLOT_PENDING;

	parameterVar[0] = charactersToReturn & 0xFF;
	parameterVar[1] = (charactersToReturn & 0xFF00) >> 8;
	parameterVar[2] = (uint8_t)slot;
	/*parameterVar[2] = (uint32_t)(&callbackVariableGet) & 0xFF;
	parameterVar[3] = ((uint32_t)(&callbackVariableGet) & 0xFF00) >> 8;
	parameterVar[4] = ((uint32_t)(&callbackVariableGet) & 0xFF0000) >> 16;
	parameterVar[5] = ((uint32_t)(&callbackVariableGet) & 0xFF000000) >> 24;*/
	parameterVar[6] = (uint32_t)returnBuffer & 0xFF;
	parameterVar[7] = ((uint32_t)returnBuffer & 0xFF00) >> 8;
	parameterVar[8] = ((uint32_t)returnBuffer & 0xFF0000) >> 16;
	parameterVar[9] = ((uint32_t)returnBuffer & 0xFF000000) >> 24;
	strcpy((parameterVar+10), getRequest);

	sprintf(taskNameBuffer, "HTTP_Async(%d)", slot);
	xTaskCreate(TheBlast_Get_Asynchronous_Task, taskNameBuffer, configMINIMAL_STACK_SIZE, parameterVar, HTTP_CLIENT_TASK_PRIO, NULL);

	return slot;
}

void TheBlast_Get_Asynchronous_Task(uint8_t * pvParameters)
{
	uint8_t tryTimes = GET_TRY_TIMES;
	static uint8_t callbackVariableGet = 0;
	int8_t slot = (int8_t)pvParameters[2];

	//parameterVar[0] = charactersToReturn & 0xFF;
	//parameterVar[1] = (charactersToReturn & 0xFF00) >> 8;
	pvParameters[2] = (uint32_t)(&callbackVariableGet) & 0xFF;
	pvParameters[3] = ((uint32_t)(&callbackVariableGet) & 0xFF00) >> 8;
	pvParameters[4] = ((uint32_t)(&callbackVariableGet) & 0xFF0000) >> 16;
	pvParameters[5] = ((uint32_t)(&callbackVariableGet) & 0xFF000000) >> 24;
	//parameterVar[6] = (uint32_t)returnBuffer & 0xFF;
	//parameterVar[7] = ((uint32_t)returnBuffer & 0xFF00) >> 8;
	//parameterVar[8] = ((uint32_t)returnBuffer & 0xFF0000) >> 16;
	//parameterVar[9] = ((uint32_t)returnBuffer & 0xFF000000) >> 24;
	//strcpy((parameterVar+10), getRequest);

	callbackVariableGet = 0;
	while (callbackVariableGet != 1 && tryTimes > 0) {
		callbackVariableGet = 0;
		if (tryTimes < GET_TRY_TIMES) vTaskDelay(100);
		xTaskCreate(TheBlast_HTTP_Get, "GetRequest", 4096, pvParameters, HTTP_CLIENT_TASK_PRIO, NULL);
		while (callbackVariableGet == 0) vTaskDelay(100);
		tryTimes--;
	}
	if (tryTimes > 0) HTTP_Asynchronous_Feedback[slot] = HTTP_SLOT_FINISH_OK;
	else HTTP_Asynchronous_Feedback[slot] = HTTP_SLOT_FINISH_ERR;

    vTaskDelete(NULL); // End this task!
	while(1);
}



void GetTime(void * pvParameters)
{
	  int client_socket;
	   struct sockaddr_in Serv_Addr;
	   struct sockaddr_in Client_Addr;
	   int addrlen=sizeof(Client_Addr);

	   int nbytes;

	   while (!DHCP_Finished) vTaskDelay(1000);

	   client_socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
	   if (client_socket < 0) ;

	   memset((char *)&Serv_Addr, 0, sizeof(Serv_Addr));
	   Serv_Addr.sin_family = AF_INET;
	   Serv_Addr.sin_len = sizeof(Serv_Addr);
	   Serv_Addr.sin_addr.s_addr = inet_addr("xx.xx.xx.xx"); // your server
	   Serv_Addr.sin_port = htons(80);

	   if (lwip_connect(client_socket, (struct sockaddr *)&Serv_Addr, sizeof(Serv_Addr)) < 0) goto ERROR;
	   printf("Connection opened\r\n");

	   strcpy(send_buffer_client, "GET /");
	   strcat(send_buffer_client, post_HTTP_header);
	   if (lwip_send(client_socket, send_buffer_client, strlen(send_buffer_client), 0) < 0) goto ERROR;
	   printf("HTTP Header sent\r\n");

	   do {
		   nbytes = lwip_recv(client_socket, recv_buffer_client, sizeof(recv_buffer_client),0);
		   vTaskDelay(50);
	   } while (nbytes == 0 && nbytes != sizeof(recv_buffer_client));
	   recv_buffer_client[nbytes] = 0x00;
	   printf("received = %d\r\n", nbytes);

	   if (lwip_close(client_socket) < 0) goto ERROR;
	   printf("Connection closed\r\n");

	   UserGUI_Hours = (recv_buffer_client[40] - '0') * 10;
	   UserGUI_Hours += (recv_buffer_client[41] - '0');
	   UserGUI_Minutes = (recv_buffer_client[43] - '0') * 10;
	   UserGUI_Minutes += (recv_buffer_client[44] - '0');
	   UserGUI_Seconds = (recv_buffer_client[46] - '0') * 10;
	   UserGUI_Seconds += (recv_buffer_client[47] - '0');

	   UserGUI_Hours += 1; // Adjust timezone (from GMT+0 to GMT+1)
	   if (UserGUI_Hours >= 24) UserGUI_Hours -= 24;

	   printf("Time is: %02u:%02u:%02u\r\n", UserGUI_Hours, UserGUI_Minutes, UserGUI_Seconds);

	   vTaskDelete(NULL); // End this task!
	   while(1);

ERROR:
	   printf("An error occoured fetching the time! Closing connection...\r\n");
	   lwip_close(client_socket);
	   printf("Connection closed\r\n");

	   vTaskDelete(NULL); // End this task!
	   while(1);
}

void TheBlast_HTTP_Get(void * pvParameters)
{
	  int client_socket;
	   struct sockaddr_in Serv_Addr;
	   struct sockaddr_in Client_Addr;
	   int addrlen=sizeof(Client_Addr);

	   int readPos, writePos, nbytes;
	   long totalBytes = 0;
	   int offset;

	   uint8_t * parameterPointer = (u8*)pvParameters;
	   char httpAddress[200];
	   char * dataPointer;
	   uint16_t i;

	   uint16_t charactersToReturn = *(uint16_t *)((uint32_t)parameterPointer);
	   uint32_t CallbackVariableLoc = *(uint32_t *)((uint32_t)parameterPointer+2);
	   uint32_t returnBufferLoc = *(uint32_t *)((uint32_t)parameterPointer+6);


	   readPos = 10;
	   writePos = 0;
	   while (parameterPointer[readPos] != 0x00 && writePos < 199) {
		   if (parameterPointer[readPos] != ' ') {
			   httpAddress[writePos++] = parameterPointer[readPos++];
		   } else {
			   httpAddress[writePos++] = '%';
			   httpAddress[writePos++] = '2';
			   httpAddress[writePos++] = '0';
			   readPos++;
		   }
	   }
	   httpAddress[writePos] = 0x00;

	   client_socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
	   if (client_socket < 0) ;

	   memset((char *)&Serv_Addr, 0, sizeof(Serv_Addr));
	   Serv_Addr.sin_family = AF_INET;
	   Serv_Addr.sin_len = sizeof(Serv_Addr);
	   Serv_Addr.sin_addr.s_addr = inet_addr("xx.xx.xx.xx"); // server address
	   Serv_Addr.sin_port = htons(80);

	   printf("Opening connection...\r\n");
	   if (lwip_connect(client_socket, (struct sockaddr *)&Serv_Addr, sizeof(Serv_Addr)) < 0) goto ERROR_TIMEOUT;
	   printf("Connection opened\r\n");

	   strcpy(send_buffer_client, "GET /");
	   strcat(send_buffer_client, httpAddress);
	   strcat(send_buffer_client, TheBlast_HTTP_Get_Header);
	   if (lwip_send(client_socket, send_buffer_client, strlen(send_buffer_client), 0) < 0) goto ERROR;

	   printf("HTTP Header sent\r\n");
	   vTaskDelay(100);

	   // Receive the first package
	   do {
		   nbytes = lwip_recv(client_socket, recv_buffer_client, sizeof(recv_buffer_client), 0);
		   vTaskDelay(50);
	   } while (nbytes == 0);

	   totalBytes += nbytes;

	   writePos = 0;

	   dataPointer = strstr(recv_buffer_client, "200 OK");
	   if (dataPointer == NULL) goto ERROR_NOT_OK;	   // Not OK response

	   dataPointer = strstr(recv_buffer_client, "\r\n\r\n");
	   if (dataPointer == NULL) goto ERROR_NOT_OK;
	   else dataPointer += 4;

	   offset = dataPointer - recv_buffer_client;

	   readPos = offset;
	   while (writePos < charactersToReturn && readPos < nbytes) {
		   *(uint16_t *) (returnBufferLoc++) = *dataPointer++;
		   readPos++;
		   writePos++;
	   }

	   // Receive the rest
	   do {
		   nbytes = lwip_recv(client_socket, recv_buffer_client, sizeof(recv_buffer_client),0);
		   totalBytes += nbytes;

		   readPos = 0;
		   dataPointer = recv_buffer_client;
		   while (writePos < charactersToReturn && readPos < nbytes) {
			   *(uint16_t *) (returnBufferLoc++) = *dataPointer++;
			   readPos++;
			   writePos++;
		   }
	   } while (nbytes == sizeof(recv_buffer_client));

	   *(uint16_t *) (returnBufferLoc) = 0;
	   printf("Totally received = %u\r\n", totalBytes);

	   if (lwip_close(client_socket) < 0) goto ERROR;
	   printf("Connection closed\r\n");

	   *(uint8_t *) (CallbackVariableLoc) = 1;
	   goto DEL_TASK;

	   ERROR_TIMEOUT:
	   	   printf("Timeout error! Closing connection...\r\n");
	   	   lwip_close(client_socket);
	   	   printf("Connection closed\r\n");
	   	   *(uint8_t *) (CallbackVariableLoc) = 2;
	   	   goto DEL_TASK;

	   ERROR_NOT_OK:
	      	   printf("Response is not 200 OK! Closing connection...\r\n");
	      	   lwip_close(client_socket);
	      	   printf("Connection closed\r\n");
	      	   *(uint8_t *) (CallbackVariableLoc) = 3;
	      	   goto DEL_TASK;

	   ERROR_NO_BMP:
	   	   printf("The response is not a BMP! Closing connection...\r\n");
	      	   lwip_close(client_socket);
	     	   printf("Connection closed\r\n");
	     	  *(uint8_t *) (CallbackVariableLoc) = 4;
	     	   goto DEL_TASK;

	   ERROR:
	     	 	printf("An error occoured! Closing connection...\r\n");
	     	    lwip_close(client_socket);
	     	   	printf("Connection closed\r\n");
	     	   *(uint8_t *) (CallbackVariableLoc) = 5;
	     	   	goto DEL_TASK;


	   DEL_TASK:
	   	   	   vTaskDelete(NULL); // End this task!
	   	   	   while(1);
}

