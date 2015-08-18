/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/



/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/


int main(void)
{
    ConfigureOscillator();
    ConfigurePPS();
    ConfigureLEDS();
    InitApp();
    ConfigureInt();

    while(1)
    {
        
    }
}