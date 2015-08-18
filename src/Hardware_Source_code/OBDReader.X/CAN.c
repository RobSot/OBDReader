
#include <xc.h>
#include "OBD.h"
#include "CAN.h"
#include "UART.h"
#include "user.h"

void CanInit()
{
    uint8_t txrx = 0xFF;                        // Configuration aux variable

    C1CTRL1bits.WIN = 0;                        // Set Control window to change configuration registers
    C1CTRL1bits.REQOP = 4;                      // Set the ECAN module for Configuration Mode
    while(C1CTRL1bits.OPMODE != 4);             // Wait for the ECAN module to enter into Configuration Mode


    /* SET UP timing stuff */
    C1CTRL1bits.CANCKS = 0x0;                   // Fcan = Fp = 70 MHz
    //C1CFG2bits.SAM = 0x1;                       // Sampled 3 times
    C1CFG1 = 6;                                 // 500 bps
    C1CFG2 = 0x4018;                            // 500 bps

    /* SET UP configuration buffer direction settings */
    /* if set to 1, corresponding buffer is Tx buffer */
    /* if set to 0, corresponding buffer is Rx buffer */

    txrx ^= txrx << NUM_OF_CANTX_BUFF;

    C1TR01CONbits.TXEN0 = (txrx >> 0) & 0x01;                    // Buffer 0
    C1TR01CONbits.TXEN1 = (txrx >> 1) & 0x01;                    // Buffer 1
    C1TR23CONbits.TXEN2 = (txrx >> 2) & 0x01;                    // Buffer 2
    C1TR23CONbits.TXEN3 = (txrx >> 3) & 0x01;                    // Buffer 3
    C1TR45CONbits.TXEN4 = (txrx >> 4) & 0x01;                    // Buffer 4
    C1TR45CONbits.TXEN5 = (txrx >> 5) & 0x01;                    // Buffer 5
    C1TR67CONbits.TXEN6 = (txrx >> 6) & 0x01;                    // Buffer 6
    C1TR67CONbits.TXEN7 = (txrx >> 7) & 0x01;                    // Buffer 7


    //C1TR01CONbits.TX0PRI = 2;                   // Prioridad del buffer 0
    //C1TR01CONbits.TX1PRI = 3;                   // Prioridad del buffer 1

    /* SET UP Filters & Masks */
    C1CTRL1bits.WIN = 1;                        // Set Filters window edition
    C1FCTRLbits.DMABS = 6;                      // 32 buffers to use in DMA
    C1FCTRLbits.FSA = FIFO_START_BUFFER;        // FIFO Start buffer

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
}

void CanTxDmaInit()
{
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
    IEC0bits.DMA0IE  = 0;                            // Disable DMA interrupt
    DMA0CONbits.CHEN = 1;                            // Enable DMA module
}

void CanRxDmaInit()
{
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
}

void OnCanRx(can_t canFrame)
{
    SendUARTFrame(OBD2UARTFramme(CAN2OBDFrame(canFrame)));
}

can_t ReadCANFrame(uint16_t RxBuffer)
{
    can_t canFrame;

    canFrame.id = (uint16_t) (CANBuff[RxBuffer][0] >> 2);
    canFrame.dlc = (uint8_t) CANBuff[RxBuffer][2];
    canFrame.data[0] = (uint8_t) CANBuff[RxBuffer][3];
    canFrame.data[1] = (uint8_t) (CANBuff[RxBuffer][3] >> 8);
    canFrame.data[2] = (uint8_t) CANBuff[RxBuffer][4];
    canFrame.data[3] = (uint8_t) (CANBuff[RxBuffer][4] >> 8);
    canFrame.data[4] = (uint8_t) CANBuff[RxBuffer][5];
    canFrame.data[5] = (uint8_t) (CANBuff[RxBuffer][5] >> 8);
    canFrame.data[6] = (uint8_t) CANBuff[RxBuffer][6];
    canFrame.data[7] = (uint8_t) (CANBuff[RxBuffer][6] >> 8);

    ClearCANRxBuffer(RxBuffer);

    return canFrame;
}

void SendCANFrame(can_t canFrame)
{
    static uint8_t TxBuffer = 0;

    /* Fill the buffer */
    CANBuff[TxBuffer][0] = (uint16_t)(canFrame.id << 2);
    CANBuff[TxBuffer][2] = (uint16_t) canFrame.dlc;
    CANBuff[TxBuffer][3] = (uint16_t) ((canFrame.data[1] << 8) | canFrame.data[0]);
    CANBuff[TxBuffer][4] = (uint16_t) ((canFrame.data[3] << 8) | canFrame.data[2]);
    CANBuff[TxBuffer][5] = (uint16_t) ((canFrame.data[5] << 8) | canFrame.data[4]);
    CANBuff[TxBuffer][6] = (uint16_t) ((canFrame.data[7] << 8) | canFrame.data[6]);

    /* Send the buffer */
    if      (TxBuffer == 0){SENDCANBUFF0;}
    else if (TxBuffer == 1){SENDCANBUFF1;}
    else if (TxBuffer == 2){SENDCANBUFF2;}
    else if (TxBuffer == 3){SENDCANBUFF3;}
    else if (TxBuffer == 4){SENDCANBUFF4;}
    else if (TxBuffer == 5){SENDCANBUFF5;}
    else if (TxBuffer == 6){SENDCANBUFF6;}
    else if (TxBuffer == 7){SENDCANBUFF7;}
    
    TxBuffer++;

    if (TxBuffer >= NUM_OF_CANTX_BUFF) {TxBuffer = 0;}
}

void ClearCANRxBuffer(uint16_t buffer)
{
    if (buffer < 16) {C1RXFUL1 &= ~(1 << buffer);}
    else {C1RXFUL2 &= ~(1 << (CANRXBUFFi - 16));}
}