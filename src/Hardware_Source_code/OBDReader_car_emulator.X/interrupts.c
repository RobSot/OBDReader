/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */
#include "user.h"
#include "OBD.h"
#include "CAN.h"


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
    unsigned int i;
    can_t canFrame;
    
    CANRXLED = ON;                                              // Blink Rx LED
    CANRXLEDBLINK;                                              // Blink Rx LED

    canFrame = ReadCANFrame(CANRXBUFFi);

    for (i=0;i<65000;i++);                                      // ~= 10 ms delay

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
        switch (CANOPMODE)
        {
            case DISSABLEOP:
                if (URxBuffA[URXCMD] == 'e')
                {
                    UTxBuff[0] = 'O';
                    UTxBuff[1] = 'N';
                    SendUARTNum(2);
                    SetCANMode(NORMALOP);
//                    T4CONbits.TON = 1;  // inicia el envio de tramas periodicas de test
                }
            break;
            case NORMALOP:
                switch (URxBuffA[URXCMD])
                {
                    case 's': // Desactivar CAN

//                        T4CONbits.TON = 0;  // para el envio periodeico de tramas de test
//                        TMR4 = 0x00;        // para el envio periodeico de tramas de test
                        UTxBuff[0] = 'K';
                        UTxBuff[1] = 'O';
                        SendUARTNum(2);
                        SetCANMode(DISSABLEOP);
                    break;
                    case 2:  // Query OBD
                        CANBuff[0][0] = (CANTXSID << 2);                        // SID = CANTXSID, SRR = 0, IDE = 0
                        CANBuff[0][2] = CANDLC;                                 // DLC = CANDLC
                        CANBuff[0][3] = (URxBuffA[URXMODE] << 8) | 2;           // Byte 0 = 6, Byte 1 = MODE
                        CANBuff[0][4] = URxBuffA[URXPID] & 0x00FF;              // Byte 2 = PID, Byte 3 = uart[0]
                        SENDCANBUFF0;                                           // Send CAN message from buff 0
                    break;
                }
            break;
        }
    }
    else  // RxBufferB has been received
    {
        switch (CANOPMODE)
        {
            case DISSABLEOP:
                if (URxBuffB[URXCMD] == 'e')
                {
                    UTxBuff[0] = 'O';
                    UTxBuff[1] = 'N';
                    SendUARTNum(2);
                    SetCANMode(NORMALOP);
//                    T4CONbits.TON = 1;  // inicia el envio de tramas periodicas de test
                }
            break;
            case NORMALOP:
                switch (URxBuffB[URXCMD])
                {
                    case 's': // Desactivar CAN

//                        T4CONbits.TON = 0;  // para el envio periodeico de tramas de test
//                        TMR4 = 0x00;        // para el envio periodeico de tramas de test
                        UTxBuff[0] = 'K';
                        UTxBuff[1] = 'O';
                        SendUARTNum(2);
                        SetCANMode(DISSABLEOP);
                    break;
                    case 2:  // Query OBD
                        CANBuff[1][0] = (CANTXSID << 2);                        // SID = CANTXSID, SRR = 0, IDE = 0
                        CANBuff[1][2] = CANDLC;                                 // DLC = CANDLC
                        CANBuff[1][3] = (URxBuffB[URXMODE] << 8) | 2;           // Byte 0 = 6, Byte 1 = MODE
                        CANBuff[1][4] = URxBuffB[URXPID] & 0x00FF;              // Byte 2 = PID, Byte 3 = uart[0]
                        SENDCANBUFF1;                                           // Send CAN message from buff 0
                    break;
                }
            break;
        }
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
    if (i==3)
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
//    static uint8_t i=0;
//
//    i++;
//    if (i==1) // t = [i*100] ms
//    {
//
//        i=0;
//    }

    T4CONbits.TON = 0;                  // Stop the Timer
    IFS1bits.T4IF = 0;                  //Clear Timer 1 interrupt flag
}