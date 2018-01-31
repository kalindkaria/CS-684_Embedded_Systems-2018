/*

* Author: Texas Instruments

* Created by: Parin Chheda
          ERTS Lab, CSE Department, IIT Bombay

* Edited by: Kalind Karia
          ERTS Lab, CSE Department, IIT Bombay

* Description: This code structure will assist you in completing Lab 3 Part 1

* Filename: lab-3_Trial_Servo_PWM.c

* Functions: setup(), config(), main()

*/


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define PWM_FREQUENCY 55

int main(void)
{
    volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint8_t ui8Adjust;

    ui8Adjust = 83;     // Center position to create 1.5 mS pulse from PWM

    /*
     *
     * To reduce the code size, use ROM versions of the APIs
     *
     * ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
     *
     */

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    // Configure the clock frequency to the PWM module as a division of the system clock
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the peripherals for PWM1, GPIOD and GPIOF modules
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure the Port D Pin 0 (PD0) as PWM output pin for Module 1, PWM generator 0
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PD0_M1PWM0);

    // Unlock the GPIO commit control register
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    // PF0 and PF4 configured as Input
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);

    // PF0 and PF4 Pull-up enabled
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Setting the PWM Clock to 625 kHz (40 MHz / 64)
    ui32PWMClock = SysCtlClockGet() / 64;

    // Divide the PWM Clock by desired frequency to determine the count to be loaded in the Load register
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

    // Configure module 1 PWM generator 0 as a down-counter and load the count value
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);

    // PWM module 1, generator 0 enabled as an output and enabled to run
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);

    // Control the Servo angle using switches

    while(1)
    {
        // Decrement the adjust variable on SW1 press by unless it reaches the lower 1.0 mS limit
        if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
        {
            ui8Adjust--;
            if (ui8Adjust < 56)
            {
                ui8Adjust = 56;     // Left position
            }
            // Load the PWM pulse width register with the new value
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
        }

        // Increment the adjust variable on SW2 press by unless it reaches the upper 2.0 mS limit
        if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
        {
            ui8Adjust++;
            if (ui8Adjust > 111)
            {
                ui8Adjust = 111;    // Right position
            }
            // Load the PWM pulse width register with the new value
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
        }

        SysCtlDelay(100000);
    }

}


