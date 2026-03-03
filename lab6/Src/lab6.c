#include "main.h"
#include "stm32f0xx_hal.h"
#include "led.h"
#include "stm32f0xx_hal_gpio.h"

void SystemClock_Config(void);

void rcc_init(){
  RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;
  RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
  RCC->AHBENR  |= RCC_AHBENR_GPIOBEN;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  rcc_init();
  LED_Init();

  GPIOC->MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER3_0; // Analog function mode 
  RCC->APB2ENR  |= RCC_APB2ENR_ADCEN; 
  ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_RES_1; 
  ADC1->CHSELR |= ADC_CHSELR_CHSEL13;
  
  ADC1->CR &= ~ADC_CR_ADEN; 
  ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
  ADC1->CR |= ADC_CR_ADCAL; 
  while (ADC1->CR & ADC_CR_ADCAL);

  LED_Write(LED_RED, GPIO_PIN_RESET);
  LED_Write(LED_GREEN, GPIO_PIN_RESET);
  LED_Write(LED_BLUE, GPIO_PIN_RESET);
  LED_Write(LED_ORANGE, GPIO_PIN_RESET);

  ADC1->CR |= ADC_CR_ADEN; // Enable ADC
  while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait until ADC is ready 
  ADC1->CR |= ADC_CR_ADSTART;


  while (1)
  {
    // Wait for end-of-conversion before reading
    while (!(ADC1->ISR & ADC_ISR_EOC));
    uint8_t adc_value = (uint8_t)ADC1->DR;
    if (adc_value < 64) {
      LED_Write(LED_RED, GPIO_PIN_SET); 
      LED_Write(LED_GREEN, GPIO_PIN_RESET);
      LED_Write(LED_BLUE, GPIO_PIN_RESET);
      LED_Write(LED_ORANGE, GPIO_PIN_RESET);
    } else if (adc_value < 128) {
      LED_Write(LED_RED, GPIO_PIN_RESET);
      LED_Write(LED_GREEN, GPIO_PIN_SET);
      LED_Write(LED_BLUE, GPIO_PIN_RESET);
      LED_Write(LED_ORANGE, GPIO_PIN_RESET);
    } else if (adc_value < 192) {
      LED_Write(LED_RED, GPIO_PIN_RESET);
      LED_Write(LED_GREEN, GPIO_PIN_RESET);
      LED_Write(LED_BLUE, GPIO_PIN_SET);
      LED_Write(LED_ORANGE, GPIO_PIN_RESET);
    } else {
      LED_Write(LED_RED, GPIO_PIN_RESET);
      LED_Write(LED_GREEN, GPIO_PIN_RESET);
      LED_Write(LED_BLUE, GPIO_PIN_RESET);
      LED_Write(LED_ORANGE, GPIO_PIN_SET);
    }
    HAL_Delay(100); // Delay for stability
  }
  return -1;
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
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
