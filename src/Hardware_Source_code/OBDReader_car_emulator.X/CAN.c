
#include <xc.h>
#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */
#include "OBD.h"
#include "CAN.h"
#include "user.h"

void CanInit()
{
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
    C1TR01CONbits.TXEN1 = 1;                    // Buffer 1 is a Tx buffer
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
    IEC0bits.DMA0IE  = 1;                            // Enable DMA interrupt
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
    obd_t obdFrame;
    obdFrame = CAN2OBDFrame(canFrame);

    /***********************/
    /*** Avaliable PIDs ****/
    /***********************/
    if (obdFrame.pid == 0x0000)
    {
        obdFrame.mode += 1;
        obdFrame.num = 6;
        obdFrame.data[0] = 0x18;    //0001 1000; 0x04,0x05
        obdFrame.data[1] = 0x19;    //0001 1001; 0x0C,0x0D,0x10
        obdFrame.data[2] = 0x18;    //0001 1000; 0x14,0x15
        obdFrame.data[3] = 0x01;    //0000 0001; 0x20

        SendOBDFrame(obdFrame);
    }

    else if (obdFrame.pid == 0x0020)
    {
        obdFrame.mode += 1;
        obdFrame.num = 6;
        obdFrame.data[0] = 0x00;    //0000 0000;
        obdFrame.data[1] = 0x00;    //0000 0000;
        obdFrame.data[2] = 0x00;    //0000 0000;
        obdFrame.data[3] = 0x01;    //0000 0001; 0x40

        SendOBDFrame(obdFrame);
    }

    else if (obdFrame.pid == 0x0040)
    {
        obdFrame.mode += 1;
        obdFrame.num = 6;
        obdFrame.data[0] = 0x04;    //0000 0100;
        obdFrame.data[1] = 0x00;
        obdFrame.data[2] = 0x00;
        obdFrame.data[3] = 0x00;

        SendOBDFrame(obdFrame);
    }

    else if (obdFrame.pid == 0x0060)
    {
        obdFrame.mode += 1;
        obdFrame.num = 6;
        obdFrame.data[0] = 0x02;
        obdFrame.data[1] = 0x02;
        obdFrame.data[2] = 0x28;
        obdFrame.data[3] = 0x01;

        SendOBDFrame(obdFrame);
    }

    else if (obdFrame.pid == 0x0080)
    {
        obdFrame.mode += 1;
        obdFrame.num = 6;
        obdFrame.data[0] = 0x18;
        obdFrame.data[1] = 0x03;
        obdFrame.data[2] = 0x05;
        obdFrame.data[3] = 0x01;

        SendOBDFrame(obdFrame);
    }

    else if (obdFrame.pid == 0x00A0)
    {
        obdFrame.mode += 1;
        obdFrame.num = 6;
        obdFrame.data[0] = 0x21;
        obdFrame.data[1] = 0x11;
        obdFrame.data[2] = 0x82;
        obdFrame.data[3] = 0x21;

        SendOBDFrame(obdFrame);
    }

    else if (obdFrame.pid == 0x00C0)
    {
        obdFrame.mode += 1;
        obdFrame.num = 6;
        obdFrame.data[0] = 0x18;
        obdFrame.data[1] = 0x10;
        obdFrame.data[2] = 0x12;
        obdFrame.data[3] = 0x21;

        SendOBDFrame(obdFrame);
    }

    /***********************/
    /* PIDs Implementation */
    /***********************/
    else if (obdFrame.pid == 0x0004)                                            // Calculated Engine Load
    {
        static uint8_t load = 0;
        if (load > 250) {load = 0;}

        obdFrame.mode += 1;
        obdFrame.num = 3;
        obdFrame.data[0] = load;

        SendOBDFrame(obdFrame);

        load++;
    }

    else if (obdFrame.pid == 0x0005)                                            // Coolant Temp
    {
        static uint8_t temp = 65;
        if (temp > 135) {temp = 125;}

        obdFrame.mode += 1;
        obdFrame.num = 3;
        obdFrame.data[0] = temp;

        SendOBDFrame(obdFrame);

        temp++;
    }

    else if (obdFrame.pid == 0x000C)                                            // Engine RPM
    {
        static uint16_t rpm = 900 * 4;
        if (rpm > 5000 * 4) {rpm = 3000 * 4;}

        obdFrame.mode += 1;
        obdFrame.num = 4;
        obdFrame.data[0] = (uint8_t) (rpm >> 8);
        obdFrame.data[1] = (uint8_t) rpm;

        SendOBDFrame(obdFrame);

        rpm = ((rpm/4)+100)*4;
    }

    else if (obdFrame.pid == 0x000D)                                            // Vehicle Speed
    {
        static uint8_t speed = 0;
        if (speed > 250) {speed = 0;}

        obdFrame.mode += 1;
        obdFrame.num = 3;
        obdFrame.data[0] = speed;

        SendOBDFrame(obdFrame);
        
        speed++;
    }

    else if (obdFrame.pid == 0x0010)                                            // MAF
    {
        static uint16_t maf = 5000;
        if (maf > 28000) {maf = 5000;}

        obdFrame.mode += 1;
        obdFrame.num = 4;
        obdFrame.data[0] = (uint8_t) (maf >> 8);
        obdFrame.data[1] = (uint8_t) maf;

        SendOBDFrame(obdFrame);

        maf += 100;
    }

    else if (obdFrame.pid == 0x0014)                                            // Oxigen sensor 1
    {
        static uint8_t voltage = 200;
        if (voltage > 220) {voltage = 50;}

        obdFrame.mode += 1;
        obdFrame.num = 3;
        obdFrame.data[0] = voltage;

        SendOBDFrame(obdFrame);

        voltage += 7;
    }

    else if (obdFrame.pid == 0x0015)                                            // Oxigen sensor 2
    {
        static uint8_t voltage = 100;
        if (voltage > 240) {voltage = 0;}

        obdFrame.mode += 1;
        obdFrame.num = 3;
        obdFrame.data[0] = voltage;

        SendOBDFrame(obdFrame);

        voltage++;
    }
    
//    else if (obdFrame.pid == 0x0024)                                            // Lamda 1
//    {
//        static uint16_t lamda = 900 * 4;
//        if (lamda > 5000 * 4) {lamda = 3000 * 4;}
//
//        obdFrame.mode += 1;
//        obdFrame.num = 6;
//        obdFrame.data[0] = (uint8_t) (lamda >> 8);
//        obdFrame.data[1] = (uint8_t) lamda;
//        obdFrame.data[2] = (uint8_t) (lamda >> 8);
//        obdFrame.data[3] = (uint8_t) lamda;
//
//        SendOBDFrame(obdFrame);
//
//        lamda = ((lamda/4)+100)*4;
//    }

    else if (obdFrame.pid == 0x0046)                                            // Ambient Temp
    {
        static uint8_t temp = 10;
        if (temp > 70) {temp = 10;}

        obdFrame.mode += 1;
        obdFrame.num = 3;
        obdFrame.data[0] = temp;

        SendOBDFrame(obdFrame);

        temp++;
    }
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

void SendCANFrame(can_t canFrame, uint8_t TxBuffer)
{
    CANBuff[TxBuffer][0] = (uint16_t)(canFrame.id << 2);
    CANBuff[TxBuffer][2] = (uint16_t) canFrame.dlc;
    CANBuff[TxBuffer][3] = (uint16_t) ((canFrame.data[1] << 8) | canFrame.data[0]);
    CANBuff[TxBuffer][4] = (uint16_t) ((canFrame.data[3] << 8) | canFrame.data[2]);
    CANBuff[TxBuffer][5] = (uint16_t) ((canFrame.data[5] << 8) | canFrame.data[4]);
    CANBuff[TxBuffer][6] = (uint16_t) ((canFrame.data[7] << 8) | canFrame.data[6]);

    switch (TxBuffer)
    {
        case 0:
            SENDCANBUFF0;
            break;
        case 1:
            SENDCANBUFF1;
            break;
    }
}

void ClearCANRxBuffer(uint16_t buffer)
{
    if (buffer < 16) {C1RXFUL1 &= ~(1 << buffer);}
    else {C1RXFUL2 &= ~(1 << (CANRXBUFFi - 16));}
}