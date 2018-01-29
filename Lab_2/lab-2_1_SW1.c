/*

* Author: Texas Instruments

* Created by: Parin Chheda
          ERTS Lab, CSE Department, IIT Bombay

* Edited by: Kalind Karia
          ERTS Lab, CSE Department, IIT Bombay

* Description: This code structure will assist you in completing Lab 2 Part 1

* Filename: lab-2_1_SW1.c

* Functions: setup(), config(), main()

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"


// Declaration of some global variables

static uint8_t flag = 0;
uint8_t ui8LED = 2;

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency, enable GPIO and Timer 0 Peripherals, unlock Port F pin 0 (PF0)
               and Configure Timer 0 as 32-bit Timer in Periodic Mode

* Example Call: setup();

*/
void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Enable Clock to the Peripheral -- Port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //unlock PF0 based on requirement
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0;

    // Enable Clock to the Peripheral -- Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configure Timer 0 as 32-bit Timer in Periodic Mode
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
}

/*

* Function Name: pin_config()

* Input: none

* Output: none

* Description: Set Port F Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected.
               Set Port F Pin 0 and 4 as input, enable pull up on both these pins.

* Example Call: pin_config();

*/

void pin_config(void)
{
    // PF1, PF2, PF3 == R, G, B pins of RGB LED configured as Output
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // PF0, PF4 == SW1, SW2 configured as Input and enabling Pull-up
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}


int main(void)
{
    setup();
    pin_config();


    //--------------   Part - 1 (SW 1)   --------------//

    //

    uint32_t ui32Period;

    uint8_t prev_cnt = 1, cnt = 1;

    // Number of cycles to read GPIO at 100 Hz, 50% duty cycle (every 10 ms)
    ui32Period = (SysCtlClockGet() / 100) / 2;

    // Load calculated period into Timer's Interval Load register
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

    // Enable specific vector associated with Timer 0A
    IntEnable(INT_TIMER0A);

    // Enable specific event within the Timer to generate an Interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Master Interrupt Enable
    IntMasterEnable();

    // Enable the Timer
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
        if (flag == 1)
        {
            if (prev_cnt == 3)
                prev_cnt = 0;

            cnt = prev_cnt + 1;

            if (cnt == 1)
            {
                ui8LED = 2; // R LED
            }

            else if (cnt == 2)
            {
                ui8LED = 8; // G LED
            }

            else if (cnt == 3)
            {
                ui8LED = 4; // B LED
            }
        }

        else
            prev_cnt = cnt;
    }

    //

}


/*

* Function Name: detectKeyPress()

* Input: none

* Output: flag whether key is pressed or not

* Description: ISR for Timer 0A Timeout

* Example Call: detectKeyPress();

*/

uint8_t detectKeyPress()
{
    static uint8_t idle = 1, press = 0, release = 0;

    // Clear the Timer Interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Read the current state of GPIO pin and enter respective states

    if (((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) & 0x10) == 0x00) && idle == 1)
    {
        // if SW2 is pressed in idle state (within first 10ms), then go to press state
        press = 1;
        idle = 0;
    }

    else if (((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) & 0x10) == 0x10) && press == 1)
    {
        // if SW2 is released in press state (within next 10ms), then go to idle state
        idle = 1;
        press = 0;
    }

    else if (((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) & 0x10) == 0x00) && press == 1)
    {
        // if SW2 is pressed in press state (within next 10ms), then go to release state
        release = 1;
        press = 0;
        flag = 1;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
    }

    else if (((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) & 0x10) == 0x10) && release == 1)
    {
        // if SW2 is released in release state (within next 10ms), then go to idle state
        idle = 1;
        release = 0;
        flag = 0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }

    return flag;
}
