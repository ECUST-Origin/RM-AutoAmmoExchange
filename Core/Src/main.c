/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "aae.h"
#include "key.h"
#include "bit_tools.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define KEY_NUM 3

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

enum RobotType_e{
  HERO = 0,
  INFANTRY = 1,
  ENGINEER = 2,
  DRONE = 3,
}RobotType;

void Key1_DownCallback();
void Key2_DownCallback();
void Key3_DownCallback();

Key_Handle_t Key1 = {
  .KeyGpioPort = KEY1_GPIO_Port,
  .KeyGpioPin = KEY1_Pin,
  .KeyDownCallback = &Key1_DownCallback,
};
Key_Handle_t Key2 = {
  .KeyGpioPort = KEY2_GPIO_Port,
  .KeyGpioPin = KEY2_Pin,
  .KeyDownCallback = &Key2_DownCallback,
};
Key_Handle_t Key3 = {
  .KeyGpioPort = KEY3_GPIO_Port,
  .KeyGpioPin = KEY3_Pin,
  .KeyDownCallback = &Key3_DownCallback,
};

Key_Handle_t* KeyHandleArr[KEY_NUM] = {&Key1, &Key2, &Key3};
volatile uint16_t KeyScanMask = 0xFFFF;  // 按键扫描标志位 0表示不扫描该按键
volatile uint16_t TaskExecFlags = 0x0000;
uint8_t IsRunningMacro = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void Key1_DownCallback(){
  __SET_BIT(TaskExecFlags, 0);
}

void Key2_DownCallback(){
  __SET_BIT(TaskExecFlags, 1);
}

void Key3_DownCallback(){
  __SET_BIT(TaskExecFlags, 2);
}

// 模拟短按电管按钮，阻止休眠
// 低电平有效，周期5秒，100ms低 + 4900ms高
void WakeupPMIC(){
  static uint16_t cnt = 0;
  cnt++;
  if(cnt < 100){
    HAL_GPIO_WritePin(PMIC_WKUP_GPIO_Port, PMIC_WKUP_Pin, 0);
  }
  else{
    HAL_GPIO_WritePin(PMIC_WKUP_GPIO_Port, PMIC_WKUP_Pin, 1);
  }
  if(cnt >= 5000){
    cnt = 0;
  }
}

/* 毫秒中断按键响应逻辑
 * 检测到某一按键按下时，将KeyScanMask的对应位置1，其它位为0，使程序不再响应其它按键
 * 若识别为误触，或者对应的键盘宏执行完毕，将KeyScanMask全部置1，响应所有按键
 * 若键盘宏执行过程中按键松开，通过IsRunningMacro标志阻止KeyScanMask全部置1，直到键盘宏执行完毕
*/
void HAL_IncTick(){
  uwTick += uwTickFreq;

  for(uint8_t i = 0; i < KEY_NUM; i++){
    if(__CHECK_BIT(KeyScanMask, i)){
      Key_Check(KeyHandleArr[i]);
    }

    if(!IsRunningMacro){
      if(KeyHandleArr[i]->KeyState != KEY_UP){
        KeyScanMask = 0x0000;
        __SET_BIT(KeyScanMask, i);
      }
      else{
        KeyScanMask = 0xFFFF;
      }
    }
  }

  WakeupPMIC();
}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(PMIC_WKUP_GPIO_Port, PMIC_WKUP_Pin, 1);

  HAL_Delay(10);
  for(uint8_t i = 0; i < KEY_NUM; i++){
    Key_Init(KeyHandleArr[i]);
  }

  RobotType = (HAL_GPIO_ReadPin(TYPE_SEL1_GPIO_Port, TYPE_SEL1_Pin) << 1) | HAL_GPIO_ReadPin(TYPE_SEL0_GPIO_Port, TYPE_SEL0_Pin);
  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // KEY1 基地买弹
    if(__CHECK_BIT(TaskExecFlags, 0)){
      __CLEAR_BIT(TaskExecFlags, 0);
      IsRunningMacro = 1;
      KeyScanMask = 0x00;
      if(RobotType == HERO){
        AAE_BuyAmmoBase_Hero();
      }
      else{
        AAE_BuyAmmoBase_Infantry();
      }
      KeyScanMask = 0xFF;
      IsRunningMacro = 0;
    }

    // KEY2 远程买弹
    if(__CHECK_BIT(TaskExecFlags, 1)){
      __CLEAR_BIT(TaskExecFlags, 1);
      IsRunningMacro = 1;
      KeyScanMask = 0x00;
      if(RobotType == HERO){
        AAE_BuyAmmoRemote_Hero();
      }
      else{
        AAE_BuyAmmoRemote_Infantry();
      }
      KeyScanMask = 0xFF;
      IsRunningMacro = 0;
    }

    // KEY3 远程买血
    if(__CHECK_BIT(TaskExecFlags, 2)){
      __CLEAR_BIT(TaskExecFlags, 2);
      IsRunningMacro = 1;
      KeyScanMask = 0x00;
      AAE_BuyHP();
      KeyScanMask = 0xFF;
      IsRunningMacro = 0;
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
