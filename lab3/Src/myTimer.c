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

    TIM3->PSC = 10 - 1;  // 800 kHz (1.25us)
    TIM3->ARR = 1000;    // 1.25 (us) * 1000 = 1.25ms (800Hz)   
    TIM3->CCMR1 |= (0b111 << TIM_CCMR1_OC1M_Pos); // PWM mode 2
    TIM3->CCMR1 |= (0b110 << TIM_CCMR1_OC2M_Pos); // PWM mode 1
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE; // Enable preload for OC1 and OC2

    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; // Enable output for OC1 and OC2

    TIM3->CCR1 = 100; // 20% duty cycle for OC1
    TIM3->CCR2 = 100; // 20% duty cycle for OC2
}

void myTimerStartTIM2(void)
{
    TIM2->CR1 |= TIM_CR1_CEN; 
}

void myTimerStartTIM3(void)
{
    TIM3->CR1 |= TIM_CR1_CEN; 
}