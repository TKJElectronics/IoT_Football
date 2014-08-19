#include "Player_Register.h"


uint8_t Player_Menu_State = 0;

uint8_t registerStep;
uint8_t readBarcodeLength = 0;
uint8_t gameCountDown = 0;
uint32_t gameCountDownTimer = 0;
uint8_t loopCheckI;

uint8_t tempName1[26], tempName2[26];


void UserGUI_Player_Register_Show(void)
{
	UserGUI_DisplayLock(currentLCDinCharge);

	GUI_SetDisplay(currentLCDinCharge);
	GUI_Clear(GUI_COLOR_BACKGROUND);
	GUI_DisplayRAW(368, 10, theblast);
	GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	GUI_Text(20, 110, "Spiller registrering", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

	registerStep = 0;
	UserGUI_Player_Register_UpdateDisplay();

	GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
	GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
	GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
	GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	BarcodeScanner_InitiateScan();
	RGB_Strip_Mode = RGB_STRIP_RAINBOW_FADE;
}

void UserGUI_Player_Register_Task(void)
{
	if (currentTouchedLCD) // LCD is touched
	{
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

			BarcodeScanner_StopScan(); // Stop any ongoing scans
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Show(USERGUI_MAIN);
			return;
		}
	}


	/* We need to register/scan users */
	if (((selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_MATCH) && (registerStep < 4)) || ((selectedGameMode == GAME_FUN_1ON1) && (registerStep < 2)))
	{
		readBarcodeLength = BarcodeScanner_GetScan();
		if (readBarcodeLength > 0)
		{
			if (selectedGameMode == GAME_FUN_1ON1 && registerStep == 0) { /* Check for Admin card scan */
				if (strcmp(BarcodeScanner_Buffer, PROGRAMMER_CARD) == 0) {
					GUI_SetDisplay(LCD_BOTH);
					UserGUI_Show(USERGUI_ADMIN);
					return;
				}
			}

			if (registerStep > 0) { // Tjek barcodes
				for (loopCheckI = 0; loopCheckI < registerStep; loopCheckI++) {
					if (strcmp(BarcodeScanner_Buffer, PlayerBarcodes[loopCheckI]) == 0) {
						GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1);
						GUI_Text_Center(21, 450, 459, 450, "Allerede registreret", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
						GUI_Text_Center(21, 520, 459, 520, "Spilleren er allerede registreret.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

						Buzzer_Set(1);
						vTaskDelay(500); // warn the user for error
						Buzzer_Set(0);
						vTaskDelay(500);

						BarcodeScanner_InitiateScan(); // Initiate another scan as we still miss users!
						currentTouchedLCD = 0;
						return;
					}
				}
			}

			GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1);
			GUI_Text_Center(21, 450, 459, 450, "Tjekker spiller", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
			GUI_Text_Center(21, 520, 459, 520, "Vent venligst...", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
			GUI_Rectangle(20, 710, 460, 766, GUI_COLOR_BACKGROUND, 1); // Fjern Afbryd knap

			if (!API_ConfirmAndSavePlayer(BarcodeScanner_Buffer, registerStep)) {
				GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1);
				GUI_Text_Center(21, 450, 459, 450, "Spiller ikke fundet", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
				GUI_Text_Center(21, 520, 459, 520, "Kontakt den turneringsansvarlige.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

				// Vis afbryd knap igen
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

				Buzzer_Set(1);
				vTaskDelay(500); // warn the user for error
				Buzzer_Set(0);

				BarcodeScanner_InitiateScan(); // Initiate another scan as we still miss users!
				currentTouchedLCD = 0;
				return;
			}

			if (registerStep > 0) { // Tjek ID's
				for (loopCheckI = 0; loopCheckI < registerStep; loopCheckI++) {
					if (strcmp(BarcodeScanner_Buffer, PlayerIDs[loopCheckI]) == 0) {
						GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1);
						GUI_Text_Center(21, 450, 459, 450, "Allerede registreret", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
						GUI_Text_Center(21, 520, 459, 520, "Spilleren er allerede registreret.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

						// Vis afbryd knap igen
						GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
						GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
						GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
						GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

						Buzzer_Set(1);
						vTaskDelay(500); // warn the user for error
						Buzzer_Set(0);

						BarcodeScanner_InitiateScan(); // Initiate another scan as we still miss users!
						currentTouchedLCD = 0;
						return;
					}
				}
			}


			/* Indicate to user that scan was successfull */
			Buzzer_Set(1);
			vTaskDelay(50);
			Buzzer_Set(0);
			vTaskDelay(50);
			Buzzer_Set(1);
			vTaskDelay(50);
			Buzzer_Set(0);

			if (selectedGameMode == GAME_MATCH && registerStep == 3) { // Last user scanned
				GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1);
				GUI_Text_Center(21, 450, 459, 450, "Finder kamp", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
				GUI_Text_Center(21, 520, 459, 520, "Vent venligst...", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
				GUI_Rectangle(20, 710, 460, 766, GUI_COLOR_BACKGROUND, 1); // Fjern Afbryd knap

				if (API_RegisterGameMatch()) {
					registerStep = 5; // Register OK - start the game!
				} else {
					registerStep = 4; // Register not OK
				}
				UserGUI_Player_Register_UpdateDisplay();
				currentTouchedLCD = 0;
				return;
			} else if ((selectedGameMode == GAME_FUN_2ON2  && registerStep == 3) || (selectedGameMode == GAME_FUN_1ON1 && registerStep == 1)) {// Last user scanned
				GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1);
				GUI_Text_Center(21, 450, 459, 450, "Opretter kamp", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
				GUI_Text_Center(21, 520, 459, 520, "Vent venligst...", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
				GUI_Rectangle(20, 710, 460, 766, GUI_COLOR_BACKGROUND, 1); // Fjern Afbryd knap

				if (API_RegisterFunMatch()) {
					registerStep = 5; // Register OK - start the game!
				} else {
					registerStep = 4; // Register not OK
				}
				UserGUI_Player_Register_UpdateDisplay();
				currentTouchedLCD = 0;
				return;
			} else {
				GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1);
				strncpy(tempName1, PlayerNames[registerStep], 25);
				GUI_Text_Center(21, 450, 459, 450, tempName1, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);
				GUI_Text_Center(21, 520, 459, 520, "Scan venligst den n\x83ste spiller.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

				// Vis afbryd knap igen
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
				GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
				GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

				registerStep++;
				UserGUI_Player_Register_UpdateDisplay();
				BarcodeScanner_InitiateScan(); // Initiate another scan as we still miss users!
				currentTouchedLCD = 0;
				return;
			}
		}
	}

	if (registerStep == 5 && (millis() > (gameCountDownTimer+1000))) {
		gameCountDownTimer = millis();
		Buzzer_Set(1);
		if (gameCountDown > 1) {
			vTaskDelay(50); // Delay for the buzzer to sound
			Buzzer_Set(0);
		}

		gameCountDown--;

		if (gameCountDown > 0) {
			if (gameCountDown == 3) { // First time displaying - clear the box
				GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_OK_GREEN, 1);
				GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
				GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
			}
			sprintf(stringBuffer, "\x3B%d\x3B", gameCountDown);
			//GUI_Rectangle(50, 300, 400, 350, GUI_COLOR_OK_GREEN, 1);
			GUI_Text_Center(21, 220, 459, 220, stringBuffer, Hobo110F, GUI_COLOR_TEXT, GUI_COLOR_OK_GREEN);
		} else {
			GUI_SetDisplay(LCD_BOTH);
			UserGUI_Score_Init();
			UserGUI_Show(USERGUI_SCORE); // Start the game
			return;
		}
	}

	currentTouchedLCD = 0;
}


void UserGUI_Player_Register_UpdateDisplay(void)
{
	if ((selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_MATCH) && (registerStep < 4)) {
		if (registerStep == 0) {
			GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_ORANGE, 1);
			GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 320, 459, 320, "Benyt dit ID-kort til at registrere dig.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_ORANGE);
		}

		if (registerStep < 2) { // blå
			GUI_Text_Center(21, 200, 459, 200, "\x86Hold: Bl\x85\x86", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_ORANGE);
		} else {// rød
			GUI_Text_Center(21, 200, 459, 200, "\x86Hold: R\x84\x64\x86", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_ORANGE);
		}

		sprintf(stringBuffer, "\x86Spiller: %d\x86", ((registerStep%2)+1));
		GUI_Text_Center(21, 250, 459, 250, stringBuffer, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_ORANGE);
	} else if ((selectedGameMode == GAME_FUN_1ON1) && (registerStep < 2)) {
		if (registerStep == 0) {
			GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_ORANGE, 1);
			GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
			GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
			GUI_Text_Center(21, 310, 459, 310, "Benyt dit ID-kort til at registrere dig.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_ORANGE);
		}

		if (registerStep == 0) { // blå
			GUI_Text_Center(21, 225, 459, 225, "\x86Spiller: Bl\x85\x86", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_ORANGE);
		} else {
			GUI_Text_Center(21, 225, 459, 225, "\x86Spiller: R\x84\x64\x86", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_ORANGE);
		}
	} else if (registerStep == 4) {
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_ERROR_RED, 1);
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 200, 459, 200, "FEJL!", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_ERROR_RED);
		GUI_Text_Center(21, 250, 459, 250, "Der blev ikke fundet en kamp mellem", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_ERROR_RED);
		GUI_Text_Center(21, 290, 459, 290, "de to hold.", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_ERROR_RED);
		GUI_Text_Center(21, 330, 459, 330, "Kontakt den turneringsansvarlige!", Arial18R, GUI_COLOR_TEXT, GUI_COLOR_ERROR_RED);
		GUI_Rectangle(21, 450, 459, 580, GUI_COLOR_BACKGROUND, 1); // Fjern status besked nederst
		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

	} else if (registerStep == 5) {
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_OK_GREEN, 1);
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 230, 459, 230, "GODKENDT!", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_OK_GREEN);
		GUI_Text_Center(21, 300, 459, 300, "Spillet starter om", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_OK_GREEN);

		UserGUI_Player_Register_RemoveLock(currentLCDinCharge);

		GUI_SetDisplay(LCD2); // blåt hold
		GUI_Rectangle(21, 431, 459, 631, GUI_COLOR_TEAM_BLUE, 1);
		GUI_Rectangle(21, 431, 459, 631, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 430, 460, 630, GUI_COLOR_BORDER, 0);

		if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_MATCH) {
			if (strlen(PlayerNames[0]) > 19) {
				strncpy(tempName1, PlayerNames[0], 19);
				tempName1[19] = 0x00; // null terminate
			} else {
				strcpy(tempName1, PlayerNames[0]);
			}
			if (strlen(PlayerNames[1]) > 19) {
				strncpy(tempName2, PlayerNames[1], 19);
				tempName2[19] = 0x00; // null terminate
			} else {
				strcpy(tempName2, PlayerNames[1]);
			}
			GUI_Text_Center(21, 470, 459, 470, tempName1, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_TEAM_BLUE);
			GUI_Text_Center(21, 550, 459, 550, tempName2, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_TEAM_BLUE);
		} else if (selectedGameMode == GAME_FUN_1ON1) {
			if (strlen(PlayerNames[0]) > 19) {
				strncpy(tempName1, PlayerNames[0], 19);
				tempName1[19] = 0x00; // null terminate
			} else {
				strcpy(tempName1, PlayerNames[0]);
			}
			GUI_Text_Center(21, 530, 459, 450, tempName1, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_TEAM_BLUE);
		}

		GUI_SetDisplay(LCD1); // rødt hold
		GUI_Rectangle(21, 431, 459, 631, GUI_COLOR_TEAM_RED, 1);
		GUI_Rectangle(21, 431, 459, 631, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 430, 460, 630, GUI_COLOR_BORDER, 0);

		if (selectedGameMode == GAME_FUN_2ON2 || selectedGameMode == GAME_MATCH) {
			if (strlen(PlayerNames[2]) > 19) {
				strncpy(tempName1, PlayerNames[2], 19);
				tempName1[19] = 0x00; // null terminate
			} else {
				strcpy(tempName1, PlayerNames[2]);
			}
			if (strlen(PlayerNames[3]) > 19) {
				strncpy(tempName2, PlayerNames[3], 19);
				tempName2[19] = 0x00; // null terminate
			} else {
				strcpy(tempName2, PlayerNames[3]);
			}
			GUI_Text_Center(21, 470, 459, 470, tempName1, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_TEAM_RED);
			GUI_Text_Center(21, 550, 459, 550, tempName2, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_TEAM_RED);
		} else if (selectedGameMode == GAME_FUN_1ON1) {
			if (strlen(PlayerNames[1]) > 19) {
				strncpy(tempName1, PlayerNames[1], 19);
				tempName1[19] = 0x00; // null terminate
			} else {
				strcpy(tempName1, PlayerNames[1]);
			}
			GUI_Text_Center(21, 530, 459, 450, tempName1, Arial34R, GUI_COLOR_TEXT, GUI_COLOR_TEAM_RED);
		}

		GUI_SetDisplay(LCD_BOTH);

		gameCountDown = 4; // first time we substract 1
		gameCountDownTimer = millis() - 700; // start in 300 msecs
	}
}


