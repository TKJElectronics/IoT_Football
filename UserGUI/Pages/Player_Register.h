#ifndef __USERGUI_PLAYER_REGISTER_H
#define __USERGUI_PLAYER_REGISTER_H

#include "UserGUI.h"

void UserGUI_Player_Register_Show(void);
void UserGUI_Player_Register_Task(void);

uint8_t API_RegisterFunMatch(void);
uint8_t API_RegisterFunMatchNoReg(uint8_t * returnBufferPointer);
uint8_t API_RegisterGameMatch(void);
uint8_t API_ConfirmAndSavePlayer(uint8_t * barcode, uint8_t playerNumber);

void UserGUI_Player_Register_RemoveLock(LCD_Control LCDInChage);

#endif /* __USERGUI_PLAYER_REGISTER_H */
