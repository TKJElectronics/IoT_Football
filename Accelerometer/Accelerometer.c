#include "Accelerometer.h"
#include "stm32f4xx.h"
#include "SSD1963_touch.h" // For SPI_Mutex

uint8_t Accelerometer_TiltDetected = 0;
uint8_t TiltDetectionEnabled = 0;
int8_t AccX_Offset = 0;


void Accelerometer_DelayShort(uint32_t delayAmount)
{
	while (delayAmount--);
}

void Accelerometer_Init(void)
{
	//Accelerometer_Init_I2C();
	//Accelerometer_Init_SPI();
	Accelerometer_Init_GPIO();
	Accelerometer_Init_Registers();
}

void Accelerometer_Init_I2C(void)
{
		GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as I2C1SDA and I2C1SCL
		GPIO_InitTypeDef GPIO_Output;
		I2C_InitTypeDef I2C_InitStruct; // this is for the I2C1 initilization
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		/* enable the peripheral clock for the pins used by 
		* I2C1, PB6 for I2C SCL and PB9 for I2C1_SDL
		*/
		/* GPIOB clock enable */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
		/* This sequence sets up the I2C1SDA and I2C1SCL pins 
		* so they work correctly with the I2C1 peripheral
		*/
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // Pins 6 (I2C1_SCL) and 9 (I2C1_SDA) are used
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 			// the pins are configured as alternate function so the USART peripheral has access to them
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;		// this defines the IO speed and has nothing to do with the baudrate!
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// this defines the output type as open drain mode (as opposed to push pull)
		GPIO_Init(GPIOB, &GPIO_InitStruct);					// now all the values are passed to the GPIO_Init() function which sets the GPIO registers
	
		
		/* enable APB1 peripheral clock for I2C1 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

        I2C_DeInit(I2C1);


		/* The I2C1_SCL and I2C1_SDA pins are now connected to their AF
		* so that the USART1 can take over control of the 
		* pins
		*/
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1); //
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
		/* Configure I2C1 */
		/* I2C DeInit */
		//I2C_DeInit(I2C1);
		/* Enable the I2C peripheral */
		I2C_Cmd(I2C1, ENABLE);
 
		/* Set the I2C structure parameters */
		I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
		I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
		I2C_InitStruct.I2C_OwnAddress1 = 0x00; //We are the master. We don't need this
		I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStruct.I2C_ClockSpeed = 100000;  //400kHz (Fast Mode)
  
		/* Initialize the I2C peripheral w/ selected parameters */
		I2C_Init(I2C1, &I2C_InitStruct);

		/* While the bus is busy */
		//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
}

