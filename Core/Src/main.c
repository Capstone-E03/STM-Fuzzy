#include "main.h"

/* Prototipe init modul (disediakan file .c di atas) */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_ADC1_Init(void);
void MX_USART1_UART_Init(void);

/* App layer (punyamu) */
void App_Run(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();

  /* masuk ke loop aplikasi – tidak kembali */
  App_Run();

  while (1) { }
}

/* ====== Clock: HSE 8MHz -> PLL -> SYSCLK 84MHz ====== */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* HSE ON, PLL ON */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM       = 8;   // 8MHz /8 = 1MHz (internal use)
  RCC_OscInitStruct.PLL.PLLN       = 336; // 1MHz *336 = 336MHz
  RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV4; // SYSCLK = 336/4 = 84MHz
  RCC_OscInitStruct.PLL.PLLQ       = 7;   // USB clock not used here
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                   | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;  // 84MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;    // 42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;    // 84MHz
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }

  /* SysTick 1ms */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* Default error handler */
void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}
