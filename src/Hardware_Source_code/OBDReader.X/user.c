/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include "user.h"            /* variables/params used by user.c               */
#include "CAN.h"
#include "UART.h"


/******************************************************************************/
/* User Global variables                                                      */
/******************************************************************************/


uint16_t CANBuff[NUM_CAN_BUFFERS][8] __attribute__((aligned(NUM_CAN_BUFFERS * 16))) = {{0}};

uint16_t UTxBuff[NUM_UART_TX_BUFFERS + 3] = {0};

uint16_t URxBuffA[NUM_UART_RX_BUFFERS] = {0};//uint8_t URxBuffA[2] = {0};
uint16_t URxBuffB[NUM_UART_RX_BUFFERS] = {0};//uint8_t URxBuffB[2] = {0};


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/


void InitApp(void)
{
    /**************************************************************************/
    /******************************* DMA **************************************/
    /**************************************************************************/

    /**** CAN ****/
    CanTxDmaInit();
    CanRxDmaInit();

    /**** UART ****/
    UartTxDmaInit();
    UartRxDmaInit();


    /**************************************************************************/
    /******************************** UART ************************************/
    /**************************************************************************/

    
    UartInit();


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
    UARTDMACNT = num - 1;                                   // number of bytes to transfer from UART buffer
    DMA2CONbits.CHEN = 1;                                   // Enable DMA2 channel
    DMA2REQbits.FORCE = 1;                                  // Manual mode: Kick-start the 1st transfer
//    UARTDMACNT = NUM_UART_TX_BUFFERS - 1;                   // keep original number of bytes to transfer from UART buffer
}


void SendUART(void)
{
    UARTDMACNT = NUM_UART_TX_BUFFERS - 1;                   // Send prefixed number of bytes throuhg UART
    while (UARTBUSY);                                       // Wait until UART is free to transmit
    DMA2CONbits.CHEN = 1;                                   // Enable DMA2 channel
    DMA2REQbits.FORCE = 1;                                  // Manual mode: Kick-start the 1st transfer
}