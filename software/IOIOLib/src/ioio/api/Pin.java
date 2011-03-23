package ioio.api;

import java.io.Closeable;

public interface Pin extends Closeable {

    // wait for change requirements go here? 
    public int getPinNumber();
    
}
