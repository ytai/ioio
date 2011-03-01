#ifndef __SPI_H__
#define __SPI_H__


void SPIInit();
void SPIConfigMaster(int spi_num, int scale, int div, int smp_end, int clk_edge,
                     int clk_pol);
void SPITasks();
void SPIReportTxStatus(int spi_num);
void SPITransmit(int spi_num, int dest, const void* data, int size, int pad_tx,
                 int trim_rx);


#endif // __SPI_H__
