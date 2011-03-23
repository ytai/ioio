package ioio.api;

import java.io.Closeable;
import java.io.InputStream;
import java.io.OutputStream;

public interface Uart extends Closeable {
    
    // Consider enums
    public static final int NO_PARITY = 0;
    public static final int ODD_PARITY = 2;                                    
    public static final int EVEN_PARITY = 1;
    
    public static final int BAUD_1200 = 1200;
    public static final int BAUD_2400 = 2400;
    public static final int BAUD_4800 = 4800;
    public static final int BAUD_9600 = 9600;
    public static final int BAUD_19200 = 19200;
    public static final int BAUD_28800 = 28800;
    public static final int BAUD_31250 = 31250;
    public static final int BAUD_33600 = 33600;
    public static final int BAUD_38400 = 38400;
    public static final int BAUD_56000  = 56000;
    public static final int BAUD_57600 = 57600;
    public static final int BAUD_115200 = 115200;
    public static final int BAUD_128000 = 128000;
    public static final int BAUD_153600 = 153600;
    public static final int BAUD_256000 = 256000; // probably not? 

    public static final int ONE_STOP_BIT = 1;
    public static final int TWO_STOP_BITS = 2;

   public InputStream openInputStream();
   public OutputStream openOutputStream();
}
