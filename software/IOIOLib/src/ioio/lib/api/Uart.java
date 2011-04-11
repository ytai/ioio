 /*
  * Copyright 2011. All rights reserved.
  *  
  * 
  * Redistribution and use in source and binary forms, with or without modification, are
  * permitted provided that the following conditions are met:
  * 
  *    1. Redistributions of source code must retain the above copyright notice, this list of
  *       conditions and the following disclaimer.
  * 
  *    2. Redistributions in binary form must reproduce the above copyright notice, this list
  *       of conditions and the following disclaimer in the documentation and/or other materials
  *       provided with the distribution.
  * 
  * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
  * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
  * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * 
  * The views and conclusions contained in the software and documentation are those of the
  * authors and should not be interpreted as representing official policies, either expressed
  * or implied.
  */
package ioio.lib.api;

import java.io.Closeable;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Simple interface to a UART connection.
 * 
 * @author arshan
 */
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

   public InputStream getInputStream();
   public OutputStream getOutputStream();
}
