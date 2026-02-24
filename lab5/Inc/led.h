#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
typedef enum {
    LED_RED,
    LED_BLUE,
    LED_ORANGE,
    LED_GREEN,
} LED_t;

void LED_Init(void);
void LED_DeInit(void);
GPIO_PinState LED_Read(LED_t led);
void LED_Write(LED_t led, GPIO_PinState PinState);
void LED_Toggle(LED_t led);
