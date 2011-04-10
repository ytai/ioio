
package ioio.lib.impl;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.InvalidOperationException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Uart implementation for the IOIO.
 * 
 * TODO(arshan) : consider changing underlying streams to bytechannel.
 * 
 * @author arshan 
 */
public class IOIOUart implements IOIOPacketListener, Uart {
  
    private int baud = 0; 
    private int parity = 0; 
    private int stop_bits = 0;
    private int data_bits = 8;

    private DigitalInput rx;
    private DigitalOutput tx;

    /**
     * This number is the tx buffer status as reported by the Ioio protocol's
     * flow control. That is the number of bytes remaining in the on IOIO
     * transmit buffer.
     */
    private volatile int onIoioTxBuffer = 0;
    private IOIOImpl ioio;

    // Uart module on the ioio.
    private int uartNum;
    private int rate;
    private boolean fourX = false;

    // Cached IOIOPackets
    private IOIOPacket configure;
    private IOIOPacket setRx;
    private IOIOPacket clearChangeNotify;
    private IOIOPacket setTx;

    // TODO(arshan): these should be bounded
    BlockingQueue<Integer> incoming = new LinkedBlockingQueue<Integer>();
    BlockingQueue<Byte> outgoing = new LinkedBlockingQueue<Byte>();

    OutgoingSpooler outgoingSpooler;
    
    IOIOUart(IOIOImpl ioio, DigitalInput rx, DigitalOutput tx, int baud, int parity, int stop)
            throws ConnectionLostException, InvalidOperationException {

        this.stop_bits = stop;
        this.parity = parity;
        this.baud = baud;
        this.rx = rx;
        this.tx = tx;
        this.ioio = ioio;
        uartNum = ioio.allocateUart();
        init();
    }

