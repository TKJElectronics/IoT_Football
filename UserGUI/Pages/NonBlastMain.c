#include "NonBlastMain.h"

void UserGUI_NonBlast_Main_Show(void)
{
	GUI_SetDisplay(LCD_BOTH);
	GUI_Clear(GUI_COLOR_BACKGROUND);

	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Rectangle(21, 261, 459, 521, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 261, 459, 521, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 260, 460, 520, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 340, 459, 340, "Tryk for at", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 390, 459, 390, "starte nyt spil", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	selectedGameMode = GAME_NONE;
}

void UserGUI_NonBlast_Main_Task(void)
{
	if (currentTouchedLCD) // LCD is touched
	{
		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 260 && TouchY <= 520) {
			GUI_SetDisplay(currentTouchedLCD);
			GUI_Rectangle(21, 261, 459, 521, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 261, 459, 521, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 260, 460, 520, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 340, 459, 340, "Tryk for at", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 390, 459, 390, "starte nyt spil", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 261, 459, 521, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 261, 459, 521, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 260, 460, 520, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 340, 459, 340, "Tryk for at", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 390, 459, 390, "starte nyt spil", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

			selectedGameMode = GAME_NONBLAST;
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Score_Init();
			UserGUI_Show(USERGUI_SCORE); // Start the game
			return;
		}
	}

	currentTouchedLCD = 0;
}
