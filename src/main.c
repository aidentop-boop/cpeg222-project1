/* Aiden Topolski
   9/15/26
   main.c
   
   Bare metal CMSIS program for NUCLEO-F446ZE
   The user button cycles through leds
   RED to GREEN to BLUE to RED
   
   The  LEDS are controlled by GPIOB's BSRR reg
   The user button on PC13 is debounced*/


#include "stm32f4xx.h"


/* PIN DEFINITIONS*/

#define GREEN_PIN 0 //PB0
#define BLUE_PIN 7 //PB7
#define RED_PIN 14 //PB14

#define BTN_PIN 13 //PC13

#define LED_PORT GPIOB // leds
#define BTN_PORT GPIOC // btn


/* LED STATE DEFINITIONS */

#define RED_STATE 0
#define GREEN_STATE 1
#define BLUE_STATE 2


/* DELAY FUNCTION */
void delay(volatile uint32_t count)
{ 
    while(count--)
    {      
    }
}

/* Controlls which LED is turned on by BSRR */

void set_led(int state)
{
    if(state == RED_STATE)
    {
        //R ON G,B OFF
        LED_PORT->BSRR = 
            (1 << RED_PIN) |
            (1 << (GREEN_PIN + 16)) | // BSRR split into two 
            (1 << (BLUE_PIN +16));    // 16 bit halves, second half
    }                                 // is LOW, hence +16
    else if(state == GREEN_STATE)
    {
        //G ON, R,B OFF
        LED_PORT->BSRR =
            (1 << GREEN_PIN) |
            (1 << (BLUE_PIN + 16)) |
            (1 << (RED_PIN + 16));
    }
    else if(state == BLUE_STATE)
    {
        LED_PORT->BSRR =
            (1 << BLUE_PIN) |
            (1<< (GREEN_PIN + 16)) |
            (1<< (RED_PIN + 16));
    }
}

/* Enable GPIO clock, configure led's as outputs
   and Button as input */

void gpio_init(void)
{
    //enable clocks for GPIOB and GPIOC
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // PB0 green LED -> output
    LED_PORT->MODER &= ~(0x3 << (GREEN_PIN * 2)); //shift 11, invert, clear (AND)
    LED_PORT->MODER |=  (0x1 << (GREEN_PIN * 2)); //shift 01, OR to set to 01 (output)

    // PB7 blue LED -> output
    LED_PORT->MODER &= ~(0x3 << (BLUE_PIN * 2));
    LED_PORT->MODER |=  (0x1 << (BLUE_PIN * 2));

    // PB14 red LED -> output
    LED_PORT->MODER &= ~(0x3 << (RED_PIN * 2));
    LED_PORT->MODER |=  (0x1 << (RED_PIN * 2));

    //PC13 USER btn -> input
    BTN_PORT->MODER &= ~(0x3 << (BTN_PIN * 2));
}
    /* Main function, starts on red, changes state 0-1-2-0... after each press */
int main(void)
{ 
    int led_state = RED_STATE;

    gpio_init(); //calls gpio initialization func to start up the I/O's

    //Start w/ red LED on
    set_led(led_state);

    while (1) 
    {
        //check if button pressed
        if (BTN_PORT->IDR & (1 << BTN_PIN))
        {
            //debounce delay
            delay(125000);

            //wait until btn is released
            while (BTN_PORT->IDR & (1<<BTN_PIN))
            {
            }

            led_state++; //next LED

            if (led_state > BLUE_STATE) 
            {
                led_state = RED_STATE;
            }
            set_led(led_state);
        }
    }
    
    return 0;
}