    private void init() throws ConnectionLostException {
        calculateRates();

        configure = new IOIOPacket(Constants.UART_CONFIGURE, new byte[] {
                (byte) (uartNum << 6 
                        | (fourX ? 0x08 : 0x00)
                        | (stop_bits == TWO_STOP_BITS ? 0x04 : 0x00) 
                        | (parity & 0x03)),
                (byte) (rate & 0xFF), 
                (byte) ((rate >> 8) & 0xFF)
        });

        setRx = new IOIOPacket(Constants.UART_SET_RX, new byte[] {
                (byte) rx.getPinNumber(), (byte) (0x80 | uartNum)
        });

        clearChangeNotify = new IOIOPacket( Constants.SET_CHANGE_NOTIFY,
                new byte[]{
                       (byte) (rx.getPinNumber() << 2)
                }
        );
        
        setTx = new IOIOPacket(Constants.UART_SET_TX, new byte[] {
                (byte) tx.getPinNumber(), (byte) (0x80 | uartNum)
        });

        
        
        outgoingSpooler = new OutgoingSpooler();
        outgoingSpooler.start();
       
        ioio.getFramerRegistry().registerFramer((byte) Constants.UART_CONFIGURE, UART_PACKET_FRAMER);
        ioio.getFramerRegistry().registerFramer((byte) Constants.UART_RX, UART_PACKET_FRAMER);
        ioio.getFramerRegistry().registerFramer((byte) Constants.UART_SET_RX, UART_PACKET_FRAMER);
        ioio.getFramerRegistry().registerFramer((byte) Constants.UART_SET_TX, UART_PACKET_FRAMER);
        ioio.getFramerRegistry().registerFramer((byte) Constants.UART_TX_STATUS, UART_PACKET_FRAMER);
        ioio.registerListener(this);
        
        // Since the rx and tx pins are initialized as DigitalI/O pins we
        // already have
        // them setup to be used as rx/tx here.
        ioio.sendPacket(configure);
        ioio.sendPacket(clearChangeNotify);
        ioio.sendPacket(setRx);
        ioio.sendPacket(setTx);       
        
        // We get back a packet of nonsense on init of rx.
        // might be better to fix in firmware? 
        try {
            this.openInputStream().read();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void calculateRates() {
        // TODO(arshan) : update this setting to real values
        fourX = baud >= 38400 ? true : false;
        rate = calculateUartRate(baud, fourX);
    }

    private int calculateUartRate(int baud, boolean fourX) {
        long numerator = fourX ? 4000000 : 1000000;
        return (int) (Math.floor(numerator / baud) - 1);
    }

    public OutputStream openOutputStream() {
        return new UARTOutputStream();
    }

    public InputStream openInputStream() {
        return new UARTInputStream();
    }
   
    public class UARTOutputStream extends OutputStream {
        @Override
        public void write(int val) throws IOException {
            outgoing.offer((byte) val);
            synchronized(outgoing){
                outgoing.notifyAll();
            }
        }
    }

    public class UARTInputStream extends InputStream {
        @Override
        public int read() throws IOException {
            try {
                int x = 0;
                x = incoming.take();
                return x;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return -1;
        }
    }

    private static final PacketFramer UART_PACKET_FRAMER = new PacketFramer() {
        @Override
        public IOIOPacket frame(byte message, InputStream in) throws IOException {
            switch (message) {
                case Constants.UART_RX:
                    int firstbyte = Bytes.readByte(in); 
                    int length = firstbyte & 0x3F;
                    byte[] payload = new byte[length+2];
                    payload[0] = (byte) firstbyte;
                    Bytes.readFully(in, payload, 1);    
                    return new IOIOPacket(message, payload);
                    
                case Constants.UART_CONFIGURE:
                    return new IOIOPacket(message, Bytes.readBytes(in, 3));

                case Constants.UART_SET_RX:
                case Constants.UART_SET_TX:
                case Constants.UART_TX_STATUS:
                    return new IOIOPacket(message, Bytes.readBytes(in, 2));
            }
            return null;
        }
    };
    
    @Override
    public void handlePacket(IOIOPacket packet) {
        switch (packet.message) {
            case Constants.UART_RX:
                // received bytes from ioio.
                if ((packet.payload[0] >> 6) == uartNum) {
                    // TODO(arshan): consider our own buffer implementation for
                    // perf. ie. circular byte[]
                    // lets us use System.arrayCopy() ...
                    for (int x = 1; x < packet.payload.length; x++) {
                        incoming.offer((int) packet.payload[x]);
                    }
                }
                break;

            case Constants.UART_SET_RX:
            case Constants.UART_SET_TX:
            case Constants.UART_CONFIGURE:
                // consider catching these to get the confirmations.
                break;

            case Constants.UART_TX_STATUS:
                // tells us how many bytes remain in the tx buffer on IOIO
                if ((packet.payload[0] & 0x3) == uartNum) {
                    onIoioTxBuffer = (((packet.payload[1] & 0xFF) << 6) | (packet.payload[0] & 0xFF) >> 2);
                }
                break;
        }
    }
    
    public void close() throws IOException {
       IOIOPacket deconfigure = new IOIOPacket(Constants.UART_CONFIGURE, new byte[] {
                (byte) (uartNum << 6 | (fourX ? 0x08 : 0x00)
                        | (stop_bits == TWO_STOP_BITS ? 0x04 : 0) | (parity & 0x3)),
                (byte) ((rate >> 8) & 0xFF)
        });

        try {
            ioio.deallocateUart(uartNum);
            ioio.sendPacket(deconfigure);
            rx.close();
            tx.close();
            ioio.unregisterListener(this);         
        } catch (ConnectionLostException e) {
            // disconnected, all state will be reset when next connected so
            // safe to ignore sequence 
        }   
    }
    
    public int getUartNum() {
        return uartNum;
    }

    @Override
    public void disconnectNotification() {
        // close();
    }

    public class OutgoingSpooler extends Thread {
        private volatile boolean running = true;

        public void run() {
            while (running) {
                // Block on the outgoing queue
                // TODO(arshan): could throttle a little so that we send more
                // bytes per send
                synchronized (outgoing) {                   
                    if (outgoing.size() == 0) {
                        try {
                            outgoing.wait();
                        } catch (InterruptedException e) {
                        }   
                    }   
                    
                    int byte_cnt = outgoing.size();
                    
                    if (onIoioTxBuffer < byte_cnt) {
                        byte_cnt = onIoioTxBuffer;
                    }
                    
                    // Proto allows max of 64 bytes per rx packet.
                    byte_cnt = Math.max(64, byte_cnt);
                    
                    // Abort if there is nothing to do
                    if (byte_cnt > 0) {
                        byte[] payload = new byte[byte_cnt + 1];
                        int x;
                        // The limit of onIoioTxBuffer should keep this from
                        // overflowing.
                        payload[0] = (byte) (getUartNum() << 6 | (byte_cnt - 1));
                        
                        for (x = 0; x < byte_cnt; x++) {
                            payload[x + 1] = outgoing.poll();
                            
                        }
                        
                        // TODO(arshan) : consider this.
                        // This number is also set by the incoming handler 
                        // based on the actual number remaining. An error condition
                        // could be that due to timing we decrement an already decremented
                        // number that has been returned from the board. 
                        onIoioTxBuffer -= byte_cnt;
                        
                        try {
                            IOIOPacket pkt = new IOIOPacket(Constants.UART_TX, payload);
                            pkt.log();
                            ioio.sendPacket(pkt);
                        } catch (ConnectionLostException e) {
                            running = false;
                        }
                    }
                }
            }
        }

        public void halt() {
            running = false;
            synchronized (outgoing) {
                outgoing.notifyAll();
            }
        }
    }
 }