uint8_t API_RegisterFunMatch(void)
{
	uint8_t i;
	uint8_t success;
	uint8_t returnBuffer[50];

	for (i = 0; i < 50; i++) returnBuffer[i] = 0;

	if (selectedGameMode == GAME_FUN_1ON1) {
		sprintf(stringBuffer, "theblast.php?get=start/fm/%s|%s", PlayerIDs[0], PlayerIDs[1]);
		success = TheBlast_Get_Synchronous(stringBuffer, 50, returnBuffer); // 1 on 1
	} else if (selectedGameMode == GAME_FUN_2ON2) {
		sprintf(stringBuffer, "theblast.php?get=start/fm/%s|%s|%s|%s", PlayerIDs[0], PlayerIDs[1], PlayerIDs[2], PlayerIDs[3]);
		success = TheBlast_Get_Synchronous(stringBuffer, 50, returnBuffer); // 2 on 2
	/*} else if (selectedGameMode == GAME_FUN_NOREG) {
		sprintf(stringBuffer, "theblast.php?get=start/fm/0|0");
		success = TheBlast_Get_Synchronous(stringBuffer, 50, returnBuffer); // Fun mode No Reg*/
	}

	printf("Return from HTTP: %s\n", returnBuffer);

	if (success) {
		if (strstr(returnBuffer, "fejl") == 0) {
			MatchID = atoi(returnBuffer);
			if (MatchID != 0) {
				return 1;
			} else {
				return 0;
			}
		} else {
			return 0; // user was not found
		}
	} else {
		return 0;
	}
}

