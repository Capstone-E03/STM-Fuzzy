/* USER CODE BEGIN Includes */
#include "app_config.h"
void App_Run(void);
/* USER CODE END Includes */

int main(void) {
  HAL_Init();
//  SystemClock_Config();
//  MX_GPIO_Init();
//  MX_ADC1_Init();
//  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  App_Run();  // loop di dalam
  /* USER CODE END 2 */

  while (1) { } // tidak akan sampai sini
}
