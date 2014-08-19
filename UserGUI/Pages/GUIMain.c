#include "GUIMain.h"

void UserGUI_Main_Show(void)
{
	GUI_SetDisplay(LCD_BOTH);
	currentLCDinCharge = LCD_BOTH;
	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_DisplayRAW(368, 10, theblast);

	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Rectangle(21, 141, 459, 291, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 141, 459, 291, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 140, 460, 292, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 162, 459, 162, "Spil for sjov", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 215, 459, 215, "Deltagerne registreres IKKE p\x85", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 247, 459, 247, "hjemmesiden.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	GUI_Rectangle(21, 364, 459, 514, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 364, 459, 514, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 363, 460, 515, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 385, 459, 385, "Spil for sjov", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 438, 459, 438, "Spil med deltagerne registreret p\x85", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 470, 459, 470, "hjemmesiden.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	GUI_Rectangle(21, 587, 459, 737, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 587, 459, 737, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 586, 460, 738, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 620, 459, 620, "Konkurrence", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 668, 459, 668, "Benyttes kun til konkurrence afvikling.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	selectedGameMode = GAME_NONE;
	RGB_Strip_Fade(RGB_STRIP_IDLE_RED, RGB_STRIP_IDLE_GREEN, RGB_STRIP_IDLE_BLUE);
	TiltDetectionEnabled = 0;
}

void UserGUI_Main_Task(void)
{
	if (currentTouchedLCD) // LCD is touched
	{
		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 140 && TouchY <= 292) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 141, 459, 291, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 141, 459, 291, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 140, 460, 292, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 162, 459, 162, "Spil for sjov", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 215, 459, 215, "Deltagerne registreres IKKE p\x85", Arial18R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 247, 459, 247, "hjemmesiden.", Arial18R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 141, 459, 291, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 141, 459, 291, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 140, 460, 292, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 162, 459, 162, "Spil for sjov", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 215, 459, 215, "Deltagerne registreres IKKE p\x85", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 247, 459, 247, "hjemmesiden.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

			MatchID = 0; // no reg match
			NoRegSlotID = 0;
			selectedGameMode = GAME_FUN_NOREG;

			NoRegSlotID = API_RegisterFunMatchNoReg(NoRegSlotBuffer);
			if (NoRegSlotID > 0) {
				currentLCDinCharge = LCD_BOTH;
				GUI_SetDisplay(LCD_BOTH);
				UserGUI_Score_Init();
				UserGUI_Show(USERGUI_SCORE); // Start the game
			}
			return;
		}

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 363 && TouchY <= 515) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 364, 459, 514, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 364, 459, 514, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 363, 460, 515, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 385, 459, 385, "Spil for sjov", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 438, 459, 438, "Spil med deltagerne registreret p\x85", Arial18R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 470, 459, 470, "hjemmesiden.", Arial18R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 364, 459, 514, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 364, 459, 514, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 363, 460, 515, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 385, 459, 385, "Spil for sjov", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 438, 459, 438, "Spil med deltagerne registreret p\x85", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 470, 459, 470, "hjemmesiden.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

			currentLCDinCharge = currentTouchedLCD;
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_FUN_SELECT);
			return;
		}

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 586 && TouchY <= 738) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 587, 459, 737, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 587, 459, 737, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 586, 460, 738, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 620, 459, 620, "Konkurrence", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 668, 459, 668, "Benyttes kun til konkurrence afvikling.", Arial18R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 587, 459, 737, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 587, 459, 737, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 586, 460, 738, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 620, 459, 620, "Konkurrence", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 668, 459, 668, "Benyttes kun til konkurrence afvikling.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

			selectedGameMode = GAME_MATCH;

			currentLCDinCharge = currentTouchedLCD;
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_PLAYER_REGISTER);
			return;
		}
	}

	currentTouchedLCD = 0;
}
