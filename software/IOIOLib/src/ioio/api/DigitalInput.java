package ioio.api;

import ioio.api.PeripheralException.InvalidStateException;

public interface DigitalInput extends Pin {
    public Boolean read() throws InvalidStateException;
}
