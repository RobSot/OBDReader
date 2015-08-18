/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include "user.h"            /* variables/params used by user.c               */
#include "CAN.h"


/******************************************************************************/
/* User Global variables                                                      */
/******************************************************************************/


uint16_t CANBuff[NUM_CAN_BUFFERS][8] __attribute__((aligned(NUM_CAN_BUFFERS * 16))) = {{0}};

uint16_t UTxBuff[10] = {0};

uint16_t URxBuffA[3] = {0};//uint8_t URxBuffA[2] = {0};
uint16_t URxBuffB[3] = {0};//uint8_t URxBuffB[2] = {0};


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/


void InitApp(void)
{
    /**************************************************************************/
    /******************************* DMA **************************************/
    /**************************************************************************/

    CanTxDmaInit();
    CanRxDmaInit();


    /*########################### DMA2 UART TX ################################*/


    DMA2CONbits.SIZE = 0;                            // word transer
    DMA2CONbits.AMODE = 0;                           // Indirect with post-increment
    DMA2CONbits.MODE = 1;                            // One shoot mode & ping pong
    DMA2CONbits.DIR = 1;                             // RAM -> Peripheral
    DMA2CNT = 6;                                     // 7 DMA requests
    DMA2REQ = 12;                                    // Select UART Tx as DMA Request source
    DMA2PAD = (volatile unsigned int) &U1TXREG;      // 0x0224
    DMA2STAL = (unsigned int) &UTxBuff;
    DMA2STAH = 0;
    //DMA2STBL = (unsigned int) &UARTTxBufferB;
    //DMA2STBH = 0;
    IFS1bits.DMA2IF  = 0;                            // Clear DMA Interrupt Flag
    IEC1bits.DMA2IE  = 0;                            // Disable DMA interrupt
    DMA2CONbits.CHEN = 0;                            // (Tiene que estar a 0 al iniciar en modo one shoot)


    /*########################### DMA3 UART RX ################################*/


    DMA3CONbits.SIZE = 0;                            // Word transer
    DMA3CONbits.AMODE = 0;                           // Indirect with post-increment
    DMA3CONbits.MODE = 2;                            // Continuous mode & ping pong
    DMA3CONbits.DIR = 0;                             // Peripheral -> RAM
    DMA3CNT = 2;                                     // 3 DMA requests
    DMA3REQ = 11;                                    // Select UART Rx as DMA Request source
    DMA3PAD = (volatile unsigned int) &U1RXREG;      // 0x0226
    DMA3STAL = (unsigned int) &URxBuffA;
    DMA3STAH = 0;
    DMA3STBL = (unsigned int) &URxBuffB;
    DMA3STBH = 0;
    IFS2bits.DMA3IF  = 0;                            // Clear DMA Interrupt Flag
    IEC2bits.DMA3IE  = 0;                            // Disable DMA interrupt
    DMA3CONbits.CHEN = 1;                            // Enable DMA module


    /**************************************************************************/
    /******************************** UART ************************************/
    /**************************************************************************/

    
    U1BRG = 37;                         // 460800 bps, Error = 1,95%
    U1MODE &= 0b0100100000001000;       // Put zeros
    U1MODE |= 0b1000000000001000;       // Put ones
    U1STA &= 0b0001011101011111;        // Put zeros
    U1STA |= 0b0000010000000000;        // Put ones
    IFS0bits.U1RXIF = 0;                // Clear Rx interrupt flag
    IFS0bits.U1TXIF = 0;                // Clear Tx interrupt flag
    IEC0bits.U1RXIE = 0;                // Disable Rx interrupts
    IEC0bits.U1TXIE = 0;                // Disable Tx interrupts
    /* Poner un delay de valor = (1/Baud rate) */


    /**************************************************************************/
    /******************************* ECAN *************************************/
    /**************************************************************************/
    

    CanInit();


    /**************************************************************************/
    /******************************* TIMERS ***********************************/
    /**************************************************************************/
    
    
    /*############################## TIMER 1 #################################*/
    
    
    T1CONbits.TON = 0;                  // Stop Timer
    T1CONbits.TCS=0;                    // Configure as a Timer
    T1CONbits.TGATE=0;                  // Configure as a Timer
    T1CONbits.TCKPS = 0b11;             // Select 1:256 Prescaler
    TMR1 = 0x00;                        // Clear timer register
    PR1 = 34180;                        // 125 ms x4 = 0.5 s
    IPC0bits.T1IP = 0x01;               // Set Timer 1 Interrupt Priority Level
    IFS0bits.T1IF = 0;                  // Clear Timer 1 Interrupt Flag
    IEC0bits.T1IE = 1;                  // Enable Timer 1 interrupt
    T1CONbits.TON = 1;                  // Start Timer


     /*############################## TIMER 2 #################################*/


    T2CONbits.TON = 0;                  // Stop Timer
    T2CONbits.TCS=0;                    // Configure as a Timer
    T2CONbits.TGATE=0;                  // Configure as a Timer
    T2CONbits.TCKPS = 0b11;             // Select 1:256 Prescaler
    TMR2 = 0x00;                        // Clear timer register
    PR2 = 27344;                        // 100 ms
    IPC1bits.T2IP = 0x01;               // Set Timer 2 Interrupt Priority Level
    IFS0bits.T2IF = 0;                  // Clear Timer 2 Interrupt Flag
    IEC0bits.T2IE = 1;                  // Enable Timer 2 interrupt
    T2CONbits.TON = 0;                  // Do not Start Timer


    /*############################## TIMER 3 #################################*/


    T3CONbits.TON = 0;                  // Stop Timer
    T3CONbits.TCS=0;                    // Configure as a Timer
    T3CONbits.TGATE=0;                  // Configure as a Timer
    T3CONbits.TCKPS = 0b11;             // Select 1:256 Prescaler
    TMR3 = 0x00;                        // Clear timer register
    PR3 = 27344;                        // 100 ms
    IPC2bits.T3IP = 0x01;               // Set Timer 3 Interrupt Priority Level
    IFS0bits.T3IF = 0;                  // Clear Timer 3 Interrupt Flag
    IEC0bits.T3IE = 1;                  // Enable Timer 3 interrupt
    T3CONbits.TON = 0;                  // Do not Start Timer


    /*############################## TIMER 4 #################################*/


    T4CONbits.TON = 0;                  // Stop Timer
    T4CONbits.TCS=0;                    // Configure as a Timer
    T4CONbits.TGATE=0;                  // Configure as a Timer
    T4CONbits.TCKPS = 0b11;             // Select 1:256 Prescaler
    TMR4 = 0x00;                        // Clear timer register
    PR4 = 2734;                         // 10 ms
    IPC6bits.T4IP = 0x01;               // Set Timer 4 Interrupt Priority Level
    IFS1bits.T4IF = 0;                  // Clear Timer 4 Interrupt Flag
    IEC1bits.T4IE = 1;                  // Enable Timer 4 interrupt
    T4CONbits.TON = 0;                  // Do not Start Timer


}


void ConfigureLEDS(void)
{
    /*Union soldada*/
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    LATBbits.LATB8 = 0;
    LATBbits.LATB9 = 0;
    /*Union soldada*/

    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB10 = 0;
    LEDRED = ON;
    CANRXLED = OFF;
    CANTXLED = OFF;
}


void SetCANMode(uint16_t mode)
{
    C1CTRL1bits.REQOP = mode;                      // Set the ECAN module Mode
    while(C1CTRL1bits.OPMODE != mode);             // Wait for the ECAN module to enter the Mode
}


void SendUARTNum(uint16_t num)
{
    while (UARTBUSY);                                       // Wait until UART is free to transmit
    UARTDMACNT = num - 1;                                   // numbero of bytes to transfer from UART buffer
    DMA2CONbits.CHEN = 1;                                   // Enable DMA2 channel
    DMA2REQbits.FORCE = 1;                                  // Manual mode: Kick-start the 1st transfer
}


void SendUART(void)
{
    while (UARTBUSY);                                       // Wait until UART is free to transmit
    DMA2CONbits.CHEN = 1;                                   // Enable DMA2 channel
    DMA2REQbits.FORCE = 1;                                  // Manual mode: Kick-start the 1st transfer
}