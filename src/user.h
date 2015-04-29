/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/


#define NUM_CAN_BUFFERS                     32                                  // max 32
#define CANTXSID                            0x7DF                               // OBD TX SID
#define CANRXSID                            0x7E8                               // OBD RX SID
#define ALLSID                              0                                   // Let pass every incoming SID
#define FILTERSID                           0x7FF                               // Only pass the filter SID
#define CANDLC                              8                                   // DLC CAN message
#define UTXNUM                              0                                   // UART Tx buffer index for number aditional of data bytes
#define UTXMODE                             1                                   // UART Tx buffer index for MODE
#define UTXPID                              2                                   // UART Tx buffer index for PID
#define DATA1                               3                                   // UART Tx buffer index for CAN Data 1 byte
#define DATA2                               4                                   // UART Tx buffer index for CAN Data 2 byte
#define DATA3                               5                                   // UART Tx buffer index for CAN Data 3 byte
#define DATA4                               6                                   // UART Tx buffer index for CAN Data 4 byte
#define UARTDMACNT                          DMA2CNT                             // Bytes to be sent by the UART
#define URXMODE                             0                                   // UART Rx buffer index  for MODE
#define URXPID                              1                                   // UART Rx buffer index  for PID
#define LEDRED                              LATBbits.LATB15                     // System status LED
#define CANRXLED                            LATBbits.LATB11                     // Rx LED for CAN
#define CANTXLED                            LATBbits.LATB10                     // Tx LED for CAN
#define CANRXBUFFi                          C1FIFObits.FNRB                     // Buffer active for reading on i time
#define CANRXLEDBLINK                       T2CONbits.TON = 1                   // Timer 2 on/off
#define CANTXLEDBLINK                       T3CONbits.TON = 1                   // Timer 3 on/off
#define SENDCANBUFF0                        C1TR01CONbits.TXREQ0 = 1            // Send CAN buffer 0
#define SENDCANBUFF1                        C1TR01CONbits.TXREQ1 = 1            // Send CAN buffer 1
#define UARTBUSY                            U1STAbits.UTXBF !=0                 // 0 if UART Free, 1 if UART Busy
#define CANOPMODE                           C1CTRL1bits.OPMODE                  // CAN current operation mode
#define NORMALOP                            0                                   // CAN normal operation mode
#define DISSABLEOP                          1                                   // CAN dissable operation mode
#define LOOPBACKOP                          2                                   // CAN loopback operation mode


/******************************************************************************/
/* User Global variables Variables                                            */
/******************************************************************************/


extern uint16_t CANBuff[NUM_CAN_BUFFERS][8] __attribute__((aligned(NUM_CAN_BUFFERS * 16)));

extern uint16_t UTxBuff[7];//extern uint8_t UTxBuff;

extern uint16_t URxBuffA[2];//extern uint8_t URxBuffA[2];
extern uint16_t URxBuffB[2];//extern uint8_t URxBuffB[2];


/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/


void InitApp(void);                         // I/O and Peripheral Initialization
void ConfigureLEDS(void);                   // LEDs configuration
void SetCANMode(uint16_t mode);             // Set desired CAN mode
void ClearCANRxBuffer(uint16_t buffer);     // Clear the full bit of "buffer" when it is read
void SendUARTNum(uint16_t num);             // Send by the UART "num" Bytes
void SendUART(void);                        // Send an UART Buffer already stored in memory