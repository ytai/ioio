#ifndef __UART_H__
#define __UART_H__

void UARTInit();
void UARTConfig(int uart, int rate, int speed4x, int two_stop_bits, int parity);
void UARTTransmit(int uart, const void* data, int size);
void UARTTasks();


#endif  // __UART_H__
