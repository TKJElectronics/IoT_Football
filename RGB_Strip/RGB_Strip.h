#ifndef __RGB_STRIP_H
#define __RGB_STRIP_H

#include "stm32f4xx.h"


#define RGB_Strip_Init() RGB_Strip_Init_GPIO()
#define RGB_Strip_WriteColorStrip(red, green, blue, led) RGB_Strip_WriteColorStrip_IO(red, green, blue, led)
#define RGB_Strip_WriteColorStrip_Bottom(red, green, blue, led) RGB_Strip_WriteColorStrip_Bottom_IO(red, green, blue, led)
#define RGB_Strip_Update() RGB_Strip_Update_IO()
#define RGB_Strip_Update_Bottom() RGB_Strip_Update_Bottom_IO()


/* RGB Strip CLOCK = PG.12 (D8) */
#define RGB_STRIP_CLK_PIN      		GPIO_Pin_12
#define RGB_STRIP_CLK_GPIO_PORT     GPIOG
#define RGB_STRIP_CLK_GPIO_CLK      RCC_AHB1Periph_GPIOG
#define RGB_STRIP_CLK_ON			GPIO_SetBits(RGB_STRIP_CLK_GPIO_PORT, RGB_STRIP_CLK_PIN);
#define RGB_STRIP_CLK_OFF			GPIO_ResetBits(RGB_STRIP_CLK_GPIO_PORT, RGB_STRIP_CLK_PIN);
#define RGB_STRIP_CLK_TOGGLE		GPIO_ToggleBits(RGB_STRIP_CLK_GPIO_PORT, RGB_STRIP_CLK_PIN);

/* RGB Strip DATA = PG.15 (D9) */
#define RGB_STRIP_DATA_PIN      	GPIO_Pin_15
#define RGB_STRIP_DATA_GPIO_PORT    GPIOG
#define RGB_STRIP_DATA_GPIO_CLK     RCC_AHB1Periph_GPIOG
#define RGB_STRIP_DATA_ON			GPIO_SetBits(RGB_STRIP_DATA_GPIO_PORT, RGB_STRIP_DATA_PIN); // FAST: GPIO->BSRRL = GPIO_Pin_15
#define RGB_STRIP_DATA_OFF			GPIO_ResetBits(RGB_STRIP_DATA_GPIO_PORT, RGB_STRIP_DATA_PIN); // FAST: GPIO->BSRRH = GPIO_Pin_15
#define RGB_STRIP_DATA_TOGGLE		GPIO_ToggleBits(RGB_STRIP_DATA_GPIO_PORT, RGB_STRIP_DATA_PIN);


/* RGB Strip bottom CLOCK = PG.6 */
#define RGB_STRIP_BOTTOM_CLK_PIN      		GPIO_Pin_6
#define RGB_STRIP_BOTTOM_CLK_GPIO_PORT    	GPIOG
#define RGB_STRIP_BOTTOM_CLK_GPIO_CLK    	RCC_AHB1Periph_GPIOG
#define RGB_STRIP_BOTTOM_CLK_ON				GPIO_SetBits(RGB_STRIP_CLK_GPIO_PORT, RGB_STRIP_CLK_PIN);
#define RGB_STRIP_BOTTOM_CLK_OFF			GPIO_ResetBits(RGB_STRIP_CLK_GPIO_PORT, RGB_STRIP_CLK_PIN);
#define RGB_STRIP_BOTTOM_CLK_TOGGLE			GPIO_ToggleBits(RGB_STRIP_CLK_GPIO_PORT, RGB_STRIP_CLK_PIN);

