/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */
#include "user.h"
#include "OBD.h"
#include "CAN.h"
#include "UART.h"


/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/


void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)  //CAN DMA Sent buffer
{ 
    IFS0bits.DMA0IF = 0;			// Clear the DMA0 Interrupt Flag
}


void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)  //CAN DMA Received buffer
{
    /* Aqui es donde mandamos el mensaje UART recibido de CAN*/
    can_t canFrame;

    CANRXLED = ON;                                              // Blink Rx LED
    CANRXLEDBLINK;                                              // Blink Rx LED

    canFrame = ReadCANFrame(CANRXBUFFi);
    OnCanRx(canFrame);

    IFS0bits.DMA1IF = 0;			                // Clear the DMA1 Interrupt Flag
}


void __attribute__((interrupt, no_auto_psv)) _DMA2Interrupt(void)  // UART DMA Sent buffer
{     
    IFS1bits.DMA2IF = 0;			// Clear the DMA0 Interrupt Flag;
}


void __attribute__((interrupt, no_auto_psv)) _DMA3Interrupt(void)  //UART DMA Received buffer
{
    /* Aqui es donde mandamos el mensaje CAN recibido de la UART */

    static unsigned int PinPonToggle = 0;           // Se inicializa solo la primera vez

    /* Pin Pon between UART BuffA & UART BuffB */

    if(PinPonToggle == 0)  // RxBufferA has been received
    {
        OnUartRx(URxBuffA);
    }
    else  // RxBufferB has been received
    {
        OnUartRx(URxBuffB);
    }
    PinPonToggle ^= 1;                                  // toggle the Buffer (A or B)
    IFS2bits.DMA3IF = 0;                                // Clear the DMA0 Interrupt Flag
}


void __attribute__((interrupt,no_auto_psv)) _C1Interrupt(void)   // Cada evento CAN de los 7 que hay
{
    if (C1INTFbits.RBOVIF == 1)             // Overflow interrupt
    {
        C1INTFbits.RBOVIF = 0;
    }

    if (C1INTFbits.RBIF == 1)               // Si se recibió un mensaje (Buffer(i) lleno) del CAN BUS
    {
        C1INTFbits.RBIF = 0;
    }

     if (C1INTFbits.TBIF == 1)              // Si se Envió un mensaje CAN al BUS
    {
        CANTXLED = ON;
        CANTXLEDBLINK;                      // Start LED timer
        C1INTFbits.TBIF = 0;
    }
    IFS2bits.C1IF = 0;
}


void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt(void)    // Every UART char sent
{
    IFS0bits.U1TXIF = 0;
}


void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt(void)    // Every UART char receive
{
    IFS0bits.U1RXIF = 0;
}


void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) // Timer 1
{
    static uint8_t i=0;
    i++;
    if (i==TDIV)
    {
        LEDRED^=1;
        i=0;
    }
    IFS0bits.T1IF = 0;                  //Clear Timer 1 interrupt flag
}


void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) // Timer 2
{
    CANRXLED = OFF;
    T2CONbits.TON = 0;                  // Stop Timer
    TMR2 = 0x00;                        // Clear timer register
    IFS0bits.T2IF = 0;                  //Clear Timer 2 interrupt flag
}


void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) // Timer 3
{
    CANTXLED = OFF;
    T3CONbits.TON = 0;                  // Stop Timer
    TMR3 = 0x00;                        // Clear timer register
    IFS0bits.T3IF = 0;                  //Clear Timer 3 interrupt flag
}

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) // Timer 4 de prueba
{
    static uint8_t i=0;
    i++;
    if (i==8)
    {
        /* CODIGO de PRUEBA 2*/
        /* END */
        i=0;
    }
    IFS1bits.T4IF = 0;                  //Clear Timer 1 interrupt flag
}