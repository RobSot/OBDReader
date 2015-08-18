/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include "system.h"          /* variables/params used by system.c             */
#include <uart.h>
#include <ecan.h>
#include <pps.h>


/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.c.                                          */
/*                                                                            */
/******************************************************************************/

/* Refer to the device Family Reference Manual Oscillator section for
information about available oscillator configurations.  Typically
this would involve configuring the oscillator tuning register or clock
switching useing the compiler's __builtin_write_OSCCON functions.
Refer to the C Compiler for PIC24 MCUs and dsPIC DSCs User Guide in the
compiler installation directory /doc folder for documentation on the
__builtin functions.*/


void ConfigureOscillator(void)
{
    /*Espera a que se realice el cambio de oscilador de FRC a PRI sin PLL
     Porque está  seleccionado el two speed start up*/

    while (OSCCONbits.COSC != OSCCONbits.NOSC);

    /*configurar PLL*/
    
    PLLFBDbits.PLLDIV = 68;   // M=70       /*******************/
    CLKDIVbits.PLLPRE = 2;    // N1=4       /***** 70 MIPS *****/
    CLKDIVbits.PLLPOST = 0;   // N2=2       /*******************/
    
    /*Hacer el switch de PRI a PRIPLL*/

    __builtin_write_OSCCONH(NOSC_PRIPLL);
    __builtin_write_OSCCONL(0x01);
    while (OSCCONbits.COSC!= NOSC_PRIPLL);    // Wait for Clock switch to occur
    while (OSCCONbits.LOCK!= 1);              // Wait for PLL to lock
}

void ConfigurePPS(void)
{
    PPSUnLock;

    /*UART1*/
    PPSInput(IN_FN_PPS_U1RX,IN_PIN_PPS_RPI44);          // Assign UART1 Receive (U1RX) to RPI44 pin
    PPSOutput(OUT_FN_PPS_U1TX,OUT_PIN_PPS_RP20);        // Assign UART1 Transmit (U1TX) to RP20 pin

    /*ECAN1*/
    PPSInput(IN_FN_PPS_C1RX,IN_PIN_PPS_RPI46);          // Assign ECAN1 Receive (C1RX) to RPI46 pin
    PPSOutput(OUT_FN_PPS_C1TX,OUT_PIN_PPS_RP37);        // Assign ECAN1 Transmit (C1TX) to RP37 pin

    PPSLock;
}

void ConfigureInt(void)
{
    INTCON1bits.NSTDIS = 1;     // Interrupt nesting Disabled
    INTCON2bits.GIE = 1;        // Enable interrupts
}