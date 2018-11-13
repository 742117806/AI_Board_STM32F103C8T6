/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define TOUCH2_Pin GPIO_PIN_13
#define TOUCH2_GPIO_Port GPIOC
#define TOUCH4_Pin GPIO_PIN_14
#define TOUCH4_GPIO_Port GPIOC
#define TOUCH1_Pin GPIO_PIN_15
#define TOUCH1_GPIO_Port GPIOC
#define LEDG_Pin GPIO_PIN_1
#define LEDG_GPIO_Port GPIOA
#define LEDR_Pin GPIO_PIN_5
#define LEDR_GPIO_Port GPIOA
#define SI4438_RX_Pin GPIO_PIN_12
#define SI4438_RX_GPIO_Port GPIOB
#define SI4438_TX_Pin GPIO_PIN_13
#define SI4438_TX_GPIO_Port GPIOB
#define SI4438_SDN_Pin GPIO_PIN_11
#define SI4438_SDN_GPIO_Port GPIOA
#define SI4438_nIRQ_Pin GPIO_PIN_12
#define SI4438_nIRQ_GPIO_Port GPIOA
#define SI4438_nIRQ_EXTI_IRQn EXTI15_10_IRQn
#define SI4438_NSS_Pin GPIO_PIN_15
#define SI4438_NSS_GPIO_Port GPIOA
#define TOUCH3_Pin GPIO_PIN_6
#define TOUCH3_GPIO_Port GPIOB
#define LED_SN3218A_SDB_Pin GPIO_PIN_7
#define LED_SN3218A_SDB_GPIO_Port GPIOB
#define LED_SN3218A_SDA_Pin GPIO_PIN_8
#define LED_SN3218A_SDA_GPIO_Port GPIOB
#define LED_SN3218A_SCL_Pin GPIO_PIN_9
#define LED_SN3218A_SCL_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
#define LEDR_ON() HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, GPIO_PIN_SET)
#define LEDR_OFF() HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, GPIO_PIN_RESET)
#define LEDR_TOGGLE() HAL_GPIO_TogglePin(LEDR_GPIO_Port, LEDR_Pin)
#define LEDR_INVERSE() HAL_GPIO_TogglePin(LEDR_GPIO_Port, LEDR_Pin)
#define LEDG_ON() HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, GPIO_PIN_SET)
#define LEDG_OFF() HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, GPIO_PIN_RESET)
#define LEDG_TOGGLE() HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin)
#define LEDG_INVERSE() HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin)

#define Touch1_RDPIN() HAL_GPIO_ReadPin(TOUCH1_GPIO_Port, TOUCH1_Pin)
#define Touch2_RDPIN() HAL_GPIO_ReadPin(TOUCH2_GPIO_Port, TOUCH2_Pin)
#define Touch3_RDPIN() HAL_GPIO_ReadPin(TOUCH3_GPIO_Port, TOUCH3_Pin)
#define Touch4_RDPIN() HAL_GPIO_ReadPin(TOUCH4_GPIO_Port, TOUCH4_Pin)

#define NIRQ_RDPIN() HAL_GPIO_ReadPin(SI4438_nIRQ_GPIO_Port, SI4438_nIRQ_Pin)
/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
