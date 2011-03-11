package ioio.lib.pic;

import java.nio.ByteBuffer;
import java.nio.channels.ByteChannel;

/**
 * A ByteChannel that has synchronous read/write operation. 
 * That is any transaction is necessarily bi-directional, with as 
 * many bytes outgoing as there are incoming.
 * 
 * Add to the interfaces ReadableByteChannel and WritableByteChannel, 
 * a readWrite method that take a ByteBuffer whose contents are written
 * and replaced with the incoming bytes.
 * 
 * @author arshan
 *
 */
public interface SynchronousByteChannel extends ByteChannel {
    public int readWrite(ByteBuffer buf);
}
