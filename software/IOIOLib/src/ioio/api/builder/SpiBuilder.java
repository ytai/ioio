package ioio.api.builder;

import ioio.api.DigitalInput;
import ioio.api.DigitalOutput;
import ioio.api.IOIOLib;
import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidOperationException;
import ioio.api.SpiChannel;

/**
 * 
 * @author arshan
 *
 */
public class SpiBuilder {

    IOIOLib controller;
    DigitalInput miso;
    DigitalOutput mosi;
    DigitalOutput clk;
    DigitalOutput select;
    
    int speed;
    
    public SpiBuilder(IOIOLib controller) {
        this.controller = controller;
    }
    
    public SpiBuilder setMiso(DigitalInput input) {
        miso = input;
        return this;
    }
    
    public SpiBuilder setMosi(DigitalOutput output) {
        mosi = output;
        return this;
    }
    
    public SpiBuilder setClk(DigitalOutput output) {
        clk = output;
        return this;
    }
    
    public SpiBuilder setSlaveSelect(DigitalOutput output) {
        select = output;
        return this;
    }
    
    public SpiBuilder setSpeed(int speed) {
        this.speed = speed;
        return this;
    }
    
    public SpiChannel build() throws ConnectionLostException, InvalidOperationException {
        return controller.openSpi(miso, mosi, clk, select, speed);
    }
}
