package ioio.lib.pic;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.InterruptibleChannel;

public class SpiChannel implements SynchronousByteChannel, InterruptibleChannel {

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

    @Override
    public int readWrite(ByteBuffer buf) {
        // TODO Auto-generated method stub
        return 0;
    }

}