uint8_t API_RegisterFunMatchNoReg(uint8_t * returnBufferPointer)
{
	uint8_t i;
	uint8_t slot;
	uint8_t returnBuffer[100];
	uint8_t * namePointer;

	for (i = 0; i < 100; i++) returnBuffer[i] = 0;

	sprintf(stringBuffer, "theblast.php?get=start/fm/0|0");
	slot = TheBlast_Get_Asynchronous(stringBuffer, 50, returnBufferPointer);

	return slot;
}

uint8_t API_RegisterGameMatch(void)
{
	uint8_t i;
	uint8_t success;
	uint8_t returnBuffer[50];

	for (i = 0; i < 50; i++) returnBuffer[i] = 0;

	sprintf(stringBuffer, "theblast.php?get=start/gm/%s|%s|%s|%s", PlayerIDs[0], PlayerIDs[1], PlayerIDs[2], PlayerIDs[3]);
	success = TheBlast_Get_Synchronous(stringBuffer, 50, returnBuffer); // 2 on 2

	printf("Return from HTTP: %s\n", returnBuffer);

	if (success) {
		if (strstr(returnBuffer, "fejl") == 0) {
			MatchID = atoi(returnBuffer);
			if (MatchID != 0) {
				return 1;
			} else {
				return 0;
			}
		} else {
			return 0; // user was not found
		}
	} else {
		return 0;
	}
}


