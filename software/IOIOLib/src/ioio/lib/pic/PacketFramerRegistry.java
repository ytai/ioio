package ioio.lib.pic;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

public class PacketFramerRegistry implements PacketFramer {
    Map<Byte, PacketFramer> framers = new HashMap<Byte, PacketFramer>();

    @Override
    public IOIOPacket frame(byte message, InputStream in) throws IOException {
        return framers.containsKey(message) ? framers.get(message).frame(message, in) : null;
    }

    public boolean registerFramer(byte message, PacketFramer framer) {
        if (framers.containsKey(message)) {
            return false;
        }
        framers.put(message, framer);
        return true;
    }

    public void reset() {
        framers.clear();
    }
}
