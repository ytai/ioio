package ioio.api;

import java.nio.channels.InterruptibleChannel;

public interface SpiChannel extends SynchronousByteChannel, InterruptibleChannel {
 
    // Warning, anti-pattern
    public static final int SPI_OFF = 0;
    public static final int SPI_31K = 31250;
    public static final int SPI_35K = 35714;
    public static final int SPI_41K = 41667;
    public static final int SPI_50K = 50000;
    public static final int SPI_62K = 62500;
    public static final int SPI_83K = 83333;
    public static final int SPI_125K = 125000;
    public static final int SPI_142K = 142857;
    public static final int SPI_166K = 166667;
    public static final int SPI_200K = 200000;
    public static final int SPI_250K = 250000;
    public static final int SPI_333K = 333333;
    public static final int SPI_500K = 500000;
    public static final int SPI_571K = 571429;
    public static final int SPI_666K = 666667;
    public static final int SPI_800K = 800000;
    public static final int SPI_1M = 1000000;
    public static final int SPI_1_3M = 1333333;
    public static final int SPI_2M = 2000000;
    public static final int SPI_2_2M = 2285714;
    public static final int SPI_2_6M = 2666667;
    public static final int SPI_3_2M = 3200000;
    public static final int SPI_4M = 4000000;
    public static final int SPI_5_3M = 5333333;
    public static final int SPI_8M = 8000000;
    
}
