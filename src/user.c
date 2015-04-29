/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>
#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include "user.h"            /* variables/params used by user.c               */


/******************************************************************************/
/* User Global variables                                                      */
/******************************************************************************/


uint16_t CANBuff[NUM_CAN_BUFFERS][8] __attribute__((aligned(NUM_CAN_BUFFERS * 16))) = {{0}};

uint16_t UTxBuff[7] = {0};

uint16_t URxBuffA[2] = {0};//uint8_t URxBuffA[2] = {0};
uint16_t URxBuffB[2] = {0};//uint8_t URxBuffB[2] = {0};


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/


void InitApp(void)
{


    /**************************************************************************/
    /******************************* DMA **************************************/
    /**************************************************************************/


    /*########################### DMA0 CAN TX #################################*/


    DMA0CONbits.SIZE = 0;                            // Word transer
    DMA0CONbits.AMODE = 2;                           // Peripheral Indirect
    DMA0CONbits.MODE = 0;                            // Continuous mode NO ping pong
    DMA0CONbits.DIR = 1;                             // RAM -> Peripheral
    DMA0CNT = 7;                                     // 7 DMA requests
    DMA0REQ = 70;                                    // Select ECAN as DMA Request source
    DMA0PAD = (volatile unsigned int) &C1TXD;        // &C1TXD = 0x0442
    DMA0STAL = (unsigned int) &CANBuff;
    DMA0STAH = 0;
    //DMA0STBL = &CANTxBufferB;
    //DMA0STBH = 0;
    IFS0bits.DMA0IF  = 0;                            // Clear DMA Interrupt Flag
    IEC0bits.DMA0IE  = 1;                            // Enable DMA interrupt
    DMA0CONbits.CHEN = 1;                            // Enable DMA module


    /*########################### DMA1 CAN RX #################################*/


    DMA1CONbits.SIZE = 0;                            // Word transer
    DMA1CONbits.AMODE = 2;                           // Peripheral Indirect
    DMA1CONbits.MODE = 0;                            // Continuous mode NO ping pong
    DMA1CONbits.DIR = 0;                             // Peripheral -> RAM
    DMA1CNT = 7;                                     // 7 DMA requests
    DMA1REQ = 34;                                    // Select ECAN as DMA Request source
    DMA1PAD = (volatile unsigned int) &C1RXD;
    DMA1STAL = (unsigned int) &CANBuff;
    DMA1STAH = 0;
    //DMA1STBL = (unsigned int) &CANRxBufferB;
    //DMA1STBH = 0;
    IFS0bits.DMA1IF = 0;                             // Clear DMA interrupt
    IEC0bits.DMA1IE = 1;                             // Enable DMA interrupt
    DMA1CONbits.CHEN = 1;                            // Enable DMA channel


    /*########################### DMA2 UART TX ################################*/


    DMA2CONbits.SIZE = 0;                            // word transer
    DMA2CONbits.AMODE = 0;                           // Indirect with post-increment
    DMA2CONbits.MODE = 1;                            // One shoot mode & ping pong
    DMA2CONbits.DIR = 1;                             // RAM -> Peripheral
    DMA2CNT = 3;                                     // 2 DMA requests
    DMA2REQ = 12;                                    // Select UART Tx as DMA Request source
    DMA2PAD = (volatile unsigned int) &U1TXREG;      // 0x0224
    DMA2STAL = (unsigned int) &UTxBuff;
    DMA2STAH = 0;
    //DMA2STBL = (unsigned int) &UARTTxBufferB;
    //DMA2STBH = 0;
    IFS1bits.DMA2IF  = 0;                            // Clear DMA Interrupt Flag
    IEC1bits.DMA2IE  = 1;                            // Enable DMA interrupt
    DMA2CONbits.CHEN = 0;                            // (Tiene que estar a 0 al iniciar en modo one shoot)


    /*########################### DMA3 UART RX ################################*/


    DMA3CONbits.SIZE = 0;                            // Word transer
    DMA3CONbits.AMODE = 0;                           // Indirect with post-increment
    DMA3CONbits.MODE = 2;                            // Continuous mode & ping pong
    DMA3CONbits.DIR = 0;                             // Peripheral -> RAM
    DMA3CNT = 1;                                     // 2 DMA requests
    DMA3REQ = 11;                                    // Select UART Rx as DMA Request source
    DMA3PAD = (volatile unsigned int) &U1RXREG;      // 0x0226
    DMA3STAL = (unsigned int) &URxBuffA;
    DMA3STAH = 0;
    DMA3STBL = (unsigned int) &URxBuffB;
    DMA3STBH = 0;
    IFS2bits.DMA3IF  = 0;                            // Clear DMA Interrupt Flag
    IEC2bits.DMA3IE  = 1;                            // Enable DMA interrupt
    DMA3CONbits.CHEN = 1;                            // Enable DMA module


    /**************************************************************************/
    /******************************** UART ************************************/
    /**************************************************************************/

    
    U1BRG = 151;                        // 115200 bps, Error = 0,94%
    U1MODE &= 0b0100100000001000;       // Put zeros
    U1MODE |= 0b1000000000001000;       // Put ones
    U1STA &= 0b0001011101011111;        // Put zeros
    U1STA |= 0b0000010000000000;        // Put ones
    IFS0bits.U1RXIF = 0;                // Clear Rx interrupt flag
    IFS0bits.U1TXIF = 0;                // Clear Tx interrupt flag
    IEC0bits.U1RXIE = 1;                // Enable Rx interrupts
    IEC0bits.U1TXIE = 1;                // Enable Tx interrupts
    /* Poner un delay de valor = (1/Baud rate) */


    /**************************************************************************/
    /******************************* ECAN *************************************/
    /**************************************************************************/
    

    C1CTRL1bits.WIN = 0;                        // Set Control window to change configuration registers
    C1CTRL1bits.REQOP = 4;                      // Set the ECAN module for Configuration Mode
    while(C1CTRL1bits.OPMODE != 4);             // Wait for the ECAN module to enter into Configuration Mode
    

    /* SET UP timing stuff */
    C1CTRL1bits.CANCKS = 0x0;                   // Fcan = Fp = 70 MHz
    //C1CFG2bits.SAM = 0x1;                       // Sampled 3 times
    C1CFG1 = 6;                                 // 500 bps
    C1CFG2 = 0x4018;                            // 500 bps

    /* SET UP Buffer settings */
    C1TR01CONbits.TXEN0 = 1;                    // Buffer 0 ia a Tx buffer
    C1TR01CONbits.TXEN1 = 1;                    // Buffer 1 is a Rx buffer
    C1TR23CONbits.TXEN2 = 0;                    // Buffer 2 is a Rx buffer
    C1TR23CONbits.TXEN3 = 0;                    // Buffer 3 is a Rx buffer
    C1TR45CONbits.TXEN4 = 0;                    // Buffer 4 is a Rx buffer
    C1TR45CONbits.TXEN5 = 0;                    // Buffer 5 is a Rx buffer
    C1TR67CONbits.TXEN6 = 0;                    // Buffer 6 is a Rx buffer
    C1TR67CONbits.TXEN7 = 0;                    // Buffer 7 is a Rx buffer

    //C1TR01CONbits.TX0PRI = 2;                   // Prioridad del buffer 0
    //C1TR01CONbits.TX1PRI = 3;                   // Prioridad del buffer 1

    /* SET UP Filters & Masks */
    C1CTRL1bits.WIN = 1;                        // Set Filters window edition
    C1FCTRLbits.DMABS = 6;                      // 32 buffers to use in DMA
    C1FCTRLbits.FSA = 2;                        // FIFO Start buffer

    C1FEN1bits.FLTEN0 = 1;                      // Filter 0 is enabled
    C1BUFPNT1bits.F0BP = 15;                    // Filter 0 points to FIFO
    C1RXF0SIDbits.SID = CANRXSID;               // Filter 0 configuration
    //C1RXF0EID = 0xFFFF;                       // Filter 0 configuration
    C1RXM0SIDbits.MIDE = 1;                     // Filter SID
    C1RXF0SIDbits.EXIDE= 0;                     // Filter SID

    C1FMSKSEL1bits.F0MSK = 0;                   // Mask 0 used for Filter 0
    C1RXM0SIDbits.SID = FILTERSID;              // Mask 0 configuration
    //C1RXM0EID = 0xFFFF;                       // Mask 0 configuration

    /* SET UP interrupts */
    /*FLAGS*/
    C1INTFbits.RBOVIF = 0;              // Clear Overrun interrupt flag
    C1INTFbits.TBIF = 0;                // Clear Buffer Tx interrupt flag
    C1INTFbits.RBIF = 0;                // Clear Buffer Rx interrupt flag
    IFS2bits.C1RXIF = 0;                // Clear word Rx interrupt flag
    IFS4bits.C1TXIF = 0;                // Clear word Tx interrupt flag
    IFS2bits.C1IF = 0;                  // Clear C1 event interrupt flag
    /*ENABLES*/
    //C1INTEbits.RBOVIE = 1;              // Enable Overrun interrupts
    //C1INTEbits.RBIE = 1;                // Enable Buffer Rx interrupts
    C1INTEbits.TBIE = 1;                // Enable Buffer Tx interrupts
    //IEC2bits.C1RXIE = 1;                // Enable word Rx interrupts
    //IEC4bits.C1TXIE = 1;                // Enable word Tx interrupts
    IEC2bits.C1IE = 1;                  // Enable C1 event interrupts


    /* START OPERATION */
    C1CTRL1bits.WIN = 0;                        // Set Control window to change configuration registers

    C1CTRL1bits.REQOP = 1;                      // Set the ECAN module for Disable Mode
    while(C1CTRL1bits.OPMODE != 1);             // Wait for the ECAN module to enter Disable Mode


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
    PR2 = 13672;                        // 50 ms
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
    PR3 = 13672;                        // 50 ms
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
    PR4 = 34000;                        // ~120 ms
    IPC6bits.T4IP = 0x01;               // Set Timer 4 Interrupt Priority Level
    IFS1bits.T4IF = 0;                  // Clear Timer 4 Interrupt Flag
    IEC1bits.T4IE = 1;                  // Enable Timer 4 interrupt
    T4CONbits.TON = 0;                  // Do not Start Timer


}


void ConfigureLEDS(void)
{
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB10 = 0;
    LEDRED = 1;
    CANRXLED = 0;
    CANTXLED = 0;
}


void SetCANMode(uint16_t mode)
{
    C1CTRL1bits.REQOP = mode;                      // Set the ECAN module Mode
    while(C1CTRL1bits.OPMODE != mode);             // Wait for the ECAN module to enter the Mode
}


void ClearCANRxBuffer(uint16_t buffer)
{
    if (buffer < 16) {C1RXFUL1 &= ~(1 << buffer);}
    else {C1RXFUL2 &= ~(1 << (CANRXBUFFi - 16));}
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