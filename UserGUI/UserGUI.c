#include "UserGUI.h"

USERGUI_PAGE CurrentPage = USERGUI_MAIN;

uint16_t TouchX, TouchY;
LCD_Control currentTouchedLCD;
LCD_Control currentLCDinCharge = LCD_BOTH;

USERGUI_GAMEMODE selectedGameMode = GAME_NONE;

uint8_t stringBuffer[200];

uint8_t PlayerBarcodes[4][50];
uint8_t PlayerIDs[4][50];
uint8_t PlayerNames[4][50];
uint32_t MatchID;
uint8_t NoRegSlotID;
uint8_t NoRegSlotBuffer[200];

uint8_t API_Network_Error = 0;
char waitForAPIrequest = 1;


void UserGUI_Show(USERGUI_PAGE newPage)
{
	GUI_FadeBacklight(FADE_DOWN);

	switch (newPage)
	{
		case USERGUI_MAIN:
			UserGUI_Main_Show();
			break;
		case USERGUI_NONBLAST_MAIN:
			UserGUI_NonBlast_Main_Show();
			break;
		case USERGUI_SCORE:
			UserGUI_Score_Show();
			break;
		case USERGUI_FUN_SELECT:
			UserGUI_Fun_Select_Show();
			break;
		case USERGUI_PLAYER_REGISTER:
			UserGUI_Player_Register_Show();
			break;
		case USERGUI_ADMIN:
			UserGUI_Admin_Show();
			break;
		case USERGUI_REBOOT_MATCH:
			UserGUI_RebootMatch_Show();
			break;
		case USERGUI_API_ERROR:
			UserGUI_API_Error_Show();
			break;
	}

	if (LCD1_Current_CS_Pin != 0 && LCD2_Current_CS_Pin != 0) {
		GUI_SetDisplay(LCD1);
		GUI_FadeBacklight(FADE_UP);
		vTaskDelay(500);
		GUI_SetDisplay(LCD2);
		GUI_FadeBacklight(FADE_UP);
		vTaskDelay(500);
		GUI_SetDisplay(LCD_BOTH);
	} else {
		GUI_FadeBacklight(FADE_UP);
	}

	currentTouchedLCD = 0;
	CurrentPage = newPage;
}


void UserGUI_Task(void * pvParameters)
{
	IRDetectors_ClearAllFlags();
	/*GUI_SetDisplay(LCD1);
	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_FadeBacklight(FADE_UP);
	vTaskDelay(1000);
	GUI_SetDisplay(LCD2);
	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_SetDisplay(LCD2);
	GUI_FadeBacklight(FADE_UP);
	vTaskDelay(500);*/

	GUI_SetDisplay(LCD_BOTH);
	if (waitForAPIrequest == 2) { // boot previous/ongoing match
		waitForAPIrequest = 0; // reset
		UserGUI_Show(USERGUI_REBOOT_MATCH);
	} else { // boot normally
		UserGUI_Show(USERGUI_MAIN);
	}

	Accelerometer_SetOffsetX();

	while (1)
	{
		if (API_Network_Error == 1) { // We have an API error!
			UserGUI_Show(USERGUI_API_ERROR);
			while (API_Network_Error == 1) vTaskDelay(1000);
			UserGUI_Show(USERGUI_MAIN);
		}

		switch (CurrentPage)
		{
			case USERGUI_MAIN:
				UserGUI_Main_Task();
				break;
			case USERGUI_SCORE:
				UserGUI_Score_Task();
				break;
			case USERGUI_FUN_SELECT:
				UserGUI_Fun_Select_Task();
				break;
			case USERGUI_PLAYER_REGISTER:
				UserGUI_Player_Register_Task();
				break;
			case USERGUI_ADMIN:
				UserGUI_Admin_Task();
				break;
			case USERGUI_REBOOT_MATCH:
				UserGUI_RebootMatch_Task();
				break;
			case USERGUI_NONBLAST_MAIN:
				UserGUI_NonBlast_Main_Task();
				break;
		}
		vTaskDelay(50);
	}
}

void UserGUI_NonBlast_Task(void * pvParameters)
{
	UserGUI_Show(USERGUI_NONBLAST_MAIN);

	while (1)
	{
		switch (CurrentPage)
		{
			case USERGUI_NONBLAST_MAIN:
				UserGUI_NonBlast_Main_Task();
				break;
			case USERGUI_SCORE:
				UserGUI_Score_Task();
				break;
		}
		vTaskDelay(50);
	}
}

void UserGUI_TouchTask(void * pvParameters)
{
	uint8_t touchCount = 0;
	while (1)
	{
		if (LCD1_TOUCH_IRQ && (currentLCDinCharge & LCD1)) {
			if (touchCount < 5) {
				TouchX = (Touch_ReadAvgX(10, LCD1) * GetMaxX()) / 4096;
				TouchY = (Touch_ReadAvgY(10, LCD1) * GetMaxY()) / 4096;
				TouchX = GetMaxX() - TouchX; // Invert x
				TouchY = GetMaxY() - TouchY; // Invert y
				currentTouchedLCD = LCD1;
			}
		} else if (LCD2_TOUCH_IRQ && (currentLCDinCharge & LCD2)) {
			if (touchCount < 5) {
				TouchX = (Touch_ReadAvgX(10, LCD2) * GetMaxX()) / 4096;
				TouchY = (Touch_ReadAvgY(10, LCD2) * GetMaxY()) / 4096;
				TouchX = GetMaxX() - TouchX; // Invert x
				TouchY = GetMaxY() - TouchY; // Invert y
				currentTouchedLCD = LCD2;
			}
		} else if (touchCount > 0) {
			touchCount = 0;
		}
		vTaskDelay(10);
	}
}

void UserGUI_DisplayLock(LCD_Control LCDInChage) // Display lock on other display than the one in charge!
{
	/*LCD_Control oldLCDControl = 0;
	if (LCD1_Current_CS_Pin != 0) oldLCDControl |= LCD1;
	if (LCD2_Current_CS_Pin != 0) oldLCDControl |= LCD2;*/

	LCD_Control LCDtoLock;
	if (LCDInChage == 0 || LCDInChage == LCD_BOTH) return; // not possible to lock none or both!
	else if (LCDInChage == LCD1) LCDtoLock = LCD2;
	else if (LCDInChage == LCD2) LCDtoLock = LCD1;

	GUI_SetDisplay(LCDtoLock);

	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_DisplayRAW(368, 10, theblast);

	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_DisplayRAW(90, 230, lock);

	GUI_FadeBacklight(FADE_UP);

	//GUI_SetDisplay(oldLCDControl); // restore LCD control state
}

void UserGUI_API_Error_Show(void)
{
	GUI_SetDisplay(LCD_BOTH);

	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_DisplayRAW(368, 10, theblast);

	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Text(20, 80, "API ERROR", Arial34R, GUI_COLOR_ERROR_RED, GUI_COLOR_BACKGROUND);

	GUI_DisplayRAW(90, 230, lock);
}
