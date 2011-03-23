package ioio.api;

import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidStateException;

public interface DigitalOutput extends Pin {    
    public void write(Boolean val) throws InvalidStateException, ConnectionLostException;
}
