package ioio.lib.pic;

import java.io.IOException;
import java.io.InputStream;

public class PacketFramers {

    public static PacketFramer getNBytePacketFramerFor(final byte messageType, final int numBytes) {
        return new PacketFramer() {
                @Override
                public IoioPacket frame(byte message, InputStream in) throws IOException {
                    assert(message == messageType);
                    return new IoioPacket(message, Bytes.readBytes(in, numBytes));
                }
            };
    }
}
