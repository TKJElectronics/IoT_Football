#ifndef __USERGUI_SCORE_H
#define __USERGUI_SCORE_H

#include "UserGUI.h"

void UserGUI_Score_Init(void);
void UserGUI_Score_Show(void);
void UserGUI_Score_Task(void);

extern int8_t BlueTeamScore;
extern uint8_t BlueTeamTilts;
extern int8_t RedTeamScore;
extern uint8_t RedTeamTilts;

extern uint8_t gameServeFirst;
extern uint8_t currentScoreDisplay;

#define SCORE_GAME_RUNNING		0
#define SCORE_GAME_RUNNING_DARK	1
#define SCORE_WINNER_BLUE		2
#define SCORE_WINNER_RED		3

#define SCORE_BEEP_INTERVAL	1000	// 3000 ms beep
#define WINNER_SCREEN_TIMEOUT  15000 // show winner screen in 10 seconds

void UserGUI_Score_Update(void);
uint8_t API_UpdateMatchResult(uint32_t matchID, int8_t redScore, int8_t blueScore, uint8_t redTilt, uint8_t blueTilt);
uint8_t API_UpdateMatchGetStatus(uint8_t slot);
void API_FinalizeMatch(uint8_t * gameMode, uint32_t matchID, int8_t redScore, int8_t blueScore, uint8_t redTilt, uint8_t blueTilt, uint8_t manualFinalize);

#endif /* __USERGUI_SCORE_H */
