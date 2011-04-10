package ioio.lib;

import ioio.api.DigitalInput;
import ioio.api.DigitalOutput;
import ioio.api.SpiChannel;
import ioio.api.exception.ConnectionLostException;
import ioio.api.exception.InvalidOperationException;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * Channel for talking to a SPI device connected to the PeripheralController.
 * @author arshan
 */
public class IOIOSpi implements SpiChannel, IOIOPacketListener{
    
    public static final int SPI_OFF = 0;
    public static final int SPI_31K = 31250;
    public static final int SPI_35K = 35714;
    public static final int SPI_41K = 41667;
    public static final int SPI_50K = 50000;
    public static final int SPI_62K = 62500;
    public static final int SPI_83K = 83333;
    public static final int SPI_125K = 125000;
    public static final int SPI_142K = 142857;
    public static final int SPI_166K = 166667;
    public static final int SPI_200K = 200000;
    public static final int SPI_250K = 250000;
    public static final int SPI_333K = 333333;
    public static final int SPI_500K = 500000;
    public static final int SPI_571K = 571429;
    public static final int SPI_666K = 666667;
    public static final int SPI_800K = 800000;
    public static final int SPI_1M = 1000000;
    public static final int SPI_1_3M = 1333333;
    public static final int SPI_2M = 2000000;
    public static final int SPI_2_2M = 2285714;
    public static final int SPI_2_6M = 2666667;
    public static final int SPI_3_2M = 3200000;
    public static final int SPI_4M = 4000000;
    public static final int SPI_5_3M = 5333333;
    public static final int SPI_8M = 8000000;
    
    SpiMaster master;
    ByteBuffer incoming;  
    
    DigitalOutput selectPin;
    IOIOImpl controller;
    
    /// Keeps track of remaining buffer in firmware.
    private int onIOIOTxBuffer = 0;
    
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
    
    public int read(ByteBuffer receive) throws IOException{
        try {
            return writeRead(null, receive, 0);
        } catch (ConnectionLostException e) {
            throw new IOException("Connection Lost");
        }
    }
    
    public int write(ByteBuffer send) throws IOException{
        try {
            return writeRead(send, null, 0);
        } catch (ConnectionLostException e) {
            throw new IOException("Connection Lost");
        }
    }

    public int writeRead(ByteBuffer buf) throws ConnectionLostException {      
        return writeRead(buf, buf, 0);
    }
    
    @Override
    public void close() throws IOException {
        if (selectPin != null) {
            selectPin.close();
        }
    }

    @Override
    public boolean isOpen() {       
        return master.isOpen();
    }
    
    @Override
    public synchronized int writeRead(ByteBuffer send, ByteBuffer receive, int rxOffset) 
    throws ConnectionLostException {      
        // Prepare for return bytes.
        incoming = receive;
        
        // Send bytes via master
        master.send(selectPin.getPinNumber(), send, receive, rxOffset);

        // wait for response
        try {
            incoming.wait();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // return the response
        return incoming.remaining();
    }
    
    

    @Override
    public void disconnectNotification() {
        try {
            close();
        } catch (IOException e) {
            // ignore
        }          
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
    
    @Override
    public void handlePacket(IOIOPacket packet) {
      switch (packet.message) {
          case Constants.SPI_DATA:  
              if (( packet.payload[0] >> 6 ) == master.getNum()) {
                  // from my SPI master
                  if ((packet.payload[1] & 0x3F) == selectPin.getPinNumber()) {
                      // from my slave, put in the incoming buffer
                      incoming.put(packet.payload, 2, packet.payload.length-2);
                      incoming.notify(); // Notify that the response has arrived.
                  }
              }
              break;
          case Constants.SPI_REPORT_TX_STATUS:
              onIOIOTxBuffer = (((packet.payload[1] & 0xFF) << 6) | (packet.payload[0] & 0xFF) >> 2);
              break;
      }
    }
}
