package ioio.lib;

import java.util.HashSet;
import java.util.Set;

/**
 * Manages listeners.
 *
 * @author arshan
 * @author birmiwal
 */
public class ListenerManager {
    Set<IOIOPacketListener> listeners = new HashSet<IOIOPacketListener>();

    public void disconnectListeners() {
        for (IOIOPacketListener listener : listeners) {
            listener.disconnectNotification();
        }
        listeners.clear();
    }

    /**
     * Handle an incoming packet from the IOIO.
     */
    public void handlePacket(IOIOPacket packet) {
        for (IOIOPacketListener listener: listeners) {
            listener.handlePacket(packet);
        }
    }

    public void registerListener(IOIOPacketListener listener) {
        if (!listeners.contains(listener)) {
            listeners.add(listener);
        }
    }
    
    public void unregisterListener(IOIOPacketListener listener) {
        listeners.remove(listener);
    }
}
