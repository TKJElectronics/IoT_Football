#include "Admin.h"


void UserGUI_Admin_Show(void)
{
	GUI_SetDisplay(LCD_BOTH);
	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_DisplayRAW(368, 10, theblast);
	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Text(20, 110, "Admin Menu", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Rectangle(21, 161, 459, 216, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 161, 459, 216, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 160, 460, 215, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 165, 459, 215, "Tving Standard-mode", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	GUI_Rectangle(21, 241, 459, 296, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 241, 459, 296, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 240, 460, 295, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 245, 459, 245, "Frigiv bold", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	GUI_Rectangle(21, 320, 459, 376, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 321, 459, 376, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 320, 460, 375, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 325, 459, 325, "Reboot", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	GUI_DisplayRAW(40, 520, tkj_logo); // Display TKJ Electronics logo

	GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
}

void UserGUI_Admin_Task(void)
{
	if (currentTouchedLCD) // LCD is touched
	{
		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 165 && TouchY <= 215) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 161, 459, 216, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 161, 459, 216, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 160, 460, 215, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 165, 459, 215, "Tving Standard-mode", Arial28R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 161, 459, 216, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 161, 459, 216, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 160, 460, 215, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 165, 459, 215, "Tving Standard-mode", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_NONBLAST_MAIN);
			return;
		}

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 240 && TouchY <= 295) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 241, 459, 296, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 241, 459, 296, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 240, 460, 295, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 245, 459, 245, "Frigiv bold", Arial28R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 241, 459, 296, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 241, 459, 296, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 240, 460, 295, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 245, 459, 245, "Frigiv bold", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			BallControl_ServeBall();
			currentTouchedLCD = 0;
			return;
		}

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 320 && TouchY <= 375) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 320, 459, 376, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 321, 459, 376, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 320, 460, 375, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 325, 459, 325, "Reboot", Arial28R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 320, 459, 376, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 321, 459, 376, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 320, 460, 375, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 325, 459, 325, "Reboot", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			NVIC_SystemReset();
			return;
		}

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 710 && TouchY <= 765) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_MAIN);
			return;
		}
	}

	currentTouchedLCD = 0;
}
