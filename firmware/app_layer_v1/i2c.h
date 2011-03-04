#ifndef __I2C_H__
#define __I2C_H__


void I2CInit();
// rate is 0:off 1:100KHz, 2:400KHz, 3:1MHz
void I2CConfigMaster(int i2c_num, int rate, int smbus_levels);
void I2CWriteRead(int i2c_num, int addr, const void* data, int data_bytes,
                  int read_bytes, int ack_last_read);


#endif  // __I2C_H__
