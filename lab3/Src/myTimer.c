#include "myTimer.h"
#include "stm32f072xb.h"

void myTimerInit(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    
    // 4Hz timer
    TIM2->PSC = 8000 - 1;   // 1ms
    TIM2->ARR = 250;        // 250ms (4Hz)

    TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt
}

void myTimerStart(void)
{
    TIM2->CR1 |= TIM_CR1_CEN; 
}