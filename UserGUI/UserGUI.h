#ifndef __USERGUI_H
#define __USERGUI_H

#include "stm32f4xx.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "main.h"
#include <stdio.h>

#include "netconf.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"

#include "http_client.h"
#include "Debugging.h"
#include "RGB_Strip.h"
#include "Accelerometer.h"
#include "SSD1963.h"
#include "BarcodeScanner.h"
#include "BallControl.h"

#include "sdio_sd.h"
#include "ff.h"


#define GUI_COLOR_TEXT			RGB565CONVERT(73,   69,     60)
#define GUI_COLOR_TEXT_WHITE	0xFFFF //RGB565CONVERT(255,    255,      255)
#define GUI_COLOR_BUTTON		RGB565CONVERT(177,  177,    177)
#define GUI_COLOR_BORDER		RGB565CONVERT(73,   69,     60)
#define GUI_COLOR_BACKGROUND	0xFFFF //RGB565CONVERT(255,    255,      255)
#define GUI_COLOR_ERROR_RED		RGB565CONVERT(255,   72,     72) // RGB565CONVERT(255,    0,      0)
#define GUI_COLOR_OK_GREEN		RGB565CONVERT(80,   237,     85) // RGB565CONVERT(255,    0,      0)

#define GUI_COLOR_DARK_BACKGROUND	0x41E6

#define GUI_COLOR_TEAM_BLUE			RGB565CONVERT(0,   128,     191)
#define GUI_COLOR_TEAM_RED			RGB565CONVERT(255,   72,     72)
#define GUI_COLOR_ORANGE			RGB565CONVERT(234,   158,     62)

//#define ADMIN_SMS_NUMBER	"29420778"
#define ADMIN_USER_ID	"1560" // THOMAS
#define GAMEMASTER_CARD	"75376787"   // "43405638"   Thomas Jespersen, The Blast kort
#define PROGRAMMER_CARD	"43405638" // THOMAS
#define EASTEREGG_FORCEDSCORE_TIMEOUT  5000 // 5 seconds scan timeout

#define	WINNER_SCORE			10
#define	SCORE_COOLDOWN			2000 // 2 seconds
#define MATCH_TIMEOUT			(8*60000) // 8 minutes, if no score, exit match

#define	API_PING_ERROR_LEVEL	1	// 3 times ping error and system should WARN!
#define API_PING_TIMEOUT		(5*60000) // 5 minutes
#define API_PING_TIMEOUT_RETRY	2000 // 5 seconds



typedef enum {
	USERGUI_MAIN,
	USERGUI_NONBLAST_MAIN,
	USERGUI_FUN_SELECT,
	USERGUI_PLAYER_REGISTER,
	USERGUI_SCORE,
	USERGUI_ADMIN,
	USERGUI_REBOOT_MATCH,
	USERGUI_API_ERROR
} USERGUI_PAGE;

typedef enum {
	GAME_NONE,
	GAME_FUN_NOREG,
	GAME_FUN_1ON1,
	GAME_FUN_2ON2,
	GAME_MATCH, // 2on2 always
	GAME_NONBLAST
} USERGUI_GAMEMODE;


extern USERGUI_PAGE CurrentPage;

extern uint16_t TouchX, TouchY;
extern LCD_Control currentTouchedLCD;
extern LCD_Control currentLCDinCharge;

extern USERGUI_GAMEMODE selectedGameMode;

extern uint8_t stringBuffer[200];
extern uint8_t PlayerBarcodes[4][50];
extern uint8_t PlayerIDs[4][50];
extern uint8_t PlayerNames[4][50];
extern uint32_t MatchID;
extern uint8_t NoRegSlotID;
extern uint8_t NoRegSlotBuffer[200];

extern uint8_t API_Network_Error;
extern char waitForAPIrequest;

void UserGUI_Task(void * pvParameters);
void UserGUI_Show(USERGUI_PAGE newMenu);
void UserGUI_NonBlast_Task(void * pvParameters);

void UserGUI_TouchTask(void * pvParameters);

void UserGUI_DisplayLock(LCD_Control LCDInChage);
void UserGUI_API_Error_Show(void);


#endif /* __USERGUI_H */
