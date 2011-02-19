package ioio.lib.pic;

import java.io.IOException;
import java.io.InputStream;

public interface PacketFramer {
    public IOIOPacket frame(byte message, InputStream in) throws IOException;
}
