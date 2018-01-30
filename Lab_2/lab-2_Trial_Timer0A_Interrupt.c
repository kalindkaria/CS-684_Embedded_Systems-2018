/*

* Author: Texas Instruments

* Created by: Parin Chheda
          ERTS Lab, CSE Department, IIT Bombay

* Edited by: Kalind Karia
          ERTS Lab, CSE Department, IIT Bombay

* Description: This code structure will assist you in completing Lab 2 Part 1

* Filename: lab-2_Trial_Timer0A_Interrupt.c

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


    //--------------   Part - 1   --------------//

    //

    uint32_t ui32Period;

    // Number of cycles to toggle GPIO at 10 Hz, 50% duty cycle
    ui32Period = (SysCtlClockGet() / 10) / 2;

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

    }

    //

}


/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: none

* Description: ISR for Timer 0A Timeout

* Example Call: pin_config();

*/

void Timer0IntHandler()
{
    // Clear the Timer Interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Read the current state of GPIO pin and
    // write back the opposite state

    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }

    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
    }
}
