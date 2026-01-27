#include "main.h"
#include "stdint.h"
#include "stm32f0xx_hal.h"
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_rcc.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f072xb.h>
#include <assert.h>
#include "hal_gpio.h"

void SystemClock_Config(void);

void HAL_RCC_GPIOC_CLK_Enable()
{
  RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;
}
void HAL_RCC_GPIOA_CLK_Enable()
{
  RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
}

int main(void)
{
  HAL_Init();           // Reset of all peripherals, init the Flash and Systick
  SystemClock_Config(); // Configure the system clock

  HAL_RCC_GPIOC_CLK_Enable();
  HAL_RCC_GPIOA_CLK_Enable();

  My_HAL_GPIO_Init(NULL, NULL);
  assert(GPIOC->MODER & (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0));

  uint32_t debouncer = 0;
  while(1) {
    debouncer = (debouncer << 1); // Always shift every loop iteration
    if (My_HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) { // If input signal is set/high
      debouncer |= 0x01; // Set lowest bit of bit-vector
    }
    if (debouncer == 0xFFFFFFFF) {

    }
    if (debouncer == 0x00000000) {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
    }
    if (debouncer == 0x7FFFFFFF) {
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
      My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
    }
  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* User can add their own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
  /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
