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


    //--------------   Part - 1   --------------//

    //

    uint8_t ui8LED = 0, cnt = 0, sw1status = 0x01;

    while(1)
    {
        // Read the state of Switch 1 (PF0)
        sw1status = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);

        if ((sw1status & 0x01) == 0x00)
        {
            cnt = cnt + 1;

            while(((sw1status & 0x01) == 0x00) && cnt == 1)
            {
                ui8LED = 2; // R LED
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

                // Read the state of Switch 1 (PF0)
                sw1status = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
            }

            while(((sw1status & 0x01) == 0x00) && cnt == 2)
            {
                ui8LED = 8; // G LED
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

                // Read the state of Switch 1 (PF0)
                sw1status = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
            }

            while(((sw1status & 0x01) == 0x00) && cnt == 3)
            {
                ui8LED = 4; // B LED
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

                // Read the state of Switch 1 (PF0)
                sw1status = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
            }

            if (cnt == 3)
                cnt = 0;
        }

        else
        {
            ui8LED = 0; // OFF LEDs
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
        }
    }

    //

}
