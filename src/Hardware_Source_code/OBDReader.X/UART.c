#include <xc.h>
#include "UART.h"
#include "user.h"
#include "OBD.h"


void UartTxDmaInit(void)
{
    DMA2CONbits.SIZE = 0;                            // word transer
    DMA2CONbits.AMODE = 0;                           // Indirect with post-increment
    DMA2CONbits.MODE = 1;                            // One shoot mode & ping pong
    DMA2CONbits.DIR = 1;                             // RAM -> Peripheral
    DMA2CNT = NUM_UART_TX_BUFFERS - 1;               // 7 DMA requests
    DMA2REQ = 12;                                    // Select UART Tx as DMA Request source
    DMA2PAD = (volatile unsigned int) &U1TXREG;      // 0x0224
    DMA2STAL = (unsigned int) &UTxBuff;
    DMA2STAH = 0;
    //DMA2STBL = (unsigned int) &UARTTxBufferB;
    //DMA2STBH = 0;
    IFS1bits.DMA2IF  = 0;                            // Clear DMA Interrupt Flag
    IEC1bits.DMA2IE  = 0;                            // Disable DMA interrupt
    DMA2CONbits.CHEN = 0;                            // (Tiene que estar a 0 al iniciar en modo one shoot)
}

void UartRxDmaInit(void)
{
    DMA3CONbits.SIZE = 0;                            // Word transer
    DMA3CONbits.AMODE = 0;                           // Indirect with post-increment
    DMA3CONbits.MODE = 2;                            // Continuous mode & ping pong
    DMA3CONbits.DIR = 0;                             // Peripheral -> RAM
    DMA3CNT = NUM_UART_RX_BUFFERS - 1;               // 3 DMA requests
    DMA3REQ = 11;                                    // Select UART Rx as DMA Request source
    DMA3PAD = (volatile unsigned int) &U1RXREG;      // 0x0226
    DMA3STAL = (unsigned int) &URxBuffA;
    DMA3STAH = 0;
    DMA3STBL = (unsigned int) &URxBuffB;
    DMA3STBH = 0;
    IFS2bits.DMA3IF  = 0;                            // Clear DMA Interrupt Flag
    IEC2bits.DMA3IE  = 1;                            // Enable DMA interrupt
    DMA3CONbits.CHEN = 1;                            // Enable DMA module
}

void UartInit(void)
{
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
}

void SendUARTFrame(uart_msg_t uartFrame)
{
    UTxBuff[0] = uartFrame.num;
    UTxBuff[1] = uartFrame.mode;
    UTxBuff[2] = uartFrame.pid;
    UTxBuff[3] = uartFrame.data[0];
    UTxBuff[4] = uartFrame.data[1];
    UTxBuff[5] = uartFrame.data[2];
    UTxBuff[6] = uartFrame.data[3];

    SendUART();
}

void OnUartRx(uint16_t *buffer)
{
    uart_cmd_t cmd;

    cmd.cmd = buffer[URXCMD];
    cmd.mode = buffer[URXMODE];
    cmd.pid = buffer[URXPID];

    switch (CANOPMODE)
    {
    case DISSABLEOP:
        if (cmd.cmd == 'e')
        {
            UTxBuff[0] = 'O';
            UTxBuff[1] = 'N';
            SendUARTNum(2);
            SetCANMode(NORMALOP);
        }
        if (cmd.cmd == 3) // cambio beat del led status
        {
            T1CONbits.TON = 0; // Stop Timer
            TMR1 = 0x00; // Clear timer register
            PR1 = 257 * (cmd.mode & 0x00FF); // URXMODE ms
            T1CONbits.TON = 1; // Start Timer
        }
        break;
    case NORMALOP:
        switch (cmd.cmd)
        {
        case 's': // Desactivar CAN
            UTxBuff[0] = 'K';
            UTxBuff[1] = 'O';
            SendUARTNum(2);
            SetCANMode(DISSABLEOP);
            break;
        case 2: // Query OBD
            SendOBDFrame(UART2OBDFrame(cmd));
            break;
        }
        break;
    }
}