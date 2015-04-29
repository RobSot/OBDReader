/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */
#include "user.h"


/******************************************************************************/
/*                    Interrupt Vector Options                                */
/******************************************************************************/
/*                                                                            */
/* Refer to the C30 (MPLAB C Compiler for PIC24F MCUs and dsPIC33F DSCs) User */
/* Guide for an up to date list of the available interrupt options.           */
/* Alternately these names can be pulled from the device linker scripts.      */
/*                                                                            */
/*                                                                            */
/* dsPIC33E Primary Interrupt Vector Names:                                   */
/*                                                                            */
/* _INT0Interrupt     _IC4Interrupt      _U4TXInterrupt                       */
/* _IC1Interrupt      _IC5Interrupt      _SPI3ErrInterrupt                    */
/* _OC1Interrupt      _IC6Interrupt      _SPI3Interrupt                       */
/* _T1Interrupt       _OC5Interrupt      _OC9Interrupt                        */
/* _DMA0Interrupt     _OC6Interrupt      _IC9Interrupt                        */
/* _IC2Interrupt      _OC7Interrupt      _PWM1Interrupt                       */
/* _OC2Interrupt      _OC8Interrupt      _PWM2Interrupt                       */
/* _T2Interrupt       _PMPInterrupt      _PWM3Interrupt                       */
/* _T3Interrupt       _DMA4Interrupt     _PWM4Interrupt                       */
/* _SPI1ErrInterrupt  _T6Interrupt       _PWM5Interrupt                       */
/* _SPI1Interrupt     _T7Interrupt       _PWM6Interrupt                       */
/* _U1RXInterrupt     _SI2C2Interrupt    _PWM7Interrupt                       */
/* _U1TXInterrupt     _MI2C2Interrupt    _DMA8Interrupt                       */
/* _AD1Interrupt      _T8Interrupt       _DMA9Interrupt                       */
/* _DMA1Interrupt     _T9Interrupt       _DMA10Interrupt                      */
/* _NVMInterrupt      _INT3Interrupt     _DMA11Interrupt                      */
/* _SI2C1Interrupt    _INT4Interrupt     _SPI4ErrInterrupt                    */
/* _MI2C1Interrupt    _C2RxRdyInterrupt  _SPI4Interrupt                       */
/* _CM1Interrupt      _C2Interrupt       _OC10Interrupt                       */
/* _CNInterrupt       _QEI1Interrupt     _IC10Interrupt                       */
/* _INT1Interrupt     _DCIEInterrupt     _OC11Interrupt                       */
/* _AD2Interrupt      _DCIInterrupt      _IC11Interrupt                       */
/* _IC7Interrupt      _DMA5Interrupt     _OC12Interrupt                       */
/* _IC8Interrupt      _RTCCInterrupt     _IC12Interrupt                       */
/* _DMA2Interrupt     _U1ErrInterrupt    _DMA12Interrupt                      */
/* _OC3Interrupt      _U2ErrInterrupt    _DMA13Interrupt                      */
/* _OC4Interrupt      _CRCInterrupt      _DMA14Interrupt                      */
/* _T4Interrupt       _DMA6Interrupt     _OC13Interrupt                       */
/* _T5Interrupt       _DMA7Interrupt     _IC13Interrupt                       */
/* _INT2Interrupt     _C1TxReqInterrupt  _OC14Interrupt                       */
/* _U2RXInterrupt     _C2TxReqInterrupt  _IC14Interrupt                       */
/* _U2TXInterrupt     _QEI2Interrupt     _OC15Interrupt                       */
/* _SPI2ErrInterrupt  _U3ErrInterrupt    _IC15Interrupt                       */
/* _SPI2Interrupt     _U3RXInterrupt     _OC16Interrupt                       */
/* _C1RxRdyInterrupt  _U3TXInterrupt     _IC16Interrupt                       */
/* _C1Interrupt       _USB1Interrupt     _ICDInterrupt                        */
/* _DMA3Interrupt     _U4ErrInterrupt    _PWMSpEventMatchInterrupt            */
/* _IC3Interrupt      _U4RXInterrupt     _PWMSecSpEventMatchInterrupt         */
/*                                                                            */
/* For alternate interrupt vector naming, simply add 'Alt' between the prim.  */
/* interrupt vector name '_' and the first character of the primary interrupt */
/* vector name.  There is no Alternate Vector or 'AIVT' for the 33E family.   */
/*                                                                            */
/* For example, the vector name _ADC2Interrupt becomes _AltADC2Interrupt in   */
/* the alternate vector table.                                                */
/*                                                                            */
/* Example Syntax:                                                            */
/*                                                                            */
/* void __attribute__((interrupt, no_auto_psv)) <Vector Name>(void)           */
/* {                                                                          */
/*     <Clear Interrupt Flag>                                                 */
/* }                                                                          */
/*                                                                            */
/* For more comprehensive interrupt examples refer to the C30 (MPLAB C        */
/* Compiler for PIC24 MCUs and dsPIC DSCs) User Guide in the                  */
/* <C30 compiler instal directory>/doc directory for the latest compiler      */
/* release.  For XC16, refer to the MPLAB XC16 C Compiler User's Guide in the */
/* <XC16 compiler instal directory>/doc folder.                               */
/*                                                                            */
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

    CANRXLED = 1;                                               // Blink Rx LED
    CANRXLEDBLINK;                                              // Blink Rx LED
    UTxBuff[UTXMODE] = CANBuff[CANRXBUFFi][3] >> 8;             // MODE Byte
    UTxBuff[UTXPID]  = CANBuff[CANRXBUFFi][4] & 0x00FF;         // PID Byte
    UTxBuff[UTXNUM]  = CANBuff[CANRXBUFFi][3] & 0x00FF;         // Number of additional bytes
    switch (UTxBuff[UTXNUM])                                    // Switch between number of bytes
    {
        case 3:
            UARTDMACNT = 3;                                     // Bytes to be sent
            UTxBuff[DATA1] = CANBuff[CANRXBUFFi][4] >> 8;       // Byte data 1
            break;
        case 4:
            UARTDMACNT = 4;                                     // Bytes to be sent
            UTxBuff[DATA1] = CANBuff[CANRXBUFFi][4] >> 8;       // Byte data 1
            UTxBuff[DATA2] = CANBuff[CANRXBUFFi][5] & 0x00FF;   // Byte data 2
            break;
        case 5:
            UARTDMACNT = 5;                                     // Bytes to be sent
            UTxBuff[DATA1] = CANBuff[CANRXBUFFi][4] >> 8;       // Byte data 1
            UTxBuff[DATA2] = CANBuff[CANRXBUFFi][5] & 0x00FF;   // Byte data 2
            UTxBuff[DATA3] = CANBuff[CANRXBUFFi][5] >> 8;       // Byte data 3
            break;
        case 6:
            UARTDMACNT = 6;                                     // Bytes to be sent
            UTxBuff[DATA1] = CANBuff[CANRXBUFFi][4] >> 8;       // Byte data 1
            UTxBuff[DATA2] = CANBuff[CANRXBUFFi][5] & 0x00FF;   // Byte data 2
            UTxBuff[DATA3] = CANBuff[CANRXBUFFi][5] >> 8;       // Byte data 3
            UTxBuff[DATA4] = CANBuff[CANRXBUFFi][6] & 0x00FF;   // Byte data 4
            break;
        default:
            UARTDMACNT = 0;                                     // Bytes to be sent
            UTxBuff[0] = '!';                                   // Error Code
            break;
    }
    if ((UTxBuff[UTXNUM] > 2) & (UTxBuff[UTXNUM] < 7))          // Send Message if is not with error
    {
        //UTxBuff[UTXNUM]  = (CANBuff[CANRXBUFFi][3] + 48) & 0x00FF;  // Number of additional bytes
        SendUART();
    }
    ClearCANRxBuffer(CANRXBUFFi);
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
                if ((URxBuffA[URXMODE] == 'e') | (URxBuffA[URXPID] == 'e'))
                {
                    UTxBuff[0] = 'O';
                    UTxBuff[1] = 'k';
                    SendUARTNum(2);
                    SetCANMode(NORMALOP);
                }
            break;
            case NORMALOP:
                if ((URxBuffA[URXMODE] == 's') | (URxBuffA[URXPID] == 's'))
                {
                    UTxBuff[0] = '|';
                    UTxBuff[1] = '#';
                    SendUARTNum(2);
                    SetCANMode(DISSABLEOP);
                }
                else
                {
                    UTxBuff[0] = ' ';
                    UTxBuff[1] = '/';
                    UTxBuff[2] = '>';
                    SendUARTNum(3);
                    CANBuff[0][0] = (CANTXSID << 2);                // SID = CANTXSID, SRR = 0, IDE = 0
                    CANBuff[0][2] = CANDLC;                         // DLC = CANDLC
                    CANBuff[0][3] = (URxBuffA[URXMODE] << 8) | 2;   // Byte 0 = 2, Byte 1 = MODE
                    CANBuff[0][4] = URxBuffA[URXPID] & 0x00FF;      // Byte 2 = PID, Byte 3 = 0
                    SENDCANBUFF0;                                   // Send CAN message from buff 0
                }
            break;
        }
    }
    else  // RxBufferB has been received
    {
        switch (CANOPMODE)
        {
            case DISSABLEOP:
                if ((URxBuffB[URXMODE] == 'e') | (URxBuffB[URXPID] == 'e'))
                {
                    UTxBuff[0] = 'O';
                    UTxBuff[1] = 'k';
                    SendUARTNum(2);
                    SetCANMode(NORMALOP);
                }
            break;
            case NORMALOP:
                if ((URxBuffB[URXMODE] == 's') | (URxBuffB[URXPID] == 's'))
                {
                    UTxBuff[0] = '|';
                    UTxBuff[1] = '#';
                    SendUARTNum(2);
                    SetCANMode(DISSABLEOP);
                }
                else
                {
                    UTxBuff[0] = ' ';
                    UTxBuff[1] = '/';
                    UTxBuff[2] = '>';
                    SendUARTNum(3);
                    CANBuff[1][0] = (CANTXSID << 2);                // SID = CANTXSID, SRR = 0, IDE = 0
                    CANBuff[1][2] = CANDLC;                         // DLC = CANDLC
                    CANBuff[1][3] = (URxBuffB[URXMODE] << 8) | 2;   // Byte 0 = 2, Byte 1 = MODE
                    CANBuff[1][4] = URxBuffB[URXPID] & 0x00FF;      // Byte 2 = PID, Byte 3 = 0
                    SENDCANBUFF1;                                   // Send CAN message from buff 1
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
        CANTXLED = 1;
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
    CANRXLED = 0;
    T2CONbits.TON = 0;                  // Stop Timer
    TMR2 = 0x00;                        // Clear timer register
    IFS0bits.T2IF = 0;                  //Clear Timer 2 interrupt flag
}


void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) // Timer 3
{
    CANTXLED = 0;
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