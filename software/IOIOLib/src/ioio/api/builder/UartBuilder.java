package ioio.api.builder;

import ioio.api.DigitalInput;
import ioio.api.DigitalOutput;
import ioio.api.IOIOLib;
import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidOperationException;
import ioio.api.Uart;

/**
 * Convenience class for building Uart connections.
 * 
 * <pre>
 * Uart myUart = new UartBuilder(ioio)
 *              .setRx(ioio.openDigitalInput(11))
 *              .setTx(ioio.openDigitalOutput(12, false))
 *              .setBaud(19200)
 *              .build();
 * </pre>
 * 
 * @author arshan
 *
 */
public class UartBuilder {

    // defaults
    int baud = 9600;
    int stopbits = 1;
    int parity = 0;
    
    IOIOLib controller;
    DigitalInput rx;
    DigitalOutput tx;
    
    public UartBuilder(IOIOLib controller) {
        this.controller = controller;
    }
    
    public UartBuilder setRx(int rxPin) {
        try {
            rx = controller.openDigitalInput(rxPin);
        } catch (ConnectionLostException e) {           
            e.printStackTrace();
        } catch (InvalidOperationException e) {
            e.printStackTrace();
        }
        return this;
    }
    
    public UartBuilder setTx(int txPin) {
        try {
            tx = controller.openDigitalOutput(txPin, false);
        } catch (ConnectionLostException e) {
            e.printStackTrace();
        } catch (InvalidOperationException e) {
            e.printStackTrace();
        }
        return this;
    }
    
    public UartBuilder setRx(DigitalInput in){
        rx = in;
        return this;
    }
    
    public UartBuilder setTx(DigitalOutput out){
        tx = out;
        return this;
    }
    
    public UartBuilder setBaud(int baud){
        this.baud = baud;
        return this;
    }
    
    public UartBuilder setParity(int parity){
        this.parity = parity;
        return this;
    }
    
    public UartBuilder setStopBits(int stopbits){
        this.stopbits = stopbits;
        return this;
    }
    
    public Uart build() 
    throws ConnectionLostException, InvalidOperationException {
        return controller.openUart(rx, tx, baud, parity, stopbits);
    }
}
