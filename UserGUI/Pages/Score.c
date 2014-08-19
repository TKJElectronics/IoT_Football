#include "Score.h"

uint8_t scoreCounter;
uint32_t scoreTickCounter;
uint8_t currentScoreDisplay = 0;
uint32_t interactionTimestamp;
uint32_t beepTimestamp;
uint32_t winnerTimestamp;
uint8_t buzzerState;


int8_t BlueTeamScore;
uint8_t BlueTeamTilts;
int8_t RedTeamScore;
uint8_t RedTeamTilts;
int8_t scoreAPIslot = -1;
uint8_t gameServeFirst = 0;

uint8_t easterEgg_ScoreSide = 0;
uint32_t easterEgg_Timeout = 0;

uint8_t API_ScoreSlot;
uint8_t API_SlotStatus;
uint8_t API_Score_ReturnBuffer[50];


void UserGUI_Score_Init(void)
{
	currentScoreDisplay = SCORE_GAME_RUNNING;

	RedTeamScore = 0;
	RedTeamTilts = 0;
	BlueTeamScore = 0;
	BlueTeamTilts = 0;

	RGB_Strip_Fade(RGB_STRIP_GAMEIDLE_RED, RGB_STRIP_GAMEIDLE_GREEN, RGB_STRIP_GAMEIDLE_BLUE);

	IRDetectors_ClearAllFlags();
	Accelerometer_TiltDetected = ACCELEROMETER_CLEAR;
	TiltDetectionEnabled = 1;
	gameServeFirst = 1;  // Serve the first ball
}

void UserGUI_Score_Show(void)
{
	if (gameServeFirst == 1) {
		//Accelerometer_Calibrate();
		//Accelerometer_SetOffsetX(); // maybe not a good idea to calibrate here?
		Buzzer_Set(0);
		BallControl_ServeBall(); // Serve the first ball
		BallControl_Cooldown = millis() + SCORE_COOLDOWN;
		Accelerometer_TiltDetected = ACCELEROMETER_CLEAR;
		gameServeFirst = 0;
	}

	GUI_SetDisplay(LCD_BOTH);
	if (currentScoreDisplay == SCORE_GAME_RUNNING) {
		GUI_Clear(GUI_COLOR_BACKGROUND);
		GUI_DisplayRAW(368, 10, theblast);

		GUI_Text(20, 60, "Score", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

		GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
		GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);

		sprintf(stringBuffer, "%d", BlueTeamScore);
		GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);

		GUI_Rectangle(21, 426, 459, 626, GUI_COLOR_TEAM_RED, 1);
		GUI_Rectangle(21, 426, 459, 626, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 425, 460, 625, GUI_COLOR_BORDER, 0);
		sprintf(stringBuffer, "%d", RedTeamScore);
		GUI_Text_Center(21, 460, 459, 460, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_RED);

		if (selectedGameMode != GAME_MATCH) {
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 715, 459, 715, "Afslut nu", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
		}
	} else if (currentScoreDisplay == SCORE_GAME_RUNNING_DARK) {
		GUI_Clear(GUI_COLOR_DARK_BACKGROUND);

		GUI_Text(20, 35, "Score", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_DARK_BACKGROUND);

		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_TEAM_BLUE, 1);
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);

		sprintf(stringBuffer, "%d", BlueTeamScore);
		GUI_Text_Center(21, 215, 459, 215, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);

		GUI_Rectangle(21, 421, 459, 621, GUI_COLOR_TEAM_RED, 1);
		GUI_Rectangle(21, 421, 459, 621, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 420, 460, 620, GUI_COLOR_BORDER, 0);

		sprintf(stringBuffer, "%d", RedTeamScore);
		GUI_Text_Center(21, 455, 459, 455, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_RED);
	} else if (currentScoreDisplay == SCORE_WINNER_BLUE) { // blue winner
		GUI_Clear(GUI_COLOR_BACKGROUND);
		GUI_DisplayRAW(368, 10, theblast);

		GUI_Text(20, 110, "Vinder", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_TEAM_BLUE, 1);
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 215, 459, 215, "Bl\x85t hold vandt!", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);
		GUI_Text_Center(21, 290, 459, 290, "Tillykke", Arial28R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);

		GUI_Rectangle(21, 421, 459, 651, GUI_COLOR_ORANGE, 1);
		GUI_Rectangle(21, 421, 459, 651, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 420, 460, 650, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 440, 459, 440, "Score:", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_ORANGE);

		sprintf(stringBuffer, "%d-%d", BlueTeamScore, RedTeamScore);
		GUI_Text_Center(21, 500, 459, 500, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_ORANGE);

		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	} else if (currentScoreDisplay == SCORE_WINNER_RED) { // red winner
		GUI_Clear(GUI_COLOR_BACKGROUND);
		GUI_DisplayRAW(368, 10, theblast);

		GUI_Text(20, 110, "Vinder", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_TEAM_RED, 1);
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 215, 459, 215, "R\x84\x64t hold vandt!", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_RED);
		GUI_Text_Center(21, 290, 459, 290, "Tillykke", Arial28R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_RED);

		GUI_Rectangle(21, 421, 459, 651, GUI_COLOR_ORANGE, 1);
		GUI_Rectangle(21, 421, 459, 651, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 420, 460, 650, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 440, 459, 440, "Score:", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_ORANGE);

		sprintf(stringBuffer, "%d-%d", RedTeamScore, BlueTeamScore);
		GUI_Text_Center(21, 500, 459, 500, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_ORANGE);

		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
	}

	scoreCounter = 0;
	scoreTickCounter = millis();

	interactionTimestamp = millis();
}

