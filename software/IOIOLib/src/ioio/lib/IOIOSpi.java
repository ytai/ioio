package ioio.lib;

import ioio.api.DigitalInput;
import ioio.api.DigitalOutput;
import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidOperationException;
import ioio.api.SpiChannel;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * Channel for talking to a SPI device connected to the PeripheralController.
 * @author arshan
 */
public class IOIOSpi implements SpiChannel, IOIOPacketListener{
    
    SpiMaster master;
    ByteBuffer incoming;  
    
    DigitalOutput selectPin;
    IOIOImpl controller;
    
    public IOIOSpi(
            DigitalInput misoPin, 
            DigitalOutput mosiPin, 
            DigitalOutput clkPin, 
            DigitalOutput selectPin,
            int speed,
            IOIOImpl controller) 
    throws ConnectionLostException, InvalidOperationException {
        master = new SpiMaster(misoPin, mosiPin, clkPin, controller);
        master.init(speed);
        controller.registerListener(this);
        controller.getFramerRegistry().registerFramer((byte) Constants.SPI_DATA, SPI_PACKET_FRAMER);
        controller.getFramerRegistry().registerFramer((byte) Constants.SPI_REPORT_TX_STATUS, SPI_PACKET_FRAMER);            
    }
    
    @Override
    public int read(ByteBuffer receive) throws IOException{
        try {
            return writeRead(null, receive, 0);
        } catch (ConnectionLostException e) {
            throw new IOException("Connection Lost");
        }
    }

    @Override
    public int write(ByteBuffer send) throws IOException{
        try {
            return writeRead(send, null, 0);
        } catch (ConnectionLostException e) {
            throw new IOException("Connection Lost");
        }
    }

    @Override
    public int writeRead(ByteBuffer buf) throws ConnectionLostException {      
        return writeRead(buf, buf, 0);
    }
    

    @Override
    public void close() throws IOException {
        // release select pin?
    }

    @Override
    public boolean isOpen() {       
        return master.isOpen();
    }

    
    @Override
    public int writeRead(ByteBuffer send, ByteBuffer receive, int rxOffset) 
    throws ConnectionLostException {
        // prepare for the return bytes
        incoming = receive;
        // send bytes via master
        master.send(selectPin.getPinNumber(), send, rxOffset);
      
        return 0;
    }

    
    
    @Override
    public void handlePacket(IOIOPacket packet) {
      switch (packet.message) {
          case Constants.SPI_DATA:
              
              break;
          case Constants.SPI_REPORT_TX_STATUS:
              // TODO use this
              break;
      }
    }

    @Override
    public void disconnectNotification() {
        // TODO Auto-generated method stub
        
    }
    
    private static final PacketFramer SPI_PACKET_FRAMER = new PacketFramer() {
        @Override
        public IOIOPacket frame(byte message, InputStream in) throws IOException {
           switch (message) {
               case Constants.SPI_DATA:
                   int bytes = in.read();
                   byte[] data = new byte[bytes&0x3F];
                   data[0] = (byte)bytes;
                   Bytes.readFully(in, data, 1);
                   return new IOIOPacket(message, data);
               case Constants.SPI_REPORT_TX_STATUS:
                   return new IOIOPacket(message, Bytes.readBytes(in, 2));
           }
           return null;
        }
    };

  
    
    
}
