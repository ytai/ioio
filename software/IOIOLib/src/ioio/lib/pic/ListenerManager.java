package ioio.lib.pic;

import java.util.LinkedList;
import java.util.List;

/**
 * Manages listeners.
 *
 * @author arshan
 * @author birmiwal
 */
public class ListenerManager {
    List<IoioPacketListener> listeners = new LinkedList<IoioPacketListener>();

    public void disconnectListeners() {
        for (IoioPacketListener listener : listeners) {
            listener.disconnectNotification();
        }

        IoioLogger.log("resetting listeners");
        listeners.clear();
    }

    /**
     * Handle an incoming packet from the IOIO.
     */
    public void handlePacket(IoioPacket packet) {
        // send to my listeners
        // IOIOLogger.log("handle packet : " + packet.toString() + " / " + listeners.size() + " listeners");
        for (IoioPacketListener listener: listeners) {
            listener.handlePacket(packet);
        }
    }

    public void registerListener(IoioPacketListener listener) {
        IoioLogger.log("registering listener " + listener.toString());
        if (!listeners.contains(listener)) {
            listeners.add(listener);
        }
    }
    
    public void unregisterListener(IoioPacketListener listener) {
        listeners.remove(listener);
    }
}
