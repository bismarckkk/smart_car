/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structoperation.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define RXBUFFERSIZE  256

char RxBuffer[RXBUFFERSIZE];
uint8_t aRxBuffer;
uint8_t Uart1_Rx_Cnt = 0;
uint16_t direction = 0;
char ad_flag = 1;
char ccd_flag = 0;
char zhijiao = 0;
uint8_t ccd_s[128];
uint8_t ccd_p[128];
uint16_t ccd_count = 0;
uint16_t ccd_SI = 5000;

uint8_t iii = 0;

void send(int16_t*, uint8_t);
void ccd_process(void);
void send_ccd(void);
void PutChar(unsigned char data);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
	MX_TIM1_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
	//定时器和串口初始化
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim7);
	
	HAL_GPIO_WritePin(SI_GPIO_Port, SI_Pin, 0);
	HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 0);
	HAL_GPIO_WritePin(PWM2_GPIO_Port, PWM2_Pin, 0);
	HAL_GPIO_WritePin(PWM2_GPIO_Port, PWM4_Pin, 0);
	//printf("start\n");
	
	for(;iii < 128; iii++)
	{
		ccd_s[iii] = 0;
	}
	
  /* USER CODE END 2 */
	
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		
    /* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		//printf("test");
		/* 把这个留在这里，ADC启动转换使用
		if(ad_flag)
		{
			HAL_ADC_Start_IT(&hadc2);
			ad_flag = 0;
		}
		*/
		//int16_t data[2] = {10,20};
		//data[0] = direction;
		//send(data, 2);
		/*
		//__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, direction);
		while (direction< 113)
	  {
			
		  direction++;
			int16_t data[2] = {10,20};
			data[0] = direction;
			send(data, 2);
		  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, direction);    //?????,?????
//		  TIM3->CCR1 = pwmVal;    ?????
		  HAL_Delay(20);
	  }
	  while (direction > 23)
	  {
		  direction--;
			int16_t data[2] = {10,20};
			data[0] = direction;
			send(data, 2);
		  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, direction);    //?????,?????
//		  TIM3->CCR1 = pwmVal;     ?????
		  HAL_Delay(20);
		}*/
		HAL_Delay(50);
		send_ccd();
		//printf("\ntt\n");
    
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
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void SendHex(unsigned char data)
{
	unsigned char temp;
	temp = data >> 4;
	if(temp >= 10)
	{
		PutChar(temp - 10 + 'A');
	}
	else
	{
		PutChar(temp + '0');
	}
	temp = data & 0x0f;
	if(temp >= 10)
	{
		PutChar(temp - 10 + 'A');
	}
	else
	{
		PutChar(temp + '0');
	}
}

void send_ccd()
{
	uint8_t dataa[150], *data = dataa;
	int len;
	unsigned char lrc=0;
	dataa[0] = 0;
	dataa[1] = 132;
	dataa[2] = 0;
	dataa[3] = 0;
	dataa[4] = 0;
	dataa[5] = 0;
	for(len = 0; len < 128; len++)
	{
		dataa[len + 6] = ccd_s[len];
	}
	PutChar('*'); // 发送帧头，一个字节
	len = (int)(data[0]<<8) | (int)(data[1]) ;
	data += 2; // 调整指针
	PutChar('L'); // 发送帧类型，共两个字节
	PutChar('D');
	while(len--) // 发送数据的ASCII码，含保留字节和CCD数据
	{
		SendHex(*data);
		lrc += *data++;
	}
	lrc = 0-lrc; // 计算CRC，可以为任意值
	SendHex(lrc); // 发送CRC校验ASCII
	PutChar('#'); // 发送帧尾，一个字节
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
	struct anotext send;
	char ss[2], *sss;
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if(Uart1_Rx_Cnt >= 255)
	{
		Uart1_Rx_Cnt = 0;
		memset(RxBuffer,0x00,sizeof(RxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t *)"数据溢出", 10,0xFFFF); 	
        
	}
	else
	{
		RxBuffer[Uart1_Rx_Cnt++] = aRxBuffer;
		//�����Ǵ�����Ϣ�������������޸�
		/*
		if((RxBuffer[Uart1_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart1_Rx_Cnt-2] == 0x0D))
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart1_Rx_Cnt,0xFFFF);
            while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
			Uart1_Rx_Cnt = 0;
			memset(RxBuffer,0x00,sizeof(RxBuffer));
		}
		*/
		struct anotext t = char2ano(RxBuffer);
		if(t.head == 0xaaaf && t.sum == Uart1_Rx_Cnt)
		{
			uint8_t i;
			send.head = 0xaaaa;
			send.name = 0xef;
			send.len = 2;
			ss[0] = t.name;
			ss[1] = t.sum;
			send.data = ss;
			send.sum = 0;
			sss = ano2char(send);
			for(i = 0; i < 6; i++)
				sss[6] = sss[i];
			HAL_UART_Transmit(&huart1, (uint8_t *)sss, 7,0xFFFF);
			free(sss);
			delstr(send);
			
			//写入数据部分写在下面
		}
	}
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
}

