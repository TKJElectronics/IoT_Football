#include "Fun_Select.h"

void UserGUI_Fun_Select_Show(void)
{
	UserGUI_DisplayLock(currentLCDinCharge);

	GUI_SetDisplay(currentLCDinCharge);
	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_DisplayRAW(368, 10, theblast);
	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Text(20, 170, "V\x83lg type", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Rectangle(21, 241, 459, 391, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 241, 459, 391, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 240, 460, 390, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 274, 459, 274, "1 on 1", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 322, 459, 322, "2 spillere i alt", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	GUI_Rectangle(21, 441, 459, 591, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 441, 459, 591, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 440, 460, 590, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 474, 459, 474, "2 on 2", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	GUI_Text_Center(21, 522, 459, 522, "4 spillere i alt", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
}

void UserGUI_Fun_Select_Task(void)
{
	if (currentTouchedLCD) // LCD is touched
	{
		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 240 && TouchY <= 390) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 241, 459, 391, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 241, 459, 391, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 240, 460, 390, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 274, 459, 274, "1 on 1", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 322, 459, 322, "2 spillere i alt", Arial18R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 241, 459, 391, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 241, 459, 391, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 240, 460, 390, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 274, 459, 274, "1 on 1", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 322, 459, 322, "2 spillere i alt", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

			selectedGameMode = GAME_FUN_1ON1;
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_PLAYER_REGISTER);
			return;
		}

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 440 && TouchY <= 590) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 441, 459, 591, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 441, 459, 591, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 440, 460, 590, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 474, 459, 474, "2 on 2", Arial34R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			GUI_Text_Center(21, 522, 459, 522, "4 spillere i alt", Arial18R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 441, 459, 591, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 441, 459, 591, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 440, 460, 590, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 474, 459, 474, "2 on 2", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_Text_Center(21, 522, 459, 522, "4 spillere i alt", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

			selectedGameMode = GAME_FUN_2ON2;
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_PLAYER_REGISTER);
			return;
		}

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 710 && TouchY <= 765) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_MAIN);
			return;
		}
	}

	currentTouchedLCD = 0;
}
