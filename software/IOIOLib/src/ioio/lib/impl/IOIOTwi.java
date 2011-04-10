package ioio.lib.impl;

import ioio.lib.api.TwiChannel;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

public class IOIOTwi implements TwiChannel, IOIOPacketListener {

    // The available speed in the IOIO implementation of Twi.
    public static final int SPEED_OFF = 0;
    public static final int SPEED_100K = 1;
    public static final int SPEED_400K = 2;
    public static final int SPEED_1M = 3;
    
    private static final PacketFramer TWI_PACKET_FRAMER = new PacketFramer() {
        @Override
        public IOIOPacket frame(byte message, InputStream in) throws IOException {
           switch (message) {
               case Constants.TWI_RESULT:
                   int bytes = in.read();
                   byte[] data = new byte[bytes&0x3F];
                   data[0] = (byte)bytes;
                   Bytes.readFully(in, data, 1);
                   return new IOIOPacket(message, data);
               case Constants.TWI_REPORT_TX_STATUS:
                   return new IOIOPacket(message, Bytes.readBytes(in, 2));
           }
           return null;
        }
    }; 
    
    @Override
    public void handlePacket(IOIOPacket packet) {
      switch (packet.message) {
          case Constants.TWI_RESULT:
              
              break;
          case Constants.TWI_REPORT_TX_STATUS:
              
              break;
      }
    }

    @Override
    public void disconnectNotification() {
        // TODO Auto-generated method stub
        
    }

    @Override
    public int read(ByteBuffer arg0) throws IOException {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public void close() throws IOException {
        // TODO Auto-generated method stub
        
    }

    @Override
    public boolean isOpen() {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public int write(ByteBuffer arg0) throws IOException {
        // TODO Auto-generated method stub
        return 0;
    }
}
