package ioio.api;

import ioio.api.PeripheralException.InvalidStateException;

public interface AnalogInput extends Pin {
    public Float getVoltage() throws InvalidStateException;
    public Float getReference() throws InvalidStateException;
    public Float read() throws InvalidStateException;
}