uint8_t API_ConfirmAndSavePlayer(uint8_t * barcode, uint8_t playerNumber)
{
	uint8_t i;
	uint8_t success;
	uint8_t returnBuffer[100];
	uint8_t * namePointer;

	for (i = 0; i < 100; i++) returnBuffer[i] = 0;

	sprintf(stringBuffer, "theblast.php?get=user/%s", barcode);
	success = TheBlast_Get_Synchronous(stringBuffer, 50, returnBuffer);

	printf("Return from HTTP: %s\n", returnBuffer);

	if (success) {
		if (strstr(returnBuffer, "fejl") == 0) {
			namePointer = strstr(returnBuffer, "|");
			if (namePointer > 0) {
				strncpy(PlayerIDs[playerNumber], returnBuffer, (namePointer-returnBuffer));
				PlayerIDs[playerNumber][(namePointer-returnBuffer)] = 0;
				namePointer++;
				strncpy(PlayerNames[playerNumber], namePointer, 50);
				for (i = 0; i < strlen(PlayerNames[playerNumber]); i++)
				{
					if (PlayerNames[playerNumber][i] == 'Æ') PlayerNames[playerNumber][i] = 0x80;
					else if (PlayerNames[playerNumber][i] == 'Ø') PlayerNames[playerNumber][i] = 0x81;
					else if (PlayerNames[playerNumber][i] == 'Å') PlayerNames[playerNumber][i] = 0x82;
					else if (PlayerNames[playerNumber][i] == 'æ') PlayerNames[playerNumber][i] = 0x83;
					else if (PlayerNames[playerNumber][i] == 'ø') PlayerNames[playerNumber][i] = 0x84;
					else if (PlayerNames[playerNumber][i] == 'å') PlayerNames[playerNumber][i] = 0x85;
				}

				strncpy(PlayerBarcodes[playerNumber], barcode, 50);
				return 1;
			} else {
				return 0; // return not as expected
			}
			//strncpy(PlayerBarcodes[playerNumber], barcode, 50); // discard the barcode
		} else {
			return 0; // user was not found
		}
	} else {
		return 0;
	}
}

