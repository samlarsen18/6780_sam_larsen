#include "main.h"
#include "stm32f0xx_hal.h"
#include "stm32f072xb.h"
#include "stm32f0xx_hal_rcc.h"
#include "string.h"
#include "hal_gpio.h"

// USART3 Tx: PC4, PB10, PC10
// USART3 Rx: PC5, PB11, PC11

// Use UART3. Pins PC4 (tx) and PC5 (rx)
void initialize_UART(void){
  GPIOC->AFR[0] |= (GPIO_AF1_USART3 << GPIO_AFRL_AFSEL4_Pos) | (GPIO_AF1_USART3 << GPIO_AFRL_AFSEL5_Pos); // PC4 and PC5 to AF1
  GPIOC->MODER |= GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1; // Alternate function mode for PC4 and PC5
  GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_5); // Push-pull output
  GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR4_0 | GPIO_OSPEEDER_OSPEEDR5_0); // Low speed
  GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR5); // No pull-up/pull-down

  
  
  RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enable USART3 clock

  USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable Transmitter and Receiver
  USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200; // Set baud rate to 115200
  USART3->CR1 |= USART_CR1_UE; // Enable USART
  USART3->CR1 |= USART_CR1_RXNEIE; // Enable RXNE interruptq

}

void rcc_init(){
  RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;
  RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
}

void tx_char(char c) {
  while((USART3->ISR & USART_ISR_TXE) == 0) { }
  USART3->TDR = c; // Transmit character
  HAL_Delay(1);
}

void tx_string(char * s, uint8_t len) {
  while(len > 0 && *s != '\0') {
    tx_char(*s++);
    len--;
  }
}

char rx_char(void) {
  while((USART3->ISR & USART_ISR_RXNE) == 0) { }
  return USART3->RDR; // Read received character
}

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */

char g_led = 0;
uint8_t g_led_state = 0;
uint8_t g_cmd_received = 0;

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  rcc_init();
  /* Configure the system clock */
  SystemClock_Config();
  initialize_UART();
  My_HAL_GPIO_Init(NULL, NULL);
  
  NVIC_EnableIRQ(USART3_4_IRQn);
  NVIC_SetPriority(USART3_4_IRQn, 3);  

  char * string = "Hello!\r\n";
  tx_string(string, strlen(string));
  tx_string("CMD: \r\n", 7);
  while(1) {
    if(g_cmd_received) {
      uint16_t pin = 0;
      switch(g_led) {
        case 'r':
          pin = GPIO_PIN_6;
          break;
        case 'g':
          pin = GPIO_PIN_9;
          break;
        case 'b':
          pin = GPIO_PIN_7;
          break;
        case 'o':
          pin = GPIO_PIN_8;
          break;
        default:
          tx_string("Invalid LED: ", 16);
          tx_char(g_led);
          tx_string("\r\n", 2);
          break;
      }
      if (pin > 0){
        switch(g_led_state){
          case '0':
            My_HAL_GPIO_WritePin(GPIOC, pin, GPIO_PIN_RESET);
            break;
          case '1':
            My_HAL_GPIO_WritePin(GPIOC, pin, GPIO_PIN_SET);
            break;
          case '2':
            My_HAL_GPIO_TogglePin(GPIOC, pin);
            break; 
          default:
            tx_string("Invalid LED state: ", 20);
            tx_char(g_led_state);
            tx_string("\r\n", 2);
        }
      }
      g_cmd_received = 0;
      tx_string("CMD: \r\n", 7);
    }
  }

  return -1;
}


void USART3_4_IRQHandler(void)
{
  static uint8_t state = 0;
  if((USART3->ISR & USART_ISR_RXNE) != 0) {
    switch(state){
      case 0:
        g_led = USART3->RDR;
        state = 1;
        break;
      case 1:
        g_led_state = USART3->RDR;
        state = 0;
        g_cmd_received = 1;
      default:
        state = 0;
        break;
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