void send(int16_t *in, uint8_t n)
{
	uint8_t i;
	struct anotext send;
	send.head = 0xaaaa;
	send.name = 0xf3;
	send.len = n * 2;
	send.data = malloc(n * 2 + 1);
	send.data[n * 2] = 0;
	short2char(send.data, in, n * 2);
	send.sum = 0;
	char *ss = ano2char(send);
	for(i = 0; i < n * 2 + 4; i++)
		ss[n * 2 + 4] += ss[i];
	HAL_UART_Transmit(&huart1, (uint8_t *)ss, n * 2 + 5,0xFFFF);
	free(ss);
	delstr(send);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == (&htim4))
  {
	//定时器4中断函数
		
		if(ccd_count == ccd_SI)
		{
			ccd_count = 0;
			HAL_GPIO_WritePin(SI_GPIO_Port, SI_Pin, 1);
		}
		if(ccd_count == 1)
		{
			HAL_GPIO_WritePin(SI_GPIO_Port, SI_Pin, 0);
		}
		if(ccd_flag)
		{
			HAL_GPIO_WritePin(SI_GPIO_Port, SI_Pin, 0);
			ccd_flag = 0;
			ccd_count++;
			if(ccd_count <= 128)
			{
				if(ad_flag)
				{
					HAL_ADC_Start_IT(&hadc2);
					ad_flag = 0;
				}
				else
				{
					ccd_s[ccd_count - 1] = 127;
				}
			}
			else if(ccd_count == 130)
			{
				//void ccd_process();
			}
		}
		else
		{
			HAL_GPIO_WritePin(SI_GPIO_Port, SI_Pin, 1);
			ccd_flag = 1;
		}
	
  }
		else if (htim == (&htim7))
    {
        //定时器7中断函数
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	ad_flag = 1;
	//HAL_ADC_Stop_IT(&hadc2); 
	ccd_s[ccd_count - 1] = HAL_ADC_GetValue(&hadc2);
}

void PutChar(unsigned char data)
{
	unsigned char *p = &data;
	HAL_UART_Transmit(&huart1, (uint8_t *)p, 1, 0xffff);
}

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

void ccd_process()
{
	uint8_t max, min, i = 5, yuzhi;
	for(max = ccd_s[i], min = ccd_s[i]; i < 123; i++)
	{
		if(ccd_s[i] > max)
			max = ccd_s[i];
		if(ccd_s[i] < min)
			min = ccd_s[i];
	}
	yuzhi = (max + min) / 2;
	for(i = 0; i < 128; i++)
	{
		if(ccd_s[i] < yuzhi)
			ccd_p[i] = 0;
		else
			ccd_p[i] = 1;
	}
}

int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
