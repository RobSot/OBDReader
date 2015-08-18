/* 
 * File:   OBD.h
 * Author: Rober
 *
 * Created on 17 de junio de 2015, 22:05
 */

#ifndef OBD_H
#define	OBD_H

#include <stdint.h>
#include "CAN.h"
#include "user.h"

struct obd_s {
    uint8_t num;
    uint8_t mode;
    uint8_t pid;
    uint8_t data[4];
};

typedef struct obd_s obd_t;

obd_t CAN2OBDFrame(can_t canFrame);
can_t OBD2CANFrame(obd_t obdFrame);
void SendOBDFrame(obd_t obdFrame);

#endif	/* OBD_H */