/* RGB Strip bottom DATA = PG.7  */
#define RGB_STRIP_BOTTOM_DATA_PIN      		GPIO_Pin_7
#define RGB_STRIP_BOTTOM_DATA_GPIO_PORT    	GPIOG
#define RGB_STRIP_BOTTOM_DATA_GPIO_CLK     	RCC_AHB1Periph_GPIOG
#define RGB_STRIP_BOTTOM_DATA_ON			GPIO_SetBits(RGB_STRIP_DATA_GPIO_PORT, RGB_STRIP_DATA_PIN); // FAST: GPIO->BSRRL = GPIO_Pin_7
#define RGB_STRIP_BOTTOM_DATA_OFF			GPIO_ResetBits(RGB_STRIP_DATA_GPIO_PORT, RGB_STRIP_DATA_PIN); // FAST: GPIO->BSRRH = GPIO_Pin_7
#define RGB_STRIP_BOTTOM_DATA_TOGGLE		GPIO_ToggleBits(RGB_STRIP_DATA_GPIO_PORT, RGB_STRIP_DATA_PIN);

  #define RGB_STRIP_SPIx                           SPI2
  #define RGB_STRIP_SPIx_CLK                       RCC_APB1Periph_SPI2
  #define RGB_STRIP_SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
  #define RGB_STRIP_SPIx_IRQn                      SPI2_IRQn
  #define RGB_STRIP_SPIx_IRQHANDLER                SPI2_IRQHandler

  #define RGB_STRIP_SPIx_SCK_PIN                   GPIO_Pin_10
  #define RGB_STRIP_SPIx_SCK_GPIO_PORT             GPIOB
  #define RGB_STRIP_SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
  #define RGB_STRIP_SPIx_SCK_SOURCE                GPIO_PinSource10
  #define RGB_STRIP_SPIx_SCK_AF                    GPIO_AF_SPI2

  #define RGB_STRIP_SPIx_MOSI_PIN                  GPIO_Pin_3
  #define RGB_STRIP_SPIx_MOSI_GPIO_PORT            GPIOC
  #define RGB_STRIP_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define RGB_STRIP_SPIx_MOSI_SOURCE               GPIO_PinSource3
  #define RGB_STRIP_SPIx_MOSI_AF                   GPIO_AF_SPI2


#define RGB_STRIP_NUM_PIXELS	122
#define RGB_STRIP_LENGTH		RGB_STRIP_NUM_PIXELS

#define RGB_STRIP_NUM_PIXELS_BOTTOM		37
#define RGB_STRIP_LENGTH_BOTTOM		RGB_STRIP_NUM_PIXELS_BOTTOM

#define RGB_STRIP_FADE_STEPS	50
#define RGB_STRIP_PULSE_STEPS	50

#define RGB_STRIP_SCORE_RIGHT_POS	110
#define RGB_STRIP_SCORE_LEFT_POS	50


#define RGB_STRIP_IDLE_RED			20
#define RGB_STRIP_IDLE_GREEN		10
#define RGB_STRIP_IDLE_BLUE			10

#define RGB_STRIP_GAMEIDLE_RED		255
#define RGB_STRIP_GAMEIDLE_GREEN	220
#define RGB_STRIP_GAMEIDLE_BLUE		220



typedef struct {
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
} pixelColor;

extern pixelColor RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS];
extern pixelColor RGB_Strip_Bottom_Pixels[RGB_STRIP_NUM_PIXELS_BOTTOM];

typedef enum {
	RGB_STRIP_OFF = 0,
	RGB_STRIP_IDLE, // Not used, but is set after FADE or SET
	RGB_STRIP_FADE,
	RGB_STRIP_SET,
	RGB_STRIP_PULSE,
	RGB_STRIP_SPINNER,
	RGB_STRIP_RAINBOW_FADE,
	RGB_STRIP_GAME_SCORE_RED,
	RGB_STRIP_GAME_SCORE_BLUE,
	RGB_STRIP_GAME_WINNER_RED,
	RGB_STRIP_GAME_WINNER_BLUE
} RGB_STRIP_MODE;

extern RGB_STRIP_MODE RGB_Strip_Mode;

uint8_t RGB_SetColor_Red, RGB_SetColor_Green, RGB_SetColor_Blue;
uint8_t RGB_SetColorOld_Red, RGB_SetColorOld_Green, RGB_SetColorOld_Blue;


void RGB_Strip_Init_GPIO(void);
void RGB_Strip_Init_SPI(void);
void RGB_Strip_WriteColorStrip_SPI(uint8_t RED, uint8_t GREEN, uint8_t BLUE, uint8_t LEDNUM);
void RGB_Strip_WriteColorStrip_IO(uint8_t RED, uint8_t GREEN, uint8_t BLUE, uint8_t LEDNUM);
void RGB_Strip_WriteColorStrip_Bottom_IO(uint8_t RED, uint8_t GREEN, uint8_t BLUE, uint8_t LEDNUM);
void RGB_Strip_Update_SPI(void);
void RGB_Strip_Update_IO(void);
void RGB_Strip_Update_Bottom_IO(void);

void RGB_Strip_Task( void *pvParameters );


void RGB_Strip_Score(uint8_t side, uint8_t red, uint8_t green, uint8_t blue);
void RGB_Strip_Fade(uint8_t RED, uint8_t GREEN, uint8_t BLUE);
void RGB_Strip_Set(uint8_t RED, uint8_t GREEN, uint8_t BLUE);
void RGB_Strip_Pulse(uint8_t RED, uint8_t GREEN, uint8_t BLUE);


#endif /* __DEBUGGING_H */
