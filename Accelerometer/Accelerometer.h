#ifndef __ACCELEROMETER_H
#define __ACCELEROMETER_H

#include "stm32f4xx.h"

#include "Os_Wrap.h"
extern OS_SemaphoreHandle SPI_Mutex;

#define Accelerometer_Lock() OS_SemaphoreTake(SPI_Mutex,OS_MAX_DELAY)
#define Accelerometer_Unlock() OS_SemaphoreGive(SPI_Mutex)


#define ACCELEROMETER_CLEAR 0
#define ACCELEROMETER_TILT_DETECTED_BLUE 1
#define ACCELEROMETER_TILT_DETECTED_RED 2
extern uint8_t Accelerometer_TiltDetected;
extern uint8_t TiltDetectionEnabled;



  #define MMA7455_SPIx                           SPI2
  #define MMA7455_SPIx_CLK                       RCC_APB1Periph_SPI2
  #define MMA7455_SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
  #define MMA7455_SPIx_IRQn                      SPI2_IRQn
  #define MMA7455_SPIx_IRQHANDLER                SPI2_IRQHandler

  #define MMA7455_SPIx_SCK_PIN                   GPIO_Pin_10
  #define MMA7455_SPIx_SCK_GPIO_PORT             GPIOB
  #define MMA7455_SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
  #define MMA7455_SPIx_SCK_SOURCE                GPIO_PinSource10
  #define MMA7455_SPIx_SCK_AF                    GPIO_AF_SPI2

  #define MMA7455_SPIx_MOSI_PIN                  GPIO_Pin_3
  #define MMA7455_SPIx_MOSI_GPIO_PORT            GPIOC
  #define MMA7455_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define MMA7455_SPIx_MOSI_SOURCE               GPIO_PinSource3
  #define MMA7455_SPIx_MOSI_AF                   GPIO_AF_SPI2

  #define MMA7455_SPIx_MISO_PIN                  GPIO_Pin_2
  #define MMA7455_SPIx_MISO_GPIO_PORT            GPIOC
  #define MMA7455_SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define MMA7455_SPIx_MISO_SOURCE               GPIO_PinSource2
  #define MMA7455_SPIx_MISO_AF                   GPIO_AF_SPI2

  #define MMA7455_SPIx_CS_PIN                  GPIO_Pin_1
  #define MMA7455_SPIx_CS_GPIO_PORT            GPIOE
  #define MMA7455_SPIx_CS_GPIO_CLK             RCC_AHB1Periph_GPIOE


#define I2C_TIMEOUT_MAX 10000

#define MMA7455_ADDRESS 0x3A // 0x1D shifted 1 left //I2C Address for the sensor
#define MMA7455_WRITE_FLAG 0x80
#define MMA7455_MODE_CONTROLL 0x16 //Call the sensors Mode Control

#define MMA7455_2G_MODE 0x05 //Set Sensitivity to 2g
#define MMA7455_4G_MODE 0x09 //Set Sensitivity to 4g
#define MMA7455_8G_MODE 0x01 //Set Sensitivity to 8g

#define X_OUT 0x06 //Register for reading the X-Axis
#define Y_OUT 0x07 //Register for reading the Y-Axis
#define Z_OUT 0x08 //Register for reading the Z-Axis


// Register names according to the datasheet.
// Register 0x1C is sometimes called 'PW', and sometimes 'PD'.
// The two reserved registers can not be used.
#define MMA7455_XOUTL 0x00      // Read only, Output Value X LSB
#define MMA7455_XOUTH 0x01      // Read only, Output Value X MSB
#define MMA7455_YOUTL 0x02      // Read only, Output Value Y LSB
#define MMA7455_YOUTH 0x03      // Read only, Output Value Y MSB
#define MMA7455_ZOUTL 0x04      // Read only, Output Value Z LSB
#define MMA7455_ZOUTH 0x05      // Read only, Output Value Z MSB
#define MMA7455_XOUT8 0x06      // Read only, Output Value X 8 bits
#define MMA7455_YOUT8 0x07      // Read only, Output Value Y 8 bits
#define MMA7455_ZOUT8 0x08      // Read only, Output Value Z 8 bits
#define MMA7455_STATUS 0x09     // Read only, Status Register
#define MMA7455_DETSRC 0x0A     // Read only, Detection Source Register
#define MMA7455_TOUT 0x0B       // Temperature Output Value (Optional)
#define MMA7455_RESERVED1 0x0C  // Reserved
#define MMA7455_I2CAD 0x0D      // Read/Write, I2C Device Address
#define MMA7455_USRINF 0x0E     // Read only, User Information (Optional)
#define MMA7455_WHOAMI 0x0F     // Read only, "Who am I" value (Optional)
#define MMA7455_XOFFL 0x10      // Read/Write, Offset Drift X LSB
#define MMA7455_XOFFH 0x11      // Read/Write, Offset Drift X MSB
#define MMA7455_YOFFL 0x12      // Read/Write, Offset Drift Y LSB
#define MMA7455_YOFFH 0x13      // Read/Write, Offset Drift Y MSB
#define MMA7455_ZOFFL 0x14      // Read/Write, Offset Drift Z LSB
#define MMA7455_ZOFFH 0x15      // Read/Write, Offset Drift Z MSB
#define MMA7455_MCTL 0x16       // Read/Write, Mode Control Register
#define MMA7455_INTRST 0x17     // Read/Write, Interrupt Latch Reset
#define MMA7455_CTL1 0x18       // Read/Write, Control 1 Register
#define MMA7455_CTL2 0x19       // Read/Write, Control 2 Register
#define MMA7455_LDTH 0x1A       // Read/Write, Level Detection Threshold Limit Value
#define MMA7455_PDTH 0x1B       // Read/Write, Pulse Detection Threshold Limit Value
#define MMA7455_PD 0x1C         // Read/Write, Pulse Duration Value
#define MMA7455_LT 0x1D         // Read/Write, Latency Time Value (between pulses)
#define MMA7455_TW 0x1E         // Read/Write, Time Window for Second Pulse Value
#define MMA7455_RESERVED2 0x1F  // Reserved

