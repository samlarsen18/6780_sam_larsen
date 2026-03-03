#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include "stm32f072xb.h"
#include "led.h"

void LED_Init()
{
    //LEDs
    GPIOC->MODER |= GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9);
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR6_0 | GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR8_0 | GPIO_OSPEEDER_OSPEEDR9_0);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7 | GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);

    // Push Button
    GPIOA->MODER &= ~GPIO_MODER_MODER0; 
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR0_0;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;
}



void LED_DeInit(void)
{
}




GPIO_PinState LED_Read(LED_t led)
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    switch(led)
    {
        case LED_RED:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_6;
            break;
        case LED_BLUE:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_7;
            break;
        case LED_ORANGE:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_8;
            break;
        case LED_GREEN:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_9;
            break;
        default:
            return 0; // Invalid LED
    }
    return (GPIOx->IDR & GPIO_Pin) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}



void LED_Write(LED_t led, GPIO_PinState PinState)
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    switch(led)
    {
        case LED_RED:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_6;
            break;
        case LED_BLUE:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_7;
            break;
        case LED_ORANGE:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_8;
            break;
        case LED_GREEN:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_9;
            break;
        default:
            return; // Invalid LED
    }
    if(PinState == GPIO_PIN_SET)
    {
        GPIOx->ODR |= GPIO_Pin; // Set the pin
    }
    else
    {
        GPIOx->ODR &= ~GPIO_Pin; // Clear the pin
    }
}


void LED_Toggle(LED_t led)
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    switch(led)
    {
        case LED_RED:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_6;
            break;
        case LED_BLUE:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_7;
            break;
        case LED_ORANGE:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_8;
            break;
        case LED_GREEN:
            GPIOx = GPIOC;
            GPIO_Pin = GPIO_PIN_9;
            break;
        default:
            return; // Invalid LED
    }
    GPIOx->ODR ^= GPIO_Pin; // Toggle the pin
}

