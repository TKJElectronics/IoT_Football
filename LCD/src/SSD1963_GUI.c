#include "stm32f4xx.h"
#include "SSD1963_GUI.h"

uint8_t currentConsoleLine;

#define CONSOLE_TEXT_COLOR			WHITE
#define CONSOLE_BACKGROUND_COLOR	0x41E6


void GUI_InitializeConsole(void)
{
	GUI_Clear(CONSOLE_BACKGROUND_COLOR);
	currentConsoleLine = 0;
}

void GUI_ConsolePrint(uint8_t * text)
{
	if (currentConsoleLine > 15) {
		GUI_Clear(BLACK);
		currentConsoleLine = 0;
	}
	GUI_Text(10, (currentConsoleLine*30)+10, text, Arial18R, CONSOLE_TEXT_COLOR, CONSOLE_BACKGROUND_COLOR);
	currentConsoleLine++;
}
