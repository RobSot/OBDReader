/* 
 * File:   UART.h
 * Author: Rober
 *
 * Created on 6 de agosto de 2015, 22:25
 */

#ifndef UART_H
#define	UART_H

#include <stdint.h>
#include "user.h"

struct uart_cmd_s {
    uint8_t cmd;
    uint8_t mode;
    uint8_t pid;
};

typedef struct uart_cmd_s uart_cmd_t;

struct uart_msg_s {
    uint8_t num;
    uint8_t mode;
    uint8_t pid;
    uint8_t data[4];
};

typedef struct uart_msg_s uart_msg_t;


void UartTxDmaInit(void);
void UartRxDmaInit(void);
void UartInit(void);
void SendUARTFrame(uart_msg_t uartFrame);
void OnUartRx(uint16_t *buffer);

#endif	/* UART_H */

