/* 
 * File:   CAN.h
 * Author: Rober
 *
 * Created on 10 de junio de 2015, 18:50
 */

#ifndef CAN_H
#define	CAN_H

#include <stdint.h>

struct can_s {
    uint16_t id;
    uint8_t dlc;
    uint8_t data[8];
};

typedef struct can_s can_t;

void CanInit(void);
void CanTxDmaInit(void);
void CanRxDmaInit(void);
void OnCanRx(can_t canFrame);
can_t ReadCANFrame(uint16_t RxBuffer);
void SendCANFrame(can_t canFrame);
void ClearCANRxBuffer(uint16_t buffer);     // Clear the full bit of "buffer" when it is read

#endif	/* CAN_H */

