
#ifndef __SPI_H__
#define __SPI_H__
#include "stm32f1xx.h"

#define WIRELESS_NSEL_L() (HAL_GPIO_WritePin(SI4438_NSS_GPIO_Port, SI4438_NSS_Pin, GPIO_PIN_RESET))
#define WIRELESS_NSEL_H() (HAL_GPIO_WritePin(SI4438_NSS_GPIO_Port, SI4438_NSS_Pin, GPIO_PIN_SET))
#define SDN_L() (HAL_GPIO_WritePin(SI4438_SDN_GPIO_Port, SI4438_SDN_Pin, GPIO_PIN_RESET))
#define SDN_H() (HAL_GPIO_WritePin(SI4438_SDN_GPIO_Port, SI4438_SDN_Pin, GPIO_PIN_SET))

uint8_t SPI_RWbyte(uint8_t sdata);

void WIRELESS_SPI_Init(void);
uint8_t SPI_RWbyte(uint8_t sdata);

#endif
