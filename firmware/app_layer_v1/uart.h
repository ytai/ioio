#ifndef __UART_H__
#define __UART_H__

void UARTInit();
void UARTConfig(int uart_num, int rate, int speed4x, int two_stop_bits,
                int parity);
void UARTTransmit(int uart_num, const void* data, int size);
void UARTTasks();
void UARTReportTxStatus(int uart);


#endif  // __UART_H__
