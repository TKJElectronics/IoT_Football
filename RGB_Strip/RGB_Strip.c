#include "RGB_Strip.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

pixelColor RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS];
pixelColor RGB_Strip_Bottom_Pixels[RGB_STRIP_NUM_PIXELS_BOTTOM];
RGB_STRIP_MODE RGB_Strip_Mode = RGB_STRIP_OFF;
float redIncrease, greenIncrease, blueIncrease;


void RGB_Strip_DelayShort(uint32_t delayAmount) // each = 0,05us
{
	while (delayAmount--);
}


void RGB_Strip_Init_GPIO(void)
{
	uint8_t led, bit;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RGB_STRIP_CLK_GPIO_CLK | RGB_STRIP_DATA_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RGB_STRIP_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(RGB_STRIP_CLK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RGB_STRIP_DATA_PIN;
	GPIO_Init(RGB_STRIP_DATA_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RGB_STRIP_BOTTOM_CLK_PIN;
	GPIO_Init(RGB_STRIP_BOTTOM_CLK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RGB_STRIP_BOTTOM_DATA_PIN;
	GPIO_Init(RGB_STRIP_BOTTOM_DATA_GPIO_PORT, &GPIO_InitStructure);

	RGB_STRIP_CLK_OFF;
	RGB_STRIP_DATA_OFF;

	/* Clear RGB strip */
	for (led = 0; led < RGB_STRIP_LENGTH; led++)
	{
		for(bit=0x80; bit; bit >>= 1) { // RED
			RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // GREEN
			RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // BLUE
			RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
	}

	/* Clear bottom RGB strip */
	for (led = 0; led < RGB_STRIP_LENGTH_BOTTOM; led++)
	{
		for(bit=0x80; bit; bit >>= 1) { // RED
			RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // GREEN
			RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // BLUE
			RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
	}
}

void RGB_Strip_Init_SPI(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  SPI_InitTypeDef  SPI_InitStructure;

	  /* Peripheral Clock Enable -------------------------------------------------*/
	  /* Enable the SPI clock */
	  RGB_STRIP_SPIx_CLK_INIT(RGB_STRIP_SPIx_CLK, ENABLE);

	  /* Enable GPIO clocks */
	  RCC_AHB1PeriphClockCmd(RGB_STRIP_SPIx_SCK_GPIO_CLK  | RGB_STRIP_SPIx_MOSI_GPIO_CLK, ENABLE);

	  /* SPI GPIO Configuration --------------------------------------------------*/
	  /* GPIO Deinitialisation */
	  GPIO_DeInit(RGB_STRIP_SPIx_SCK_GPIO_PORT);
	  GPIO_DeInit(RGB_STRIP_SPIx_MOSI_GPIO_PORT);

	  /* Connect SPI pins to AF5 */
	  GPIO_PinAFConfig(RGB_STRIP_SPIx_SCK_GPIO_PORT, RGB_STRIP_SPIx_SCK_SOURCE, RGB_STRIP_SPIx_SCK_AF);
	  GPIO_PinAFConfig(RGB_STRIP_SPIx_MOSI_GPIO_PORT, RGB_STRIP_SPIx_MOSI_SOURCE, RGB_STRIP_SPIx_MOSI_AF);

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	  /* SPI SCK pin configuration */
	  GPIO_InitStructure.GPIO_Pin = RGB_STRIP_SPIx_SCK_PIN;
	  GPIO_Init(RGB_STRIP_SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

	  /* SPI  MOSI pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  RGB_STRIP_SPIx_MOSI_PIN;
	  GPIO_Init(RGB_STRIP_SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

	  /* SPI configuration -------------------------------------------------------*/
	  SPI_I2S_DeInit(RGB_STRIP_SPIx);
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

	  SPI_Init(RGB_STRIP_SPIx, &SPI_InitStructure);

	  /* Enable the SPI peripheral */
	  SPI_Cmd(RGB_STRIP_SPIx, ENABLE);
}

uint8_t RGB_Strip_Send(uint8_t data)
{

	RGB_STRIP_SPIx->DR = data; // write data to be transmitted to the SPI data register
	while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return RGB_STRIP_SPIx->DR; // return received data from SPI data register
}

void RGB_Strip_Update_SPI(void)
{
	uint8_t i;
	for (i = 0; i < RGB_STRIP_NUM_PIXELS; i++) {
		RGB_STRIP_SPIx->DR = RGB_Strip_Pixels[i].Red; // write data to be transmitted to the SPI data register
		while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
		RGB_STRIP_SPIx->DR = RGB_Strip_Pixels[i].Green; // write data to be transmitted to the SPI data register
		while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
		RGB_STRIP_SPIx->DR = RGB_Strip_Pixels[i].Blue; // write data to be transmitted to the SPI data register
		while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	}

	vTaskDelay(2);
}

void RGB_Strip_Update_IO(void)
{
	uint8_t i, bit;

	for (i = 0; i < RGB_STRIP_NUM_PIXELS; i++) {
		for(bit=0x80; bit; bit >>= 1) { // RED
			if(RGB_Strip_Pixels[i].Red & bit) RGB_STRIP_DATA_GPIO_PORT->BSRRL = RGB_STRIP_DATA_PIN; // SET PIN
	        else RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // GREEN
			if(RGB_Strip_Pixels[i].Green & bit) RGB_STRIP_DATA_GPIO_PORT->BSRRL = RGB_STRIP_DATA_PIN; // SET PIN
	        else RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // BLUE
			if(RGB_Strip_Pixels[i].Blue & bit) RGB_STRIP_DATA_GPIO_PORT->BSRRL = RGB_STRIP_DATA_PIN; // SET PIN
	        else RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
	}

	vTaskDelay(1);
	//RGB_Strip_DelayShort(12000);
}

void RGB_Strip_Update_Bottom_IO(void)
{
	uint8_t i, bit;

	for (i = 0; i < RGB_STRIP_NUM_PIXELS_BOTTOM; i++) {
		for(bit=0x80; bit; bit >>= 1) { // RED
			if(RGB_Strip_Bottom_Pixels[i].Red & bit) RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_DATA_PIN; // SET PIN
	        else RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // GREEN
			if(RGB_Strip_Bottom_Pixels[i].Green & bit) RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_DATA_PIN; // SET PIN
	        else RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // BLUE
			if(RGB_Strip_Bottom_Pixels[i].Blue & bit) RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_DATA_PIN; // SET PIN
	        else RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
	}

	vTaskDelay(1);
	//RGB_Strip_DelayShort(12000);
}

void RGB_Strip_WriteColorStrip_SPI(uint8_t RED, uint8_t GREEN, uint8_t BLUE, uint8_t LEDNUM)
{
	uint8_t led;
	uint8_t bit;

	for (led = 0; led < LEDNUM; led++)
	{
		RGB_STRIP_SPIx->DR = RED; // write data to be transmitted to the SPI data register
		while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
		RGB_STRIP_SPIx->DR = GREEN; // write data to be transmitted to the SPI data register
		while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
		RGB_STRIP_SPIx->DR = BLUE; // write data to be transmitted to the SPI data register
		while( !(RGB_STRIP_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	}

	vTaskDelay(1);
}

void RGB_Strip_WriteColorStrip_IO(uint8_t RED, uint8_t GREEN, uint8_t BLUE, uint8_t LEDCOUNT)
{
	uint8_t led;
	uint8_t bit;

	for (led = 0; led < LEDCOUNT; led++)
	{
		for(bit=0x80; bit; bit >>= 1) { // RED
			if(RED & bit) RGB_STRIP_DATA_GPIO_PORT->BSRRL = RGB_STRIP_DATA_PIN; // SET PIN
	        else RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // GREEN
			if(GREEN & bit) RGB_STRIP_DATA_GPIO_PORT->BSRRL = RGB_STRIP_DATA_PIN; // SET PIN
	        else RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // BLUE
			if(BLUE & bit) RGB_STRIP_DATA_GPIO_PORT->BSRRL = RGB_STRIP_DATA_PIN; // SET PIN
	        else RGB_STRIP_DATA_GPIO_PORT->BSRRH = RGB_STRIP_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_CLK_GPIO_PORT->BSRRL = RGB_STRIP_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_CLK_GPIO_PORT->BSRRH = RGB_STRIP_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
	}

	vTaskDelay(1);
	//RGB_Strip_DelayShort(12000);
}

void RGB_Strip_WriteColorStrip_Bottom_IO(uint8_t RED, uint8_t GREEN, uint8_t BLUE, uint8_t LEDCOUNT)
{
	uint8_t led;
	uint8_t bit;

	for (led = 0; led < LEDCOUNT; led++)
	{
		for(bit=0x80; bit; bit >>= 1) { // RED
			if(RED & bit) RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_DATA_PIN; // SET PIN
	        else RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // GREEN
			if(GREEN & bit) RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_DATA_PIN; // SET PIN
	        else RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
		for(bit=0x80; bit; bit >>= 1) { // BLUE
			if(BLUE & bit) RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_DATA_PIN; // SET PIN
	        else RGB_STRIP_BOTTOM_DATA_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_DATA_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(2);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRL = RGB_STRIP_BOTTOM_CLK_PIN; // SET PIN
			RGB_Strip_DelayShort(50);
			RGB_STRIP_BOTTOM_CLK_GPIO_PORT->BSRRH = RGB_STRIP_BOTTOM_CLK_PIN; // CLEAR PIN
			RGB_Strip_DelayShort(50);
	    }
	}

	vTaskDelay(1);
	//RGB_Strip_DelayShort(12000);
}



void RGB_Strip_Task( void *pvParameters )
{
	RGB_STRIP_MODE RGB_Strip_Mode_Old = RGB_STRIP_OFF;
	uint8_t i;
	uint8_t cRED, cGREEN, cBLUE;
	uint16_t rainbowStep = 0;
	uint8_t pulseStep = 0;
	uint8_t winnerStep = 0;
	RGB_Strip_WriteColorStrip(0, 0, 0, RGB_STRIP_LENGTH);
    RGB_Strip_WriteColorStrip_Bottom(0, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
	RGB_SetColorOld_Red = 0;
	RGB_SetColorOld_Green = 0;
	RGB_SetColorOld_Blue = 0;

	for ( ;; )
	{
		if (RGB_Strip_Mode == RGB_STRIP_RAINBOW_FADE) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			/*colorWipe(255, 0, 0, 50);
			colorWipe(0, 255, 0, 50);
			colorWipe(0, 0, 255, 50);
			rainbow(20);
			rainbowCycle(20);
			rainbow(20);*/

			rainbowStep++;
			if (rainbowStep > 256) rainbowStep = 0;
		    for (i=0; i < RGB_STRIP_NUM_PIXELS; i++) {
		    	Wheel( ((i + rainbowStep) % 255), &cRED, &cGREEN, &cBLUE);
		        RGB_Strip_Pixels[i].Red = cRED;
		        RGB_Strip_Pixels[i].Green = cGREEN;
		        RGB_Strip_Pixels[i].Blue = cBLUE;
		    }
		    for (i=0; i < RGB_STRIP_NUM_PIXELS_BOTTOM; i++) {
		    	Wheel( ((i + rainbowStep) % 255), &cRED, &cGREEN, &cBLUE);
		        RGB_Strip_Bottom_Pixels[i].Red = cRED;
		        RGB_Strip_Bottom_Pixels[i].Green = cGREEN;
		        RGB_Strip_Bottom_Pixels[i].Blue = cBLUE;
		    }
		    RGB_Strip_Update();   // write all the pixels out
		    RGB_Strip_Update_Bottom();   // write all the pixels out
		    vTaskDelay(20);
		} else

		if (RGB_Strip_Mode == RGB_STRIP_OFF && RGB_Strip_Mode != RGB_Strip_Mode_Old) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			RGB_Strip_WriteColorStrip(0, 0, 0, RGB_STRIP_LENGTH); // Turn all off!
			RGB_Strip_WriteColorStrip_Bottom(0, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
			RGB_Strip_Mode = RGB_STRIP_IDLE;
		} else

		if (RGB_Strip_Mode == RGB_STRIP_PULSE && RGB_Strip_Mode != RGB_Strip_Mode_Old) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;

			RGB_Strip_WriteColorStrip(0, 0, 0, RGB_STRIP_LENGTH); // Turn all off!
			RGB_Strip_WriteColorStrip_Bottom(0, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
		} else

		if (RGB_Strip_Mode == RGB_STRIP_FADE && RGB_Strip_Mode != RGB_Strip_Mode_Old) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;

			redIncrease = (float)(RGB_SetColor_Red - RGB_SetColorOld_Red) / RGB_STRIP_FADE_STEPS;
			greenIncrease = (float)(RGB_SetColor_Green - RGB_SetColorOld_Green) / RGB_STRIP_FADE_STEPS;
			blueIncrease = (float)(RGB_SetColor_Blue - RGB_SetColorOld_Blue) / RGB_STRIP_FADE_STEPS;

			for (i = 0; i < RGB_STRIP_FADE_STEPS; i++) {
				RGB_SetColor_Red = RGB_SetColorOld_Red + (i*redIncrease);
				RGB_SetColor_Green = RGB_SetColorOld_Green + (i*greenIncrease);
				RGB_SetColor_Blue = RGB_SetColorOld_Blue + (i*blueIncrease);
				RGB_Strip_WriteColorStrip(RGB_SetColor_Red, RGB_SetColor_Green, RGB_SetColor_Blue, RGB_STRIP_LENGTH);
				RGB_Strip_WriteColorStrip_Bottom(RGB_SetColor_Red, RGB_SetColor_Green, RGB_SetColor_Blue, RGB_STRIP_LENGTH_BOTTOM);
				vTaskDelay(2);
			}
			RGB_Strip_WriteColorStrip(RGB_SetColor_Red, RGB_SetColor_Green, RGB_SetColor_Blue, RGB_STRIP_LENGTH);
			RGB_Strip_WriteColorStrip_Bottom(RGB_SetColor_Red, RGB_SetColor_Green, RGB_SetColor_Blue, RGB_STRIP_LENGTH_BOTTOM);
			RGB_SetColorOld_Red = RGB_SetColor_Red;
			RGB_SetColorOld_Green = RGB_SetColor_Green;
			RGB_SetColorOld_Blue = RGB_SetColor_Blue;
			RGB_Strip_Mode = RGB_STRIP_IDLE;
		} else

		if (RGB_Strip_Mode == RGB_STRIP_SET && RGB_Strip_Mode != RGB_Strip_Mode_Old) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			RGB_Strip_WriteColorStrip(RGB_SetColor_Red, RGB_SetColor_Green, RGB_SetColor_Blue, RGB_STRIP_LENGTH); // White idle color
			RGB_Strip_WriteColorStrip_Bottom(RGB_SetColor_Red, RGB_SetColor_Green, RGB_SetColor_Blue, RGB_STRIP_LENGTH_BOTTOM); // White idle color
			RGB_SetColorOld_Red = RGB_SetColor_Red;
			RGB_SetColorOld_Green = RGB_SetColor_Green;
			RGB_SetColorOld_Blue = RGB_SetColor_Blue;
			RGB_Strip_Mode = RGB_STRIP_IDLE;
		} else

		if (RGB_Strip_Mode == RGB_STRIP_GAME_SCORE_RED) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			RGB_SetColorOld_Red = 0;
			RGB_SetColorOld_Green = 0;
			RGB_SetColorOld_Blue = 0;
			RGB_Strip_WriteColorStrip_Bottom(255, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
			RGB_Strip_Score(1, 255, 0, 0);
			RGB_Strip_Score(1, 255, 0, 0);
		} else

		if (RGB_Strip_Mode == RGB_STRIP_GAME_SCORE_BLUE) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			RGB_SetColorOld_Red = 0;
			RGB_SetColorOld_Green = 0;
			RGB_SetColorOld_Blue = 0;
			RGB_Strip_WriteColorStrip_Bottom(0, 0, 255, RGB_STRIP_LENGTH_BOTTOM);
			RGB_Strip_Score(0, 0, 0, 255);
			RGB_Strip_Score(0, 0, 0, 255);

		} else

		if (RGB_Strip_Mode == RGB_STRIP_GAME_WINNER_RED) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			if (winnerStep == 0) {
				RGB_Strip_WriteColorStrip(255, 0, 0, RGB_STRIP_LENGTH);
				RGB_Strip_WriteColorStrip_Bottom(255, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
				winnerStep = 1;
				vTaskDelay(100);
			} else {
				RGB_Strip_WriteColorStrip(0, 0, 0, RGB_STRIP_LENGTH);
				RGB_Strip_WriteColorStrip_Bottom(0, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
				winnerStep = 0;
				vTaskDelay(100);
			}
		} else

		if (RGB_Strip_Mode == RGB_STRIP_GAME_WINNER_BLUE) {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			if (winnerStep == 0) {
				RGB_Strip_WriteColorStrip(0, 0, 255, RGB_STRIP_LENGTH);
				RGB_Strip_WriteColorStrip_Bottom(0, 0, 255, RGB_STRIP_LENGTH_BOTTOM);
				winnerStep = 1;
				vTaskDelay(100);
			} else {
				RGB_Strip_WriteColorStrip(0, 0, 0, RGB_STRIP_LENGTH);
				RGB_Strip_WriteColorStrip_Bottom(0, 0, 0, RGB_STRIP_LENGTH_BOTTOM);
				winnerStep = 0;
				vTaskDelay(100);
			}
		}

		else {
			RGB_Strip_Mode_Old = RGB_Strip_Mode;
			vTaskDelay(100);
		}
	}
}

void RGB_Strip_Fade(uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
	RGB_SetColor_Red = RED;
	RGB_SetColor_Green = GREEN;
	RGB_SetColor_Blue = BLUE;
	RGB_Strip_Mode = RGB_STRIP_FADE;
}

void RGB_Strip_Set(uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
	RGB_SetColor_Red = RED;
	RGB_SetColor_Green = GREEN;
	RGB_SetColor_Blue = BLUE;
	RGB_Strip_Mode = RGB_STRIP_SET;
}

void RGB_Strip_Pulse(uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
	RGB_SetColor_Red = 0;
	RGB_SetColor_Green = 0;
	RGB_SetColor_Blue = 0;
	redIncrease = (float)RGB_SetColor_Red / RGB_STRIP_PULSE_STEPS;
	greenIncrease = (float)RGB_SetColor_Green / RGB_STRIP_PULSE_STEPS;
	blueIncrease = (float)RGB_SetColor_Blue / RGB_STRIP_PULSE_STEPS;
	RGB_Strip_Mode = RGB_STRIP_PULSE;
}


void rainbow(uint8_t wait) {
  int i, j;
  uint8_t cRED, cGREEN, cBLUE;

  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < RGB_STRIP_NUM_PIXELS; i++) {
    	Wheel( ((i + j) % 255), &cRED, &cGREEN, &cBLUE);
        RGB_Strip_Pixels[i].Red = cRED;
        RGB_Strip_Pixels[i].Green = cGREEN;
        RGB_Strip_Pixels[i].Blue = cBLUE;
    }
    RGB_Strip_Update();   // write all the pixels out
    vTaskDelay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  uint8_t cRED, cGREEN, cBLUE;

  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < RGB_STRIP_NUM_PIXELS; i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
    	Wheel( (((i * 256 / RGB_STRIP_NUM_PIXELS) + j) % 256), &cRED, &cGREEN, &cBLUE);
        RGB_Strip_Pixels[i].Red = cRED;
        RGB_Strip_Pixels[i].Green = cGREEN;
        RGB_Strip_Pixels[i].Blue = cBLUE;
    }
    RGB_Strip_Update();   // write all the pixels out
    vTaskDelay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint8_t cRED, uint8_t cGREEN, uint8_t cBLUE, uint8_t wait) {
  int i;

  for (i=0; i < RGB_STRIP_NUM_PIXELS; i++) {
      RGB_Strip_Pixels[i].Red = cRED;
      RGB_Strip_Pixels[i].Green = cGREEN;
      RGB_Strip_Pixels[i].Blue = cBLUE;
      RGB_Strip_Update();
      vTaskDelay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
void Wheel(uint8_t WheelPos, uint8_t *cRED, uint8_t *cGREEN, uint8_t *cBLUE)
{
  if (WheelPos < 85) {
	  *cRED = WheelPos * 3;
	  *cGREEN = 255 - (WheelPos * 3);
	  *cBLUE = 0;
  } else if (WheelPos < 170) {
	  WheelPos -= 85;
	  *cRED = 255 - (WheelPos * 3);
	  *cGREEN = 0;
	  *cBLUE = WheelPos * 3;
  } else {
	  WheelPos -= 170;
	  *cRED = 0;
	  *cGREEN = WheelPos * 3;
	  *cBLUE = 255 - (WheelPos * 3);
  }
}

void RGB_Strip_Score(uint8_t side, uint8_t red, uint8_t green, uint8_t blue) // side = 0 == right,    side = 1 == left
{
	uint8_t i, i2, i3;
	uint8_t found;
	uint8_t initPos;
	int16_t runnerPos1[6], runnerPos2[6];
	if (side == 0) { // right
		initPos = RGB_STRIP_SCORE_RIGHT_POS;
	} else {
		initPos = RGB_STRIP_SCORE_LEFT_POS;
	}

	for (i2 = 0; i2 < 6; i2++) {
		runnerPos1[i2] = initPos;
		runnerPos2[i2] = initPos;
	}

	/*for (i = 0; i < RGB_STRIP_NUM_PIXELS; i++) {
		RGB_Strip_Pixels[i].Red = 0;
		RGB_Strip_Pixels[i].Green = 0;
		RGB_Strip_Pixels[i].Blue = 0;
	}
	RGB_Strip_Pixels[initPos].Red = red;
	RGB_Strip_Pixels[initPos].Green = green;
	RGB_Strip_Pixels[initPos].Blue = blue;
	RGB_Strip_Update();
	vTaskDelay(5);*/

	for (i = 0; i <= (RGB_STRIP_SCORE_RIGHT_POS-RGB_STRIP_SCORE_LEFT_POS)+6; i++) {
		if (i < 6) i2 = i;
		else i2 = 6;

		for (i3 = 0; i3 < i2; i3++) {
			if (runnerPos1[i3] > 0)
				runnerPos1[i3] -= 2;
			else
				runnerPos1[i3] = RGB_STRIP_NUM_PIXELS;

			if (runnerPos2[i3] < RGB_STRIP_NUM_PIXELS)
				runnerPos2[i3] += 2;
			else
				runnerPos2[i3] = 0;
		}

		for (i2 = 0; i2 < RGB_STRIP_NUM_PIXELS; i2++) {
			found = 0;
			for (i3 = 0; i3 < 6; i3++) {
				if (runnerPos1[i3] == i2 || runnerPos2[i3] == i2) {
				    RGB_Strip_Pixels[i2].Red = red;
				    RGB_Strip_Pixels[i2].Green = green;
				    RGB_Strip_Pixels[i2].Blue = blue;
				    found = 1;
				    break;
				}
			}

			if (found == 0) {
			    RGB_Strip_Pixels[i2].Red = 0;
			    RGB_Strip_Pixels[i2].Green = 0;
			    RGB_Strip_Pixels[i2].Blue = 0;
			}
		}
		RGB_Strip_Update();

		vTaskDelay(1);
	}

	/*for (i=2; i < RGB_STRIP_NUM_PIXELS; i += 2) {
	  RGB_Strip_Pixels[i-2].Red = 0;
	  RGB_Strip_Pixels[i-2].Green = 0;
	  RGB_Strip_Pixels[i-2].Blue = 0;
      RGB_Strip_Pixels[i-1].Red = 0;
      RGB_Strip_Pixels[i-1].Green = 0;
      RGB_Strip_Pixels[i-1].Blue = 0;

      RGB_Strip_Pixels[i].Red = red;
      RGB_Strip_Pixels[i].Green = green;
      RGB_Strip_Pixels[i].Blue = blue;
      RGB_Strip_Pixels[i+1].Red = red;
      RGB_Strip_Pixels[i+1].Green = green;
      RGB_Strip_Pixels[i+1].Blue = blue;
      RGB_Strip_Update();
      RGB_Strip_Update_Bottom();
      vTaskDelay(1);
	}

    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS-2].Red = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS-2].Green = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS-2].Blue = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS-1].Red = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS-1].Green = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS-1].Blue = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS].Red = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS].Green = 0;
    RGB_Strip_Pixels[RGB_STRIP_NUM_PIXELS].Blue = 0;
    RGB_Strip_Update();
    RGB_Strip_Update_Bottom();*/
}