// Defines for the bits, to be able to change
// between bit number and binary definition.
// By using the bit number, programming the MMA7455
// is like programming an AVR microcontroller.
// But instead of using "(1<<X)", or "_BV(X)",
// the Arduino "bit(X)" is used.
#define MMA7455_D0 0
#define MMA7455_D1 1
#define MMA7455_D2 2
#define MMA7455_D3 3
#define MMA7455_D4 4
#define MMA7455_D5 5
#define MMA7455_D6 6
#define MMA7455_D7 7

// Status Register
#define MMA7455_DRDY MMA7455_D0
#define MMA7455_DOVR MMA7455_D1
#define MMA7455_PERR MMA7455_D2

// Mode Control Register
#define MMA7455_MODE0 MMA7455_D0
#define MMA7455_MODE1 MMA7455_D1
#define MMA7455_GLVL0 MMA7455_D2
#define MMA7455_GLVL1 MMA7455_D3
#define MMA7455_STON MMA7455_D4
#define MMA7455_SPI3W MMA7455_D5
#define MMA7455_DRPD MMA7455_D6

// Control 1 Register
#define MMA7455_INTPIN MMA7455_D0
#define MMA7455_INTREG0 MMA7455_D1
#define MMA7455_INTREG1 MMA7455_D2
#define MMA7455_XDA MMA7455_D3
#define MMA7455_YDA MMA7455_D4
#define MMA7455_ZDA MMA7455_D5
#define MMA7455_THOPT MMA7455_D6
#define MMA7455_DFBW MMA7455_D7

// Control 2 Register
#define MMA7455_LDPL MMA7455_D0
#define MMA7455_PDPL MMA7455_D1
#define MMA7455_DRVO MMA7455_D2

// Interrupt Latch Reset Register
#define MMA7455_CLR_INT1 MMA7455_D0
#define MMA7455_CLR_INT2 MMA7455_D1

// Detection Source Register
#define MMA7455_INT1 MMA7455_D0
#define MMA7455_INT2 MMA7455_D1
#define MMA7455_PDZ MMA7455_D2
#define MMA7455_PDY MMA7455_D3
#define MMA7455_PDX MMA7455_D4
#define MMA7455_LDZ MMA7455_D5
#define MMA7455_LDY MMA7455_D6
#define MMA7455_LDX MMA7455_D7

// I2C Device Address Register
#define MMA7455_I2CDIS MMA7455_D7



// Default I2C address for the MMA7455
#define MMA7455_I2C_ADDRESS 0x1D

typedef union xyz_union
{
  struct
  {
    uint8_t x_lsb;
    uint8_t x_msb;
    uint8_t y_lsb;
    uint8_t y_msb;
    uint8_t z_lsb;
    uint8_t z_msb;
  } reg;
  struct
  {
    int16_t x;
    int16_t y;
    int16_t z;
  } value;
} xyz_union;


void Accelerometer_Init(void);
void Accelerometer_Init_I2C(void);
void Accelerometer_Init_SPI(void);
uint8_t Accelerometer_Init_Registers(void);
uint8_t Accelerometer_Calibrate(void);
void Accelerometer_Write(uint8_t Register, uint8_t Data);
uint8_t Accelerometer_ReadByte(uint8_t Register);

void Accelerometer_GPIO_Write(uint8_t Register, uint8_t Data);
void Accelerometer_GPIO_WriteMultiple(uint8_t Register, uint8_t *Data, uint8_t num);
uint8_t Accelerometer_GPIO_ReadByte(uint8_t Register);

int MMA7455_xyz( int *pX, int *pY, int *pZ);

void Accelerometer_SetOffsetX(void);
void Accelerometer_Task(void *pvParameters);



#endif /* __ACCELEROMETER_H */