void Accelerometer_Init_SPI(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  SPI_InitTypeDef  SPI_InitStructure;

	  /* Peripheral Clock Enable -------------------------------------------------*/
	  /* Enable the SPI clock */
	  MMA7455_SPIx_CLK_INIT(MMA7455_SPIx_CLK, ENABLE);

	  /* Enable GPIO clocks */
	  RCC_AHB1PeriphClockCmd(MMA7455_SPIx_SCK_GPIO_CLK  | MMA7455_SPIx_MOSI_GPIO_CLK | MMA7455_SPIx_MISO_GPIO_CLK, ENABLE);

	  /* SPI GPIO Configuration --------------------------------------------------*/
	  /* GPIO Deinitialisation */
	  GPIO_DeInit(MMA7455_SPIx_SCK_GPIO_PORT);
	  GPIO_DeInit(MMA7455_SPIx_MOSI_GPIO_PORT);
	  GPIO_DeInit(MMA7455_SPIx_MISO_GPIO_PORT);

	  /* Connect SPI pins to AF5 */
	  GPIO_PinAFConfig(MMA7455_SPIx_SCK_GPIO_PORT, MMA7455_SPIx_SCK_SOURCE, MMA7455_SPIx_SCK_AF);
	  GPIO_PinAFConfig(MMA7455_SPIx_MOSI_GPIO_PORT, MMA7455_SPIx_MOSI_SOURCE, MMA7455_SPIx_MOSI_AF);
	  GPIO_PinAFConfig(MMA7455_SPIx_MISO_GPIO_PORT, MMA7455_SPIx_MISO_SOURCE, MMA7455_SPIx_MISO_AF);

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	  /* SPI SCK pin configuration */
	  GPIO_InitStructure.GPIO_Pin = MMA7455_SPIx_SCK_PIN;
	  GPIO_Init(MMA7455_SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

	  /* SPI  MOSI pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  MMA7455_SPIx_MOSI_PIN;
	  GPIO_Init(MMA7455_SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin =  MMA7455_SPIx_MISO_PIN;
	  GPIO_Init(MMA7455_SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);

	  /* SPI configuration -------------------------------------------------------*/
	  SPI_I2S_DeInit(MMA7455_SPIx);
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

	  SPI_Init(MMA7455_SPIx, &SPI_InitStructure);

	  /* Enable the SPI peripheral */
	  SPI_Cmd(MMA7455_SPIx, ENABLE);
}

void Accelerometer_Init_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(MMA7455_SPIx_SCK_GPIO_CLK | MMA7455_SPIx_MISO_GPIO_CLK | MMA7455_SPIx_MOSI_GPIO_CLK | MMA7455_SPIx_CS_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = MMA7455_SPIx_SCK_PIN;
	GPIO_Init(MMA7455_SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MMA7455_SPIx_MOSI_PIN;
	GPIO_Init(MMA7455_SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MMA7455_SPIx_CS_PIN;
	GPIO_Init(MMA7455_SPIx_CS_GPIO_PORT, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = MMA7455_SPIx_MISO_PIN;
	GPIO_Init(MMA7455_SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);

	MMA7455_SPIx_CS_GPIO_PORT->BSRRL = MMA7455_SPIx_CS_PIN; // SET PIN
}

void Accelerometer_Write_I2C(uint16_t Register, uint8_t Data)
{	
		/* Generate the Start Condition */
		I2C_GenerateSTART(I2C1, ENABLE);

		/* Test on I2C1 EV5, Start trnsmitted successfully and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
   
		/* Send Memory device slave Address for write */
		I2C_Send7bitAddress(I2C1, MMA7455_ADDRESS, I2C_Direction_Transmitter);
 
		/* Test on I2C1 EV6 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

		/* Send I2C1 location address LSB */
		I2C_SendData(I2C1, Register);

		/* Test on I2C1 EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
		/* Send Data */
		I2C_SendData(I2C1, Data);

		/* Test on I2C1 EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
 
		/* Send I2C1 STOP Condition */
		I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t Accelerometer_ReadByte_I2C(uint16_t Register)
{
		uint8_t Data = 0;
	
		/* While the bus is busy - we have only one device */
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

		/* Generate the Start Condition */
		I2C_GenerateSTART(I2C1, ENABLE);

		/* Test on I2C1 EV5 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
		/* Send DCMI selcted device slave Address for write */
		I2C_Send7bitAddress(I2C1, MMA7455_ADDRESS, I2C_Direction_Transmitter);
 
		/* Test on I2C1 EV6 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

		/* Clear EV6 by setting again the PE bit */
		I2C_Cmd(I2C1, ENABLE);

		/* Send I2C1 location address LSB */
		I2C_SendData(I2C1, Register);

		/* Test on I2C1 EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

		/* Generate the Start Condition */
		I2C_GenerateSTART(I2C1, ENABLE);
  
		/* Test on I2C1 EV6 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
		/* Send DCMI selcted device slave Address for write */
		I2C_Send7bitAddress(I2C1, MMA7455_ADDRESS, I2C_Direction_Receiver);
   
		/* Test on I2C1 EV6 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
 
		/* Prepare an NACK for the next data received */
		I2C_AcknowledgeConfig(I2C1, DISABLE);  

		/* Test on I2C1 EV7 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    
		/* Prepare Stop after receiving data */
		I2C_GenerateSTOP(I2C1, ENABLE);
	
		/* Receive the Data */
		Data = I2C_ReceiveData(I2C1);
		
		/* return the read data */
		return Data;
}

void Accelerometer_Write(uint8_t Register, uint8_t Data)
{
	MMA7455_SPIx->DR = MMA7455_WRITE_FLAG | (Register<<1); // write data to be transmitted to the SPI data register
	while( !(MMA7455_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( MMA7455_SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore

	MMA7455_SPIx->DR = Data; // write data to be transmitted to the SPI data register
	while( !(MMA7455_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( MMA7455_SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
}

uint8_t Accelerometer_ReadByte(uint8_t Register)
{
	MMA7455_SPIx->DR = (Register<<1); // write data to be transmitted to the SPI data register
	while( !(MMA7455_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( MMA7455_SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore

	MMA7455_SPIx->DR = 0x00; // DUMMY FOR READ!
	while( !(MMA7455_SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(MMA7455_SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( MMA7455_SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return MMA7455_SPIx->DR; // return received data from SPI data register
}


void Accelerometer_GPIO_Write(uint8_t Register, uint8_t Data)
{
	uint8_t i, bit;

	Register = MMA7455_WRITE_FLAG | (Register<<1);

	MMA7455_SPIx_CS_GPIO_PORT->BSRRH = MMA7455_SPIx_CS_PIN; // CLEAR PIN
	Accelerometer_DelayShort(100);

	for(bit=0x80; bit; bit >>= 1) {
		if(Register & bit) MMA7455_SPIx_MOSI_GPIO_PORT->BSRRL = MMA7455_SPIx_MOSI_PIN; // SET PIN
	    else MMA7455_SPIx_MOSI_GPIO_PORT->BSRRH = MMA7455_SPIx_MOSI_PIN; // CLEAR PIN
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
		Accelerometer_DelayShort(100);
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
		Accelerometer_DelayShort(100);
	}

	for(bit=0x80; bit; bit >>= 1) {
		if(Data & bit) MMA7455_SPIx_MOSI_GPIO_PORT->BSRRL = MMA7455_SPIx_MOSI_PIN; // SET PIN
	    else MMA7455_SPIx_MOSI_GPIO_PORT->BSRRH = MMA7455_SPIx_MOSI_PIN; // CLEAR PIN
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
		Accelerometer_DelayShort(100);
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
		Accelerometer_DelayShort(100);
	}

	MMA7455_SPIx_CS_GPIO_PORT->BSRRL = MMA7455_SPIx_CS_PIN; // SET PIN
	Accelerometer_DelayShort(500);
}

uint8_t Accelerometer_GPIO_ReadByte(uint8_t Register)
{
	uint8_t i, bit;
	uint8_t read = 0;

	Register = (Register<<1);

	MMA7455_SPIx_CS_GPIO_PORT->BSRRH = MMA7455_SPIx_CS_PIN; // CLEAR PIN
	Accelerometer_DelayShort(100);

	for(bit=0x80; bit; bit >>= 1) {
		if(Register & bit) MMA7455_SPIx_MOSI_GPIO_PORT->BSRRL = MMA7455_SPIx_MOSI_PIN; // SET PIN
	    else MMA7455_SPIx_MOSI_GPIO_PORT->BSRRH = MMA7455_SPIx_MOSI_PIN; // CLEAR PIN
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
		Accelerometer_DelayShort(100);
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
		Accelerometer_DelayShort(100);
	}

	for(bit=0x80; bit; bit >>= 1) {
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
		Accelerometer_DelayShort(100);
		if (MMA7455_SPIx_MISO_GPIO_PORT->IDR & MMA7455_SPIx_MISO_PIN) read |= bit;
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
		Accelerometer_DelayShort(100);
	}

	MMA7455_SPIx_CS_GPIO_PORT->BSRRL = MMA7455_SPIx_CS_PIN; // SET PIN
	Accelerometer_DelayShort(500);
	return read;
}

void Accelerometer_GPIO_WriteMultiple(uint8_t Register, uint8_t *Data, uint8_t num)
{
	uint8_t i, bit;

	Register = MMA7455_WRITE_FLAG | (Register<<1);

	MMA7455_SPIx_CS_GPIO_PORT->BSRRH = MMA7455_SPIx_CS_PIN; // CLEAR PIN
	Accelerometer_DelayShort(100);

	for(bit=0x80; bit; bit >>= 1) {
		if(Register & bit) MMA7455_SPIx_MOSI_GPIO_PORT->BSRRL = MMA7455_SPIx_MOSI_PIN; // SET PIN
	    else MMA7455_SPIx_MOSI_GPIO_PORT->BSRRH = MMA7455_SPIx_MOSI_PIN; // CLEAR PIN
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
		Accelerometer_DelayShort(100);
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
		Accelerometer_DelayShort(100);
	}

	for (i = 0; i < num; i++) {
		for(bit=0x80; bit; bit >>= 1) {
			if(Data[i] & bit) MMA7455_SPIx_MOSI_GPIO_PORT->BSRRL = MMA7455_SPIx_MOSI_PIN; // SET PIN
			else MMA7455_SPIx_MOSI_GPIO_PORT->BSRRH = MMA7455_SPIx_MOSI_PIN; // CLEAR PIN
			MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
			Accelerometer_DelayShort(100);
			MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
			Accelerometer_DelayShort(100);
		}
	}

	MMA7455_SPIx_CS_GPIO_PORT->BSRRL = MMA7455_SPIx_CS_PIN; // SET PIN
	Accelerometer_DelayShort(500);
}

uint8_t Accelerometer_GPIO_ReadMultiple(uint8_t Register, uint8_t *ReadBuffer, uint8_t num)
{
	uint8_t i, bit;
	uint8_t read = 0;

	Register = (Register<<1);

	MMA7455_SPIx_CS_GPIO_PORT->BSRRH = MMA7455_SPIx_CS_PIN; // CLEAR PIN
	Accelerometer_DelayShort(100);

	for(bit=0x80; bit; bit >>= 1) {
		if(Register & bit) MMA7455_SPIx_MOSI_GPIO_PORT->BSRRL = MMA7455_SPIx_MOSI_PIN; // SET PIN
	    else MMA7455_SPIx_MOSI_GPIO_PORT->BSRRH = MMA7455_SPIx_MOSI_PIN; // CLEAR PIN
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
		Accelerometer_DelayShort(100);
		MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
		Accelerometer_DelayShort(100);
	}

	for (i = 0; i < num; i++) {
		ReadBuffer[i] = 0;
		for(bit=0x80; bit; bit >>= 1) {
			MMA7455_SPIx_SCK_GPIO_PORT->BSRRL = MMA7455_SPIx_SCK_PIN; // SET PIN
			Accelerometer_DelayShort(100);
			if (MMA7455_SPIx_MISO_GPIO_PORT->IDR & MMA7455_SPIx_MISO_PIN) ReadBuffer[i] |= bit;
			MMA7455_SPIx_SCK_GPIO_PORT->BSRRH = MMA7455_SPIx_SCK_PIN; // CLEAR PIN
			Accelerometer_DelayShort(100);
		}
	}

	MMA7455_SPIx_CS_GPIO_PORT->BSRRL = MMA7455_SPIx_CS_PIN; // SET PIN
	Accelerometer_DelayShort(500);
	return 1;
}

uint8_t Accelerometer_Init_Registers(void)
{
	  int x, y, z, error;
	  xyz_union xyz;
	  uint8_t c1, c2;

	  // Initialize the sensor
	  //
	  // Sensitivity:
	  //    2g : GLVL0
	  //    4g : GLVL1
	  //    8g : GLVL1 | GLVL0
	  // Mode:
	  //    Standby         : 0
	  //    Measurement     : MODE0
	  //    Level Detection : MODE1
	  //    Pulse Detection : MODE1 | MODE0
	  // There was no need to add functions to write and read
	  // a single byte. So only the two functions to write
	  // and read multiple bytes are used.

	  // Set mode for "2g sensitivity" and "Measurement Mode".
	  c1 = (1<<MMA7455_GLVL0) | (1<<MMA7455_MODE0);
	  Accelerometer_GPIO_Write(MMA7455_MCTL, c1);

	  // Read it back, to test the sensor and communication.
	  c2 = Accelerometer_GPIO_ReadByte(MMA7455_MCTL);

	  if (c1 != c2)
	    return (-99);

	  // Clear the offset registers.
	  // If the Arduino was reset or with a warm-boot,
	  // there still could be offset written in the sensor.
	  // Only with power-up the offset values of the sensor
	  // are zero.
	  xyz.value.x = xyz.value.y = xyz.value.z = 0;
	  //Accelerometer_GPIO_WriteMultiple(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL, xyz.reg.x_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+1, xyz.reg.x_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+2, xyz.reg.y_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+3, xyz.reg.y_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+4, xyz.reg.z_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+5, xyz.reg.z_msb);

	  // The mode has just been set, and the sensor is activated.
	  // To get a valid reading, wait some time.
	  Accelerometer_DelayShort(20000000);


	  // Calcuate the offset.
	  //
	  // The values are 16-bits signed integers, but the sensor
	  // uses offsets of 11-bits signed integers.
	  // However that is not a problem,
	  // as long as the value is within the range.

	  // Assuming that the sensor is flat horizontal,
	  // the 'z'-axis should be 1 'g'. And 1 'g' is
	  // a value of 64 (if the 2g most sensitive setting
	  // is used).
	  // Note that the actual written value should be doubled
	  // for this sensor.

	  for (c1 = 0; c1 < 100; c1++) {
		  error = MMA7455_xyz (&x, &y, &z); // get the x,y,z values
		  Accelerometer_DelayShort(10000);
	  }
	  if (error != 0)
	    return (error);

	  xyz.value.x = 2 * -x;        // The sensor wants double values.
	  xyz.value.y = 2 * -y;
	  xyz.value.z = 2 * -(z-64);   // 64 is for 1 'g' for z-axis.

	  //Accelerometer_GPIO_WriteMultiple(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL, xyz.reg.x_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+1, xyz.reg.x_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+2, xyz.reg.y_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+3, xyz.reg.y_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+4, xyz.reg.z_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+5, xyz.reg.z_msb);

	  // The offset has been set, and everything should be okay.
	  // But by setting the offset, the offset of the sensor
	  // changes.
	  // A second offset calculation has to be done after
	  // a short delay, to compensate for that.
	  Accelerometer_DelayShort(20000000);

	  for (c1 = 0; c1 < 100; c1++) {
		  error = MMA7455_xyz (&x, &y, &z);    // get the x,y,z values again
		  Accelerometer_DelayShort(10000);
	  }
	  if (error != 0)
	    return (error);

	  xyz.value.x += 2 * -x;       // add to previous value
	  xyz.value.y += 2 * -y;
	  xyz.value.z += 2 * -(z-64);  // 64 is for 1 'g' for z-axis.

	  // Write the offset for a second time.
	  // This time the offset is fine tuned.
	  //Accelerometer_GPIO_WriteMultiple(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL, xyz.reg.x_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+1, xyz.reg.x_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+2, xyz.reg.y_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+3, xyz.reg.y_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+4, xyz.reg.z_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+5, xyz.reg.z_msb);

	  return (0);          // return : no error
}

uint8_t Accelerometer_Calibrate(void)
{
	  int x, y, z, error;
	  xyz_union xyz;
	  uint8_t c1, c2;

	  // Clear the offset registers.
	  // If the Arduino was reset or with a warm-boot,
	  // there still could be offset written in the sensor.
	  // Only with power-up the offset values of the sensor
	  // are zero.
	  xyz.value.x = xyz.value.y = xyz.value.z = 0;
	  //Accelerometer_GPIO_WriteMultiple(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL, xyz.reg.x_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+1, xyz.reg.x_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+2, xyz.reg.y_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+3, xyz.reg.y_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+4, xyz.reg.z_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+5, xyz.reg.z_msb);

	  // The mode has just been set, and the sensor is activated.
	  // To get a valid reading, wait some time.
	  Accelerometer_DelayShort(20000000);


	  // Calcuate the offset.
	  //
	  // The values are 16-bits signed integers, but the sensor
	  // uses offsets of 11-bits signed integers.
	  // However that is not a problem,
	  // as long as the value is within the range.

	  // Assuming that the sensor is flat horizontal,
	  // the 'z'-axis should be 1 'g'. And 1 'g' is
	  // a value of 64 (if the 2g most sensitive setting
	  // is used).
	  // Note that the actual written value should be doubled
	  // for this sensor.

	  for (c1 = 0; c1 < 100; c1++) {
		  error = MMA7455_xyz (&x, &y, &z); // get the x,y,z values
		  Accelerometer_DelayShort(10000);
	  }
	  if (error != 0)
	    return (error);

	  xyz.value.x = 2 * -x;        // The sensor wants double values.
	  xyz.value.y = 2 * -y;
	  xyz.value.z = 2 * -(z-64);   // 64 is for 1 'g' for z-axis.

	  //Accelerometer_GPIO_WriteMultiple(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL, xyz.reg.x_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+1, xyz.reg.x_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+2, xyz.reg.y_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+3, xyz.reg.y_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+4, xyz.reg.z_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+5, xyz.reg.z_msb);

	  // The offset has been set, and everything should be okay.
	  // But by setting the offset, the offset of the sensor
	  // changes.
	  // A second offset calculation has to be done after
	  // a short delay, to compensate for that.
	  Accelerometer_DelayShort(20000000);

	  for (c1 = 0; c1 < 100; c1++) {
		  error = MMA7455_xyz (&x, &y, &z);    // get the x,y,z values again
		  Accelerometer_DelayShort(10000);
	  }
	  if (error != 0)
	    return (error);

	  xyz.value.x += 2 * -x;       // add to previous value
	  xyz.value.y += 2 * -y;
	  xyz.value.z += 2 * -(z-64);  // 64 is for 1 'g' for z-axis.

	  // Write the offset for a second time.
	  // This time the offset is fine tuned.
	  //Accelerometer_GPIO_WriteMultiple(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL, xyz.reg.x_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+1, xyz.reg.x_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+2, xyz.reg.y_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+3, xyz.reg.y_msb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+4, xyz.reg.z_lsb);
	  Accelerometer_GPIO_Write(MMA7455_XOFFL+5, xyz.reg.z_msb);

	  return (0);          // return : no error
}

int8_t Accelerometer_GetX(void)
{
	int8_t acc_tmp;
	Accelerometer_Lock();
	acc_tmp = Accelerometer_GPIO_ReadByte(X_OUT);
	Accelerometer_Unlock();
	return acc_tmp;
}
int8_t Accelerometer_GetY(void)
{
	int8_t acc_tmp;
	Accelerometer_Lock();
	Accelerometer_GPIO_ReadByte(Y_OUT);
	Accelerometer_Unlock();
	return acc_tmp;

}
int8_t Accelerometer_GetZ(void)
{
	int8_t acc_tmp;
	Accelerometer_Lock();
	acc_tmp = Accelerometer_GPIO_ReadByte(Z_OUT);
	Accelerometer_Unlock();
	return acc_tmp;
}

// --------------------------------------------------------
// MMA7455_xyz
//
// Get the 'g' forces.
// The values are with integers as 64 per 'g'.
//
int MMA7455_xyz( signed int *pX, signed int *pY, signed int *pZ)
{
  xyz_union xyz;
  int error;
  uint8_t c;

  Accelerometer_Lock();

  // Wait for status bit DRDY to indicate that
  // all 3 axis are valid.
  while ((Accelerometer_GPIO_ReadByte(MMA7455_STATUS) & (1<<MMA7455_DRDY)) == 0);
  // Read 6 bytes, containing the X,Y,Z information
  // as 10-bit signed integers.

  //error = Accelerometer_GPIO_ReadMultiple (MMA7455_XOUTL, (uint8_t *) &xyz, 6);
  xyz.reg.x_lsb = Accelerometer_GPIO_ReadByte(MMA7455_XOUTL);
  xyz.reg.x_msb = Accelerometer_GPIO_ReadByte(MMA7455_XOUTL+1);
  xyz.reg.y_lsb = Accelerometer_GPIO_ReadByte(MMA7455_XOUTL+2);
  xyz.reg.y_msb = Accelerometer_GPIO_ReadByte(MMA7455_XOUTL+3);
  xyz.reg.z_lsb = Accelerometer_GPIO_ReadByte(MMA7455_XOUTL+4);
  xyz.reg.z_msb = Accelerometer_GPIO_ReadByte(MMA7455_XOUTL+5);

  Accelerometer_Unlock();

  // The output is 10-bits and could be negative.
  // To use the output as a 16-bit signed integer,
  // the sign bit (bit 9) is extended for the 16 bits.
  if (xyz.reg.x_msb & 0x02)    // Bit 9 is sign bit.
    xyz.reg.x_msb |= 0xFC;     // Stretch bit 9 over other bits.
  if (xyz.reg.y_msb & 0x02)
    xyz.reg.y_msb |= 0xFC;
  if (xyz.reg.z_msb & 0x02)
    xyz.reg.z_msb |= 0xFC;

  // The result is the g-force in units of 64 per 'g'.
  *pX = xyz.value.x;
  *pY = xyz.value.y;
  *pZ = xyz.value.z;

  return (0);                  // return : no error
}

void Accelerometer_SetOffsetX(void)
{
	int32_t AccAvg = 0;
	uint8_t i;
	for (i = 0; i < 20; i++) {
		AccAvg += Accelerometer_GetX();
		vTaskDelay(10);
	}
	AccX_Offset = AccAvg / 20;
}

void Accelerometer_Task(void *pvParameters)
{
	int8_t ACC_X;
	uint8_t aboveThreshold = 0;
	uint8_t i;

	for (i = 0; i < 100; i++) {
		ACC_X = Accelerometer_GetX();
		vTaskDelay(20);
	}

	vTaskDelay(2000);

	while (1)
	{
		if (TiltDetectionEnabled) {
			ACC_X = Accelerometer_GetX() - AccX_Offset;
			if (((ACC_X < -2 && ACC_X > -15) || (ACC_X > 2 && ACC_X < 15)) && (Accelerometer_TiltDetected == ACCELEROMETER_CLEAR)) {
				if (aboveThreshold > 25)
				{
					if (ACC_X < -2)
						Accelerometer_TiltDetected = ACCELEROMETER_TILT_DETECTED_RED;
					else if (ACC_X > 2)
						Accelerometer_TiltDetected = ACCELEROMETER_TILT_DETECTED_BLUE;

				} else {
					aboveThreshold++;
				}
			} else {
				aboveThreshold = 0;
			}
		}
		vTaskDelay(10);
	}
}
