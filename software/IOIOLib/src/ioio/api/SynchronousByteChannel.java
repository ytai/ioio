package ioio.api;

import ioio.api.PeripheralException.ConnectionLostException;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ByteChannel;

/**
 * Extends ByteChannel, adding synchronous read/write operation. 
 * Any transaction is necessarily bi-directional, with as 
 * many bytes outgoing as there are incoming.
 * 
 * Add to the interfaces ReadableByteChannel and WritableByteChannel, 
 * a writeRead method that take a ByteBuffer whose contents are written
 * and replaced with the incoming bytes.
 * 
 * @author arshan
 *
 */
public interface SynchronousByteChannel extends ByteChannel {
    
    /**
     * Write the contents of the buffer to the channel, replacing each
     * byte with the read value for that transaction.
     * @param buf
     * @return number of bytes transacted
     */
    public int writeRead(ByteBuffer buf) throws IOException, ConnectionLostException;

    /**
     * Writes the remaining bytes from the send buffer, and fills any remaining space
     * in the read buffer. The beginning of the read is offset by @rxOffset number of 
     * bytes.
     * @param send Bytes to send
     * @param receive Bytes received
     * @param rxOffset Number of bytes to offset the beginning of read from write
     * @return number of bytes transacted
     */
    public int writeRead(ByteBuffer send, ByteBuffer receive, int rxOffset)
    throws IOException, ConnectionLostException;
}
