package ioio.lib;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class Uart {

	// Define some known good constants? 
	public static final int NO_PARITY = 0;
	public static final int ODD_PARITY = 1;
	public static final int EVEN_PARITY = 2;

	public static final int BAUD_2400 = 2400;
	public static final int BAUD_4800 = 4800;
	public static final int BAUD_9600 = 9600;
	public static final int BAUD_19200 = 19200;
	public static final int BAUD_28800 = 28800;
	public static final int BAUD_33600 = 33600;
	
	public static final float ONE_STOP_BIT = 1;
	public static final float TWO_STOP_BITS = 2;
	public static final float ONE_AND_HALF_STOP_BITS = 1.5f; 
	
	// Support all standard bauds
	private int baud = 0;
	
	// even/odd/no
	private int parity = 0;
	
	// 1/1.5/2
	private int stop_bits = 0;
	
	// 7/8/9
	private int data_bits = 0;
	
	public Uart(int Rx, int Tx) {		
		//TBI waiting on protocol spec
	}
	
	
	public UARTOutputStream openOutputStream() {
		return new UARTOutputStream(this);
	}
	
	public UARTInputStream openInputStream() {
		return new UARTInputStream(this);
	}
	
	// override the input/output streams? 
	public void close() {
		
	}
	
	public class UARTOutputStream extends OutputStream {

		protected UARTOutputStream(Uart uart) {
			
		}
		
		@Override
		public void write(int arg0) throws IOException {
			// TODO Auto-generated method stub
			
		}
		
	}

	public class UARTInputStream extends InputStream {
		
		protected UARTInputStream(Uart uart) {			
		}
		
		@Override
		public int read() throws IOException {
			// TODO Auto-generated method stub
			return 0;
		}		
	}
	

}
