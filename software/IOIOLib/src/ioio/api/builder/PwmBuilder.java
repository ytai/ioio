package ioio.api.builder;

import ioio.api.DigitalOutput;
import ioio.api.IOIOLib;
import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidOperationException;
import ioio.api.PeripheralException.OutOfResourceException;
import ioio.api.PwmOutput;

/**
 * 
 * @author arshan
 *
 */
public class PwmBuilder {

    private int frequency = 100;
    private DigitalOutput output;
    
    public PwmBuilder setOutput(DigitalOutput out) {
        output = out;
        return this;
    }
    
    public PwmBuilder setFrequency(int frequency) {
        this.frequency = frequency;
        return this;
    }   
    
    public PwmOutput build(IOIOLib controller) 
    throws OutOfResourceException, ConnectionLostException, InvalidOperationException {
        return controller.openPwmOutput(output, frequency);
    }
}
