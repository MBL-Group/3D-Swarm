/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "sac6010.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */



float theta = 0;

float current_set[8] = {0,0,0,0,0,0,0,0};
float amp[8] = {0,0,0,0,2,2,2,2};
float amp_k = 1.0;

int time_cnt = 0;
int main_loop_cnt = 0;

int enable_pin = 0;
uint32_t pwm_pulse[8] = {0,0,0,0,0,0,0,0};

float rotated_freq = 1.0;

int control_mode = 1; // 0: Normal, 1: Periodic

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void generate_sin_table(float* table,int N, float A);
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
  MX_USART1_UART_Init();
	MX_USB_DEVICE_Init();
  
  MX_TIM10_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  //MX_TIM11_Init();
  /* USER CODE BEGIN 2 */

	
	// 60MHz/20 = 3Mhz, 3000000hz/1000 = 3000Hz
	MX_TIM10_Init_Freq(20-1,1000);
	HAL_TIM_Base_Start_IT(&htim10);  
	
	// PWM init
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//CDC_Transmit_FS("USB SYSTEM ON\r\n",15);
		

		HAL_Delay(1);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 5;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&htim10))
    {
			if (control_mode == 1) // periodic mode
			{
				if(main_loop_cnt>(3000/rotated_freq))
				{
					main_loop_cnt = 0;
				}
				
				theta = -main_loop_cnt/(3000/rotated_freq) * 3.14159265359f*2;
				
				current_set[0] = amp_k*amp[0]*cos(theta);
				current_set[2] = -amp_k*amp[2]*cos(theta);
				
				current_set[1] = amp_k*amp[1]*sin(theta);
				current_set[3] = -amp_k*amp[3]*sin(theta);
				
				current_set[4] = -amp_k*amp[4]*cos(theta);
				current_set[6] = amp_k*amp[6]*cos(theta);
				
				current_set[5] = -amp_k*amp[5]*sin(theta);
				current_set[7] = amp_k*amp[7]*sin(theta);			
			}
			
			pwm_pulse[0] = calculate_current_to_pwm(current_set[0],10,20000);
			pwm_pulse[1] = calculate_current_to_pwm(current_set[1],10,20000);
			pwm_pulse[2] = calculate_current_to_pwm(current_set[2],10,20000);
			pwm_pulse[3] = calculate_current_to_pwm(current_set[3],10,20000);
			pwm_pulse[4] = calculate_current_to_pwm(current_set[4],10,20000);
			pwm_pulse[5] = calculate_current_to_pwm(current_set[5],10,20000);
			pwm_pulse[6] = calculate_current_to_pwm(current_set[6],10,20000);
			pwm_pulse[7] = calculate_current_to_pwm(current_set[7],10,20000);	
			
			
			if (enable_pin == 0)
			{
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
				htim2.Instance->CCR1 = 0;
				htim2.Instance->CCR2 = 0;
				htim2.Instance->CCR3 = 0;
				htim2.Instance->CCR4 = 0;
				htim3.Instance->CCR1 = 0;
				htim3.Instance->CCR2 = 0;
				htim3.Instance->CCR3 = 0;
				htim3.Instance->CCR4 = 0;					
			}
			else
			{
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);		
				htim2.Instance->CCR1 = pwm_pulse[0];
				htim2.Instance->CCR2 = pwm_pulse[1];
				htim2.Instance->CCR3 = pwm_pulse[2];
				htim2.Instance->CCR4 = pwm_pulse[3];
				htim3.Instance->CCR1 = pwm_pulse[4];
				htim3.Instance->CCR2 = pwm_pulse[5];
				htim3.Instance->CCR3 = pwm_pulse[6];
				htim3.Instance->CCR4 = pwm_pulse[7];
			}				
			main_loop_cnt +=1;
				
			time_cnt += 1;
    }
		else if(htim == (&htim11))
		{
		}
}


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

#ifdef  USE_FULL_ASSERT
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