void UserGUI_Player_Register_RemoveLock(LCD_Control LCDInChage)
{
		/*LCD_Control oldLCDControl = 0;
		if (LCD1_Current_CS_Pin != 0) oldLCDControl |= LCD1;
		if (LCD2_Current_CS_Pin != 0) oldLCDControl |= LCD2;*/

		LCD_Control LCDtoLock;
		if (LCDInChage == 0 || LCDInChage == LCD_BOTH) return; // not possible to lock none or both!
		else if (LCDInChage == LCD1) LCDtoLock = LCD2;
		else if (LCDInChage == LCD2) LCDtoLock = LCD1;

		GUI_SetDisplay(LCDtoLock);

		GUI_FadeBacklight(FADE_DOWN);

		GUI_Clear(GUI_COLOR_BACKGROUND);
		GUI_DisplayRAW(368, 10, theblast);
		GUI_Text(20, 35, "Bordfodbold", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

		GUI_Text(20, 110, "Spiller registrering", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_BACKGROUND);

		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_OK_GREEN, 1);
		GUI_Rectangle(21, 181, 459, 381, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 180, 460, 380, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 230, 459, 230, "GODKENDT!", Arial34R, GUI_COLOR_TEXT, GUI_COLOR_OK_GREEN);
		GUI_Text_Center(21, 300, 459, 300, "Spillet starter om", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_OK_GREEN);

		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BUTTON, 1);
		GUI_Rectangle(21, 711, 459, 766, GUI_COLOR_BORDER, 0);
		GUI_Rectangle(20, 710, 460, 765, GUI_COLOR_BORDER, 0);
		GUI_Text_Center(21, 715, 459, 715, "Afbryd", Arial28R, GUI_COLOR_TEXT, GUI_COLOR_BUTTON);

		GUI_FadeBacklight(FADE_UP);

		GUI_SetDisplay(LCD_BOTH); // restore LCD control state to both displays
	}
