/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>

#define START_CHAR        '#'
#define END_CHAR          '$'
#define MAX_FRAME_CHARS   35

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(uint8_t sign);


/* Space for your global variables. */

letter_count_ letter_count;
static char validString[MAX_FRAME_CHARS + 1];
static volatile uint8_t validStringReady = 0;
static char txBuffer[128];

static void waitTxIdle(void)
{
	while (LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_7) != 0);
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
}

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  /* Space for your local variables, callback registration ...*/

  USART2_RegisterCallback(proccesDmaData);

  while (1)
  {
	  /* Periodic transmission of information about DMA Rx buffer state.
	   * Transmission frequency - 0.5Hz.
	   * Message format - "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %f%"
	   * Example message (what I wish to see in terminal) - Buffer capacity: 1000 bytes, occupied memory: 231 bytes, load [in %]: 23.1%
	   */

	  /* Valid text string information transmission.
	   * Transmission frequency - when new valid string is received.
	   * Message format - "Valid string: %s, lower-case: %d, upper-case: %d"
	   * Example message (what I wish to see in terminal) - Valid string: Platn15uborZnakov, lower-case: 13, upper-case: 2
	   */

	  if (validStringReady)
	  {
		  validStringReady = 0;
		  waitTxIdle();
		  int len = snprintf(txBuffer, sizeof(txBuffer),
				  "Valid string: %s, lower-case: %d, upper-case: %d\r\n",
				  validString, letter_count.small_letter, letter_count.capital_letter);
		  USART2_PutBuffer((uint8_t *)txBuffer, (uint8_t)len);
	  }
  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/*
 * Implementation of function processing data received via USART.
 */
void proccesDmaData(uint8_t sign)
{
	/* Process received data */

		static uint8_t receiving = 0;
		static uint8_t count = 0;
		static uint8_t idx = 0;
		static char payload[MAX_FRAME_CHARS + 1];

		if (!receiving)
		{
			if (sign == START_CHAR) { receiving = 1; count = 0; idx = 0; }
			return;
		}

		if (sign == START_CHAR)
		{
			count = 0; idx = 0;
			return;
		}

		count++;

		if (sign == END_CHAR)
		{
			uint8_t lower = 0, upper = 0;
			for (uint8_t i = 0; i < idx; i++)
			{
				if (payload[i] >= 'a' && payload[i] <= 'z') lower++;
				else if (payload[i] >= 'A' && payload[i] <= 'Z') upper++;
			}
			payload[idx] = '\0';
			for (uint8_t i = 0; i <= idx; i++) validString[i] = payload[i];
			letter_count.small_letter = lower;
			letter_count.capital_letter = upper;
			validStringReady = 1;
			receiving = 0;
			return;
		}

		if (count >= MAX_FRAME_CHARS)
		{
			receiving = 0;
			return;
		}

		payload[idx++] = (char)sign;
}


void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
