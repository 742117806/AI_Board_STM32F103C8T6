#include "stm32f1xx.h"
#include "main.h"

#define SN3218A_SDB_H() HAL_GPIO_WritePin(LED_SN3218A_SDB_GPIO_Port, LED_SN3218A_SDB_Pin, GPIO_PIN_SET)
#define SN3218A_SDB_L() HAL_GPIO_WritePin(LED_SN3218A_SDB_GPIO_Port, LED_SN3218A_SDB_Pin, GPIO_PIN_RESET)

#define I2C_SDA_H() HAL_GPIO_WritePin(LED_SN3218A_SDA_GPIO_Port, LED_SN3218A_SDA_Pin, GPIO_PIN_SET)
#define I2C_SDA_L() HAL_GPIO_WritePin(LED_SN3218A_SDA_GPIO_Port, LED_SN3218A_SDA_Pin, GPIO_PIN_RESET)
#define I2C_SDA_RD() HAL_GPIO_ReadPin(LED_SN3218A_SDA_GPIO_Port, LED_SN3218A_SDA_Pin)

#define I2C_SCL_H() HAL_GPIO_WritePin(LED_SN3218A_SCL_GPIO_Port, LED_SN3218A_SCL_Pin, GPIO_PIN_SET)
#define I2C_SCL_L() HAL_GPIO_WritePin(LED_SN3218A_SCL_GPIO_Port, LED_SN3218A_SCL_Pin, GPIO_PIN_RESET)

#define SN3218A_POWER_ON() SN3218A_SDB_H()
#define SN3218A_POWER_OFF() SN3218A_SDB_L()

#define SN3218_PowerOn() SN3218A_SDB_H()
#define SN3218_PowerOff() SN3218A_SDB_L()

void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write1Byte(uint8_t buf);
void I2c_Init(void);
