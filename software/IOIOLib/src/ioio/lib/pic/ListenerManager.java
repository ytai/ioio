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
    List<IOIOPacketListener> listeners = new LinkedList<IOIOPacketListener>();

    void disconnectListeners() {
        for (IOIOPacketListener listener : listeners) {
            listener.disconnectNotification();
        }

        IOIOLogger.log("resetting listeners");
        listeners.clear();
    }

    /**
     * Handle an incoming packet from the IOIO.
     */
    public void handlePacket(IOIOPacket packet) {
        // send to my listeners
        // IOIOLogger.log("handle packet : " + packet.toString() + " / " + listeners.size() + " listeners");
        // TODO(arshan): add some filters for message types? we've already case'd it above
        for (IOIOPacketListener listener: listeners) {
            listener.handlePacket(packet);
        }
    }

    public void registerListener(IOIOPacketListener listener) {
        IOIOLogger.log("registering listener");
        if (!listeners.contains(listener)) {
            listeners.add(listener);
        }
    }
}
