/*

* Author: Texas Instruments

* Editted by: Parin Chheda
          ERTS Lab, CSE Department, IIT Bombay

* Description: This code structure will assist you in completing Lab 1
* Filename: lab-1.c

* Functions: setup(), config(), main()


*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include <time.h>


/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency,enable GPIO Peripherals and unlock Port F pin 0 (PF0)

* Example Call: setup();

*/
void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //unlock PF0 based on requirement
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0;
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


    //--------------   Part - 2   --------------//

    //

    uint8_t val = 0x10, sw2status = 0;

    while(1)
    {
        // Read the state of Switch 2 (PF4)
        val = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);

        if ((val & 0x10) == 0x00)
        {
            sw2status = sw2status + 1;

            while((val & 0x10) == 0x00)
            {
                // Read the state of Switch 2 (PF4)
                val = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
            }
        }
    }

    //

}
