#ifndef __SPI_H__
#define __SPI_H__


void SPIInit();
void SPIConfig(int spi, int scale, int div, int smp_end, int clk_edge,
               int clk_pol);
void SPITasks();
void SPIReportTxStatus(int spi);
void SPITransmit(int spi, int dest, const void* data, int size);


#endif // __SPI_H__
