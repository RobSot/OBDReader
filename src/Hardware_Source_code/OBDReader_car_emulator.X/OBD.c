#include "OBD.h"

obd_t CAN2OBDFrame(can_t canFrame)
{
    obd_t obdFrame;

    obdFrame.num = canFrame.data[0];
    obdFrame.mode = canFrame.data[1];
    obdFrame.pid = canFrame.data[2];
    obdFrame.data[0] = canFrame.data[3];
    obdFrame.data[1] = canFrame.data[4];
    obdFrame.data[2] = canFrame.data[5];
    obdFrame.data[3] = canFrame.data[6];

    return obdFrame;
}

can_t OBD2CANFrame(obd_t obdFrame)
{
    can_t canFrame;

    canFrame.id = CANTXSID;
    canFrame.dlc = CANDLC;
    canFrame.data[0] = obdFrame.num;
    canFrame.data[1] = obdFrame.mode;
    canFrame.data[2] = obdFrame.pid;
    canFrame.data[3] = obdFrame.data[0];
    canFrame.data[4] = obdFrame.data[1];
    canFrame.data[5] = obdFrame.data[2];
    canFrame.data[6] = obdFrame.data[3];

    return canFrame;
}

void SendOBDFrame(obd_t obdFrame)
{
    can_t canFrame;
    static uint8_t bufferCont = 0;
    canFrame = OBD2CANFrame(obdFrame);
    if (bufferCont >= NUM_OF_CANTX_BUFF) {bufferCont = 0;}
    SendCANFrame(canFrame,bufferCont);
    bufferCont++;
}