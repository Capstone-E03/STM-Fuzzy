#include "main.h"

/* Enable GPIO clocks dan set PA5 sebagai input pull-up (DHT DATA).
   Pin analog & AF untuk ADC/UART dikerjakan di MSP init masing-masing peripheral. */
void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* PA5 (DHT DATA) initial: input pull-up.
     Driver DHT akan mengubah mode ke OUTPUT_OD saat start signal. */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin  = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
