#include "RebootMatch.h"
#include "Score.h"

void UserGUI_RebootMatch_Init(uint8_t * rebootString)
{
	uint8_t tempBuffer[20];
	uint8_t * pointerStart;
	uint8_t * pointerEnd;
	currentScoreDisplay = SCORE_GAME_RUNNING;

	//    27/2|10/0|0

	pointerStart = rebootString;
	pointerEnd = strstr(pointerStart, "/");
	if (pointerEnd == 0) {
		waitForAPIrequest = 0;
		return; // exit and boot normally
	}
	strncpy(tempBuffer, pointerStart, (pointerEnd-pointerStart));
	tempBuffer[(pointerEnd-pointerStart)] = 0;
	MatchID = atoi(tempBuffer);
	if (MatchID == 0) {
		waitForAPIrequest = 0;
		return; // exit and boot normally
	}

	pointerStart = pointerEnd+1;
	pointerEnd = strstr(pointerStart, "|");
	if (pointerEnd == 0) {
		waitForAPIrequest = 0;
		return; // exit and boot normally
	}
	strncpy(tempBuffer, pointerStart, (pointerEnd-pointerStart));
	tempBuffer[(pointerEnd-pointerStart)] = 0;
	BlueTeamScore = atoi(tempBuffer);

	pointerStart = pointerEnd+1;
	pointerEnd = strstr(pointerStart, "/");
	if (pointerEnd == 0) {
		waitForAPIrequest = 0;
		return; // exit and boot normally
	}
	strncpy(tempBuffer, pointerStart, (pointerEnd-pointerStart));
	tempBuffer[(pointerEnd-pointerStart)] = 0;
	RedTeamScore = atoi(tempBuffer);

	pointerStart = pointerEnd+1;
	pointerEnd = strstr(pointerStart, "|");
	if (pointerEnd == 0) {
		waitForAPIrequest = 0;
		return; // exit and boot normally
	}
	strncpy(tempBuffer, pointerStart, (pointerEnd-pointerStart));
	tempBuffer[(pointerEnd-pointerStart)] = 0;
	BlueTeamTilts = atoi(tempBuffer);

	pointerStart = pointerEnd+1;
	pointerEnd = rebootString + strlen(rebootString);
	if (pointerEnd == 0) {
		waitForAPIrequest = 0;
		return; // exit and boot normally
	}
	strncpy(tempBuffer, pointerStart, (pointerEnd-pointerStart));
	tempBuffer[(pointerEnd-pointerStart)] = 0;
	RedTeamTilts = atoi(tempBuffer);

	RGB_Strip_Fade(RGB_STRIP_GAMEIDLE_RED, RGB_STRIP_GAMEIDLE_GREEN, RGB_STRIP_GAMEIDLE_BLUE);

	IRDetectors_ClearAllFlags();
	Accelerometer_Calibrate();
	Accelerometer_TiltDetected = ACCELEROMETER_CLEAR;
	gameServeFirst = 1;  // Serve the first ball

	//selectedGameMode = GAME_MATCH;
	selectedGameMode = GAME_FUN_NOREG; // just run as No Reg to enable "Afslut kamp" button
	waitForAPIrequest = 2; // Show RebootMatch screen
}

void UserGUI_RebootMatch_Show(void)
{
	GUI_SetDisplay(LCD_BOTH);
	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_DisplayRAW(368, 10, theblast);
	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Text(20, 120, "Igangv\x83rende kamp", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
	GUI_Text(20, 170, "Scor bolde og tryk Forts\x83t", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Rectangle(21, 251, 459, 551, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 251, 459, 551, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 250, 460, 550, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 370, 459, 370, "Forts\x83t spil", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
}

void UserGUI_RebootMatch_Task(void)
{
	if (currentTouchedLCD) // LCD is touched
	{
		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 200 && TouchY <= 500) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 251, 459, 551, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 251, 459, 551, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 250, 460, 550, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 370, 459, 370, "Forts\x83t spil", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 251, 459, 551, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 251, 459, 551, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 250, 460, 550, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 370, 459, 370, "Forts\x83t spil", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_SetDisplay(LCD_BOTH);
			IRDetectors_ClearAllFlags();
			Accelerometer_TiltDetected = ACCELEROMETER_CLEAR;
			UserGUI_Show(USERGUI_SCORE); // Start the game
			return;
		}
	}

	currentTouchedLCD = 0;
}
