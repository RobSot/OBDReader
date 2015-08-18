#include "OBD.h"

uart_msg_t OBD2UARTFramme(obd_t obdFrame)
{
    uart_msg_t uartFrame;

    uartFrame.num = obdFrame.num;
    uartFrame.mode = obdFrame.mode;
    uartFrame.pid = obdFrame.pid;
    uartFrame.data[0] = obdFrame.data[0];
    uartFrame.data[1] = obdFrame.data[1];
    uartFrame.data[2] = obdFrame.data[2];
    uartFrame.data[3] = obdFrame.data[3];

    return uartFrame;
}

obd_t UART2OBDFrame(uart_cmd_t cmd)
{
    obd_t obdFrame;

    obdFrame.num = 2;
    obdFrame.mode = cmd.mode;
    obdFrame.pid = cmd.pid;
    obdFrame.data[0] = 0;
    obdFrame.data[1] = 0;
    obdFrame.data[2] = 0;
    obdFrame.data[3] = 0;

    return obdFrame;
}

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
    canFrame = OBD2CANFrame(obdFrame);
    SendCANFrame(canFrame);
}