package ioio.lib.pic;

/**
 * Handles packets for processing.
 *
 * @author arshan
 * @author birmiwal
 */
public interface IoioPacketListener {
    public void handlePacket(IoioPacket packet);
    public void disconnectNotification();
}
