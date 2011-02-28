package ioio.lib.pro;

import ioio.lib.pic.IoioImpl;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * This class can have side effects on the IOIO that will result in 
 * it no longer working. You have been warned.
 * 
 * TODO(arshan) : Contains constants and get/set/and-set/or-set/xor-set methods for the 
 * onboard registers.
 * 
 * TODO(arshan) : Consider also exposing functionality like the BTBridge in this class/package
 *                Who wants to implement gdb for IOIO? 
 *                
 * TODO(arshan) : what about using the ProIO as the underlying (composed) connector in the IOIO? 
 * 
 * @author arshan
 *
 */
public class ProIo extends IoioImpl {
  
    /**
     * return a handle to the stream that talks to the IOIO board.
     * @return
     */
    public OutputStream getOutputStream() {
        return new IoioOutputStream();
    }
    
    public InputStream getInputStream() {
        return new IoioInputStream();
    }

    public class IoioOutputStream extends OutputStream {

        @Override
        public void write(int arg0) throws IOException {
            // TODO Auto-generated method stub
            
        }}
    
    public class IoioInputStream extends InputStream {

        @Override
        public int read() throws IOException {
            // TODO Auto-generated method stub
            return 0;
        }}
    
  
    
}
