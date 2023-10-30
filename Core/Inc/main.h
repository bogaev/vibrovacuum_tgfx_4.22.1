/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "backend/config.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define COIL_PWM_Pin GPIO_PIN_1
#define COIL_PWM_GPIO_Port GPIOA
#define LCD_BCK_LGHT_Pin GPIO_PIN_3
#define LCD_BCK_LGHT_GPIO_Port GPIOA
#define PRESSURE_SENS_Pin GPIO_PIN_4
#define PRESSURE_SENS_GPIO_Port GPIOA
#define COIL_RES1_Pin GPIO_PIN_12
#define COIL_RES1_GPIO_Port GPIOH
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_11
#define LCD_RST_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_8
#define LED1_GPIO_Port GPIOC
#define LCD_INTRPT_Pin GPIO_PIN_7
#define LCD_INTRPT_GPIO_Port GPIOD
#define LCD_INTRPT_EXTI_IRQn EXTI9_5_IRQn
#define PUMP_PWM_Pin GPIO_PIN_7
#define PUMP_PWM_GPIO_Port GPIOB
#define PUMP_RES1_Pin GPIO_PIN_8
#define PUMP_RES1_GPIO_Port GPIOB
#define PUMP_RES2_Pin GPIO_PIN_9
#define PUMP_RES2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
