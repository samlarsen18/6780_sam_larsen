#include "stm32f0xx_hal.h"
#include "i2c.h"

void i2c_init() {
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    // PB11, PB13 - alternate function, PB14 - output, PB15 - input
    GPIOB->MODER &= ~(GPIO_MODER_MODER11_Msk | GPIO_MODER_MODER13_Msk);
    GPIOB->MODER |= GPIO_MODER_MODER11_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_0;
    GPIOB->MODER &= ~GPIO_MODER_MODER15_0;

    GPIOB->OTYPER |= GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_13;
    GPIOC->MODER |= GPIO_MODER_MODER0_0;
    
    GPIOB->AFR[1] &= ~((0xF << GPIO_AFRH_AFSEL11_Pos) | (0xF << GPIO_AFRH_AFSEL13_Pos));
    GPIOB->AFR[1] |= (1 << GPIO_AFRH_AFSEL11_Pos) | (5 << GPIO_AFRH_AFSEL13_Pos);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

    I2C2->TIMINGR = (1 << I2C_TIMINGR_PRESC_Pos)   | 
                    (0x13 << I2C_TIMINGR_SCLL_Pos) |
                    (0xF << I2C_TIMINGR_SCLH_Pos)  |
                    (2 << I2C_TIMINGR_SDADEL_Pos)  | 
                    (4 << I2C_TIMINGR_SCLDEL_Pos);
    
    I2C2->CR1 |= I2C_CR1_PE;
    /* Delay to allow I2C peripheral to stabilize after enabling */
    for (volatile int i = 0; i < 100; i++);
}

uint8_t i2c_read_byte(uint8_t address, uint8_t reg)
{
    I2C2->CR2 = (address << 1) | 
                (1 << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_START;

    while (!((I2C2->ISR & I2C_ISR_TXIS) || (I2C2->ISR & I2C_ISR_NACKF)));
    
    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR |= I2C_ICR_NACKCF;
        return 0xFF; // ERROR
    }
    
    I2C2->TXDR = reg;
    
    while (!((I2C2->ISR & I2C_ISR_TC)));
    
    I2C2->CR2 = (address << 1) | 
                (1 << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_RD_WRN |
                I2C_CR2_START;
    
    while (!((I2C2->ISR & I2C_ISR_RXNE) || (I2C2->ISR & I2C_ISR_NACKF)));
    
    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR |= I2C_ICR_NACKCF;
        return 0xFF; // ERROR
    }

    while (!((I2C2->ISR & I2C_ISR_TC)));
    
    uint8_t data = I2C2->RXDR;

    I2C2->CR2 |= I2C_CR2_STOP;
    
    return data;
}

void i2c_write_byte(uint8_t address, uint8_t reg, uint8_t data)
{
    I2C2->CR2 = (address << 1) | 
                (2 << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_START;

    while (!((I2C2->ISR & I2C_ISR_TXIS) || (I2C2->ISR & I2C_ISR_NACKF)));
    
    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR |= I2C_ICR_NACKCF;
        // ERROR
        return;
    }
    
    I2C2->TXDR = reg;
    
    while (!((I2C2->ISR & I2C_ISR_TXIS) || (I2C2->ISR & I2C_ISR_NACKF)));

    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR |= I2C_ICR_NACKCF;
        // ERROR
        return;
    }

    I2C2->TXDR = data;

    while (!((I2C2->ISR & I2C_ISR_TC)));
    
    I2C2->CR2 |= I2C_CR2_STOP;
}