void UserGUI_Score_Task(void)
{
	uint8_t i;
	uint8_t readBarcodeLength;

	if (currentTouchedLCD) // LCD is touched
	{
		interactionTimestamp = millis();
		/*if (TouchX >= 20 && TouchX <= 460 && TouchY >= 200 && TouchY <= 600) {
			GUI_SetDisplay(currentTouchedLCD);
			if (currentScoreDisplay < 3) currentScoreDisplay++;
			else currentScoreDisplay = 0;
			UserGUI_Score_Show();
			return;
		}*/

		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 710 && TouchY <= 765 && currentScoreDisplay == SCORE_GAME_RUNNING && selectedGameMode != GAME_MATCH) {
			GUI_SetDisplay(currentTouchedLCD);

			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BUTTON, 0);
			GUI_Text_Center(21, 715, 459, 715, "Afslut nu", Arial28R, GUI_COLOR_BUTTON, GUI_COLOR_BORDER);
			while (LCD1_TOUCH_IRQ || LCD2_TOUCH_IRQ) vTaskDelay(10);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);

			if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_FUN_1ON1 || selectedGameMode == GAME_FUN_NOREG) {
				GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				API_FinalizeMatch("fm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
			} else {
				GUI_Text_Center(21, 715, 459, 715, "Afslut nu", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			}
			//else if (selectedGameMode == GAME_MATCH)  // Burde ikke være nødvendigt, da afslut knappen ikke er der under Match mode
			//	API_FinalizeMatch("gm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);

			GUI_SetDisplay(LCD_BOTH);
			if (selectedGameMode == GAME_NONBLAST) {
				UserGUI_Show(USERGUI_NONBLAST_MAIN);
			} else {
				UserGUI_Show(USERGUI_MAIN);
			}
			return;
		}

		// Easter egg - forced score - BLUE
		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 130 && TouchY <= 330 && currentScoreDisplay == SCORE_GAME_RUNNING && selectedGameMode != GAME_MATCH) {
			easterEgg_ScoreSide = 1; // BLUE
			easterEgg_Timeout = millis() + EASTEREGG_FORCEDSCORE_TIMEOUT;
			BarcodeScanner_InitiateScan();
		}

		// Easter egg - forced score - RED
		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 425 && TouchY <= 625 && currentScoreDisplay == SCORE_GAME_RUNNING && selectedGameMode != GAME_MATCH) {
			easterEgg_ScoreSide = 2; // RED
			easterEgg_Timeout = millis() + EASTEREGG_FORCEDSCORE_TIMEOUT;
			BarcodeScanner_InitiateScan();
		}

		if (easterEgg_Timeout > 0 && easterEgg_ScoreSide > 0 && currentScoreDisplay == SCORE_GAME_RUNNING && selectedGameMode != GAME_MATCH) {
			readBarcodeLength = BarcodeScanner_GetScan(); // check for GameMaster scan
			if (readBarcodeLength > 0) {
				if (strcmp(BarcodeScanner_Buffer, GAMEMASTER_CARD) == 0) {
					if (easterEgg_ScoreSide == 1) {
						easterEgg_ScoreSide = 0;
						easterEgg_Timeout = 0;
						goto BlueScoreCall;
					} else if (easterEgg_ScoreSide == 2) {
						easterEgg_ScoreSide = 0;
						easterEgg_Timeout = 0;
						goto RedScoreCall;
					}
				}
			}
		} else if (easterEgg_ScoreSide > 0 && easterEgg_Timeout == 0) {
			easterEgg_ScoreSide = 0;
			BarcodeScanner_StopScan(); // Stop any ongoing scans
		}


		if (TouchX >= 20 && TouchX <= 460 && TouchY >= 710 && TouchY <= 765 && (currentScoreDisplay == SCORE_WINNER_BLUE || currentScoreDisplay == SCORE_WINNER_RED)) {
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
			if (selectedGameMode == GAME_NONBLAST) {
				UserGUI_Show(USERGUI_NONBLAST_MAIN);
			} else {
				UserGUI_Show(USERGUI_MAIN);
			}
			return;
		}
	}

	if (currentScoreDisplay == SCORE_WINNER_BLUE || currentScoreDisplay == SCORE_WINNER_RED) {
		if (millis() > (beepTimestamp+SCORE_BEEP_INTERVAL)) {
			for (i = 25; i > 0; i--) {
				Buzzer_Set(1);
				vTaskDelay(2*i);
				Buzzer_Set(0);
				vTaskDelay(i);
			}
			beepTimestamp = millis();
		}

		if (millis() > winnerTimestamp) {
			GUI_SetDisplay(LCD_BOTH);
			if (selectedGameMode == GAME_NONBLAST) {
				UserGUI_Show(USERGUI_NONBLAST_MAIN);
			} else {
				UserGUI_Show(USERGUI_MAIN);
			}
		}
	}

	if (IRDetectors_GetFlag(LEFT_GOAL) && (currentScoreDisplay == SCORE_GAME_RUNNING || currentScoreDisplay == SCORE_GAME_RUNNING_DARK)) { // Left = Red
		interactionTimestamp = millis();

		if (BallControl_Cooldown != 0) { // score too fast
			vTaskDelay(500);
			IRDetectors_ClearFlag(LEFT_GOAL);
		} else {
RedScoreCall:
			RedTeamScore++;
			UserGUI_Score_Update();
			RGB_Strip_Mode = RGB_STRIP_GAME_SCORE_RED;

			if (selectedGameMode != GAME_NONBLAST && RedTeamScore != WINNER_SCORE) {
				API_SlotStatus = API_UpdateMatchGetStatus(API_ScoreSlot);
				if (API_SlotStatus == HTTP_SLOT_AVAILABLE || API_SlotStatus == HTTP_SLOT_FINISH_OK) { // Everything OK - just continue
					API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
				}
				else if (API_SlotStatus == HTTP_SLOT_PENDING) { // Old API call still waiting! :O
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
					while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
					API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
					GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				}
				else if (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_FINISH_ERR) { // Gammel API kald blev ikke gemt ordentligt, prøv det nye
					API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
					while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
					GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				}
			}

			vTaskDelay(500);
			IRDetectors_ClearFlag(LEFT_GOAL);
			Accelerometer_TiltDetected = ACCELEROMETER_CLEAR; // clear a evt. faulty tilt
			RGB_Strip_Fade(RGB_STRIP_GAMEIDLE_RED, RGB_STRIP_GAMEIDLE_GREEN, RGB_STRIP_GAMEIDLE_BLUE);

			if (RedTeamScore != WINNER_SCORE) {
				BallControl_ServeBall();
				BallControl_Cooldown = millis() + SCORE_COOLDOWN;
			} else { // RED WINS!
				winnerTimestamp = millis() + WINNER_SCREEN_TIMEOUT;
				beepTimestamp = millis() + SCORE_BEEP_INTERVAL;
				currentScoreDisplay = SCORE_WINNER_RED;
				RGB_Strip_WriteColorStrip(255, 0, 0, RGB_STRIP_LENGTH);
				RGB_Strip_WriteColorStrip_Bottom(255, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
				UserGUI_Show(USERGUI_SCORE); // Show winner screen!
				RGB_Strip_Mode = RGB_STRIP_GAME_WINNER_RED;
				for (i = 25; i > 0; i--) {
					Buzzer_Set(1);
					vTaskDelay(10+(5*i));
					Buzzer_Set(0);
					vTaskDelay(10+(2*i));
				}

				if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_FUN_1ON1 || selectedGameMode == GAME_FUN_NOREG) {
					GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
					API_FinalizeMatch("fm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				} else if (selectedGameMode == GAME_MATCH) {
					GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
					API_FinalizeMatch("gm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				} else {
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				}
			}
		}
	} else

	if (IRDetectors_GetFlag(RIGHT_GOAL) && (currentScoreDisplay == SCORE_GAME_RUNNING || currentScoreDisplay == SCORE_GAME_RUNNING_DARK)) { // Right = Blue
		interactionTimestamp = millis();

		if (BallControl_Cooldown != 0) { // score too fast
			vTaskDelay(500);
			IRDetectors_ClearFlag(RIGHT_GOAL);
		} else {
BlueScoreCall:
			BlueTeamScore++;
			UserGUI_Score_Update();
			RGB_Strip_Mode = RGB_STRIP_GAME_SCORE_BLUE;

			if (selectedGameMode != GAME_NONBLAST && BlueTeamScore != WINNER_SCORE) {
				API_SlotStatus = API_UpdateMatchGetStatus(API_ScoreSlot);
				if (API_SlotStatus == HTTP_SLOT_AVAILABLE || API_SlotStatus == HTTP_SLOT_FINISH_OK) { // Everything OK - just continue
					API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
				}
				else if (API_SlotStatus == HTTP_SLOT_PENDING) { // Old API call still waiting! :O
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
					while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
					API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
					GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				}
				else if (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_FINISH_ERR) { // Gammel API kald blev ikke gemt ordentligt, prøv det nye
					API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
					while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
					/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
					GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
					sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
					GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				}
			}

			vTaskDelay(500);
			IRDetectors_ClearFlag(RIGHT_GOAL);
			Accelerometer_TiltDetected = ACCELEROMETER_CLEAR; // clear a evt. faulty tilt
			RGB_Strip_Fade(RGB_STRIP_GAMEIDLE_RED, RGB_STRIP_GAMEIDLE_GREEN, RGB_STRIP_GAMEIDLE_BLUE);

			if (BlueTeamScore != WINNER_SCORE) {
				BallControl_ServeBall();
				BallControl_Cooldown = millis() + SCORE_COOLDOWN;
			} else { // BLUE WINS!
				winnerTimestamp = millis() + WINNER_SCREEN_TIMEOUT;
				beepTimestamp = millis() + SCORE_BEEP_INTERVAL;
				currentScoreDisplay = SCORE_WINNER_BLUE;
				RGB_Strip_WriteColorStrip(0, 0, 255, RGB_STRIP_LENGTH);
				RGB_Strip_WriteColorStrip_Bottom(0, 0, 255, RGB_STRIP_LENGTH_BOTTOM);
				UserGUI_Show(USERGUI_SCORE); // Show winner screen!
				RGB_Strip_Mode = RGB_STRIP_GAME_WINNER_BLUE;
				for (i = 25; i > 0; i--) {
					Buzzer_Set(1);
					vTaskDelay(10+(5*i));
					Buzzer_Set(0);
					vTaskDelay(10+(2*i));
				}

				if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_FUN_1ON1 || selectedGameMode == GAME_FUN_NOREG) {
					GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
					API_FinalizeMatch("fm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				} else if (selectedGameMode == GAME_MATCH) {
					GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
					API_FinalizeMatch("gm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				} else {
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
					GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
					GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
					GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				}
			}
		}
	}

	if (Accelerometer_TiltDetected == ACCELEROMETER_TILT_DETECTED_BLUE && (currentScoreDisplay == SCORE_GAME_RUNNING || currentScoreDisplay == SCORE_GAME_RUNNING_DARK) && millis() >= BallControl_Cooldown) {
		interactionTimestamp = millis();

		BlueTeamTilts++;

		if (selectedGameMode == GAME_MATCH) { // Game mode
			if (BlueTeamScore > -10)
				BlueTeamScore--;
		} else { // Fun mode
			if (BlueTeamScore > 0)
				BlueTeamScore--;
		}
		UserGUI_Score_Update();

		Buzzer_Set(1);
		vTaskDelay(1000);
		Buzzer_Set(0);

		if (selectedGameMode != GAME_NONBLAST && BlueTeamScore > -10) {
			API_SlotStatus = API_UpdateMatchGetStatus(API_ScoreSlot);
			if (API_SlotStatus == HTTP_SLOT_AVAILABLE || API_SlotStatus == HTTP_SLOT_FINISH_OK) { // Everything OK - just continue
				API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
			}
			else if (API_SlotStatus == HTTP_SLOT_PENDING) { // Old API call still waiting! :O
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
				API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
				GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
			}
			else if (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_FINISH_ERR) { // Gammel API kald blev ikke gemt ordentligt, prøv det nye
				API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
				GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
			}
		} else if (BlueTeamScore == -10) { // End the game
			winnerTimestamp = millis() + WINNER_SCREEN_TIMEOUT;
			beepTimestamp = millis() + SCORE_BEEP_INTERVAL;
			currentScoreDisplay = SCORE_WINNER_RED;
			RGB_Strip_WriteColorStrip(0, 0, 255, RGB_STRIP_LENGTH);
			RGB_Strip_WriteColorStrip_Bottom(0, 0, 255, RGB_STRIP_LENGTH_BOTTOM);
			UserGUI_Show(USERGUI_SCORE); // Show winner screen!
			RGB_Strip_Mode = RGB_STRIP_GAME_WINNER_RED;
			for (i = 25; i > 0; i--) {
				Buzzer_Set(1);
				vTaskDelay(10+(5*i));
				Buzzer_Set(0);
				vTaskDelay(10+(2*i));
			}

			if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_FUN_1ON1 || selectedGameMode == GAME_FUN_NOREG) {
				GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				API_FinalizeMatch("fm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			} else if (selectedGameMode == GAME_MATCH) {
				GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				API_FinalizeMatch("gm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			} else {
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			}
		}

		Accelerometer_TiltDetected = ACCELEROMETER_CLEAR;

	} else if (Accelerometer_TiltDetected == ACCELEROMETER_TILT_DETECTED_RED && (currentScoreDisplay == SCORE_GAME_RUNNING || currentScoreDisplay == SCORE_GAME_RUNNING_DARK) && millis() >= BallControl_Cooldown) {
		interactionTimestamp = millis();

		RedTeamTilts++;

		if (selectedGameMode == GAME_MATCH) { // Game mode
			if (RedTeamScore > -10)
				RedTeamScore--;
		} else { // Fun mode
			if (RedTeamScore > 0)
				RedTeamScore--;
		}
		UserGUI_Score_Update();

		Buzzer_Set(1);
		vTaskDelay(1000);
		Buzzer_Set(0);

		if (selectedGameMode != GAME_NONBLAST && RedTeamScore > -10) {
			API_SlotStatus = API_UpdateMatchGetStatus(API_ScoreSlot);
			if (API_SlotStatus == HTTP_SLOT_AVAILABLE || API_SlotStatus == HTTP_SLOT_FINISH_OK) { // Everything OK - just continue
				API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
			}
			else if (API_SlotStatus == HTTP_SLOT_PENDING) { // Old API call still waiting! :O
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
				API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
				GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
			}
			else if (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_FINISH_ERR) { // Gammel API kald blev ikke gemt ordentligt, prøv det nye
				API_ScoreSlot = API_UpdateMatchResult(MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts);
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 165, 459, 165, "Vent venligst...", Arial34R, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
				while (API_UpdateMatchGetStatus(API_ScoreSlot) == HTTP_SLOT_PENDING) vTaskDelay(250);
				/*GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_TEAM_BLUE, 1);
				GUI_Rectangle(21, 131, 459, 331, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 130, 460, 330, GUI_COLOR_BORDER, 0);
				sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
				GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);*/
			}
		} else if (RedTeamScore == -10) { // End the game
			winnerTimestamp = millis() + WINNER_SCREEN_TIMEOUT;
			beepTimestamp = millis() + SCORE_BEEP_INTERVAL;
			currentScoreDisplay = SCORE_WINNER_BLUE;
			RGB_Strip_WriteColorStrip(0, 0, 255, RGB_STRIP_LENGTH);
			RGB_Strip_WriteColorStrip_Bottom(0, 0, 255, RGB_STRIP_LENGTH_BOTTOM);
			UserGUI_Show(USERGUI_SCORE); // Show winner screen!
			RGB_Strip_Mode = RGB_STRIP_GAME_WINNER_BLUE;
			for (i = 25; i > 0; i--) {
				Buzzer_Set(1);
				vTaskDelay(10+(5*i));
				Buzzer_Set(0);
				vTaskDelay(10+(2*i));
			}

			if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_FUN_1ON1 || selectedGameMode == GAME_FUN_NOREG) {
				GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				API_FinalizeMatch("fm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			} else if (selectedGameMode == GAME_MATCH) {
				GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
				API_FinalizeMatch("gm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			} else {
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			}
		}

		Accelerometer_TiltDetected = ACCELEROMETER_CLEAR;

	} else if (((Accelerometer_TiltDetected == ACCELEROMETER_TILT_DETECTED_BLUE) || (Accelerometer_TiltDetected == ACCELEROMETER_TILT_DETECTED_RED)) && (currentScoreDisplay == SCORE_GAME_RUNNING || currentScoreDisplay == SCORE_GAME_RUNNING_DARK) && millis() < BallControl_Cooldown) {
		Accelerometer_TiltDetected = ACCELEROMETER_CLEAR;
	}

	if (millis() > (interactionTimestamp+MATCH_TIMEOUT)) { // no interaction for long time - exit match
		if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_FUN_1ON1 || selectedGameMode == GAME_FUN_NOREG) {
			GUI_Text_Center(21, 715, 459, 715, "Gemmer resultat...", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
			API_FinalizeMatch("fm", MatchID, RedTeamScore, BlueTeamScore, RedTeamTilts, BlueTeamTilts, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
		} else {
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
			GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 715, 459, 715, "Tilbage", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);
		}

		GUI_SetDisplay(LCD_BOTH);
		if (selectedGameMode == GAME_NONBLAST) {
			UserGUI_Show(USERGUI_NONBLAST_MAIN);
		} else {
			UserGUI_Show(USERGUI_MAIN);
		}
	}

	if (MatchID == 0 && NoRegSlotID > 0 && selectedGameMode == GAME_FUN_NOREG) {
		if (HTTP_Asynchronous_Feedback[NoRegSlotID] != HTTP_SLOT_FINISH_OK && HTTP_Asynchronous_Feedback[NoRegSlotID] != HTTP_SLOT_PENDING) { // API call was not OK - try again
			MatchID = 0; // no reg match
			NoRegSlotID = 0;
			NoRegSlotID = API_RegisterFunMatchNoReg(NoRegSlotBuffer);
		} else if (HTTP_Asynchronous_Feedback[NoRegSlotID] == HTTP_SLOT_FINISH_OK) {
			printf("Return from HTTP: %s\n", NoRegSlotBuffer);
			if (strstr(NoRegSlotBuffer, "fejl") == 0) {
				MatchID = atoi(NoRegSlotBuffer);
				if (MatchID != 0) {
					return 1;
				} else {
					return 0;
				}
			} else {
				MatchID = 0; // try again
				NoRegSlotID = 0;
				NoRegSlotID = API_RegisterFunMatchNoReg(NoRegSlotBuffer);
			}
		}
	}

	currentTouchedLCD = 0;
}


void UserGUI_Score_Update(void)
{
	sprintf(stringBuffer, "\x3B%d\x3B", BlueTeamScore);
	GUI_Text_Center(21, 165, 459, 165, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_BLUE);
	sprintf(stringBuffer, "\x3B%d\x3B", RedTeamScore);
	GUI_Text_Center(21, 460, 459, 460, stringBuffer, Hobo110F, GUI_COLOR_TEXT_WHITE, GUI_COLOR_TEAM_RED);
}



/*void API_UpdateMatchResult(uint32_t matchID, uint8_t redScore, uint8_t blueScore, uint8_t redTilt, uint8_t blueTilt)
{
	uint8_t i;
	uint8_t success;
	uint8_t returnBuffer[100];
	uint8_t * namePointer;

	for (i = 0; i < 100; i++) returnBuffer[i] = 0;

	sprintf(stringBuffer, "theblast.php?get=update/%d/%d|%d/%d|%d", matchID, redScore, blueScore, redTilt, blueTilt);
	success = TheBlast_Get_Synchronous(stringBuffer, 50, returnBuffer);

	printf("Return from HTTP: %s\n", returnBuffer);

	if (success) {
		if (strstr(returnBuffer, "success") != 0) {
			return 1;
		} else {
			return 0; // some error message to parse
		}
	} else {
		return 0; // API call was not OK
	}
}*/

uint8_t API_UpdateMatchResult(uint32_t matchID, int8_t redScore, int8_t blueScore, uint8_t redTilt, uint8_t blueTilt)
{
	uint8_t i;
	uint8_t slot;
	uint8_t * namePointer;

	for (i = 0; i < 50; i++) API_Score_ReturnBuffer[i] = 0;

	sprintf(stringBuffer, "theblast.php?get=update/%d/%d|%d/%d|%d", matchID, blueScore, redScore, blueTilt, redTilt);
	slot = TheBlast_Get_Asynchronous(stringBuffer, 50, API_Score_ReturnBuffer);
	return slot;
}

uint8_t API_UpdateMatchGetStatus(uint8_t slot) {
	if (HTTP_Asynchronous_Feedback[slot] == HTTP_SLOT_PENDING) {
		return HTTP_SLOT_PENDING;
	} else if (HTTP_Asynchronous_Feedback[slot] == HTTP_SLOT_AVAILABLE) {
		return HTTP_SLOT_AVAILABLE;
	} else if (HTTP_Asynchronous_Feedback[slot] == HTTP_SLOT_FINISH_OK) {
		printf("Return from HTTP: %s\n", API_Score_ReturnBuffer);

		if (strstr(API_Score_ReturnBuffer, "success") != 0) {
			HTTP_Asynchronous_Feedback[slot] = HTTP_SLOT_AVAILABLE;
			return HTTP_SLOT_FINISH_OK;
		} else {
			HTTP_Asynchronous_Feedback[slot] = HTTP_SLOT_AVAILABLE;
			return HTTP_SLOT_FINISH_ERR; // some error message to parse
		}
	} else {
		HTTP_Asynchronous_Feedback[slot] = HTTP_SLOT_AVAILABLE;
		return HTTP_SLOT_FINISH_ERR;
	}
}

void API_FinalizeMatch(uint8_t * gameMode, uint32_t matchID, int8_t redScore, int8_t blueScore, uint8_t redTilt, uint8_t blueTilt, uint8_t manualFinalize)
{
	uint8_t i;
	uint8_t slot;
	uint8_t returnBuffer[100];
	uint8_t * namePointer;

	if (matchID == 0) return 0; // can't finalize a non-existing match!

	for (i = 0; i < 100; i++) returnBuffer[i] = 0;

	if (manualFinalize) {
		sprintf(stringBuffer, "theblast.php?get=end/%s/%d/%d|%d/%d|%d/u", gameMode, matchID, blueScore, redScore, blueTilt, redTilt);
	} else {
		sprintf(stringBuffer, "theblast.php?get=end/%s/%d/%d|%d/%d|%d/s", gameMode, matchID, blueScore, redScore, blueTilt, redTilt);
	}
	slot = TheBlast_Get_Asynchronous(stringBuffer, 50, returnBuffer);

	while (HTTP_Asynchronous_Feedback[slot] == HTTP_SLOT_PENDING) vTaskDelay(500);
	if (HTTP_Asynchronous_Feedback[slot] != HTTP_SLOT_FINISH_OK) return 0;  // API call was not OK

	printf("Return from HTTP: %s\n", returnBuffer);

	if (strstr(returnBuffer, "success") != 0) {
		return 1;
	} else {
		return 0; // some error message to parse
	}
}
