
package ioio.lib.pic;

import ioio.lib.Input;
import ioio.lib.IoioException.ConnectionLostException;
import ioio.lib.IoioException.InvalidOperationException;
import ioio.lib.Output;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Uart implementation for the IOIO.
 * 
 * @author arshan 
 * TODO(arshan) : consider implementing this as ByteChannel
 */
public class Uart implements IoioPacketListener {

    // Define some known good constants?
    public static final int NO_PARITY = 0;
    public static final int ODD_PARITY = 2; // I'd prefer Odd be 1 for artistic reasons :)                                            
    public static final int EVEN_PARITY = 1;

    // TODO(arshan): what is the highest supported baud?
    public static final int BAUD_2400 = 2400;
    public static final int BAUD_4800 = 4800;
    public static final int BAUD_9600 = 9600;
    public static final int BAUD_19200 = 19200;
    public static final int BAUD_28800 = 28800;
    public static final int BAUD_33600 = 33600;
    public static final int BAUD_38400 = 38400;
    public static final int ONE_STOP_BIT = 1;
    public static final int TWO_STOP_BITS = 2;

    // TODO(arshan): handle this? 
   // public static final float ONE_AND_HALF_STOP_BITS = 1.5f;

    private int baud = 0; // Support all standard bauds
    private int parity = 0; // even/odd/no
    private int stop_bits = 0; // 1/1.5/2
    private int data_bits = 8; // 7/8/9

    private Input<Boolean> rx;
    private Output<Boolean> tx;

    /**
     * This number is the tx buffer status as reported by the Ioio protocol's
     * flow control. That is the number of bytes remaining in the on IOIO
     * transmit buffer.
     */
    private volatile int onIoioTxBuffer = 0;
    private IoioImpl ioio;

    // Uart module on the ioio.
    private int uartNum;
    private int rate;
    private boolean fourX = false;

    // Cached IOIOPackets
    private IoioPacket configure;
    private IoioPacket setRx;
    private IoioPacket setTx;

    // TODO(arshan): these should be bounded
    BlockingQueue<Integer> incoming = new LinkedBlockingQueue<Integer>();
    BlockingQueue<Byte> outgoing = new LinkedBlockingQueue<Byte>();

    private int txPin;
    private int rxPin;

    OutgoingSpooler outgoingSpooler;
    
    Uart(IoioImpl ioio, int module, int rx, int tx, int baud, int parity, int stop)
            throws ConnectionLostException, InvalidOperationException {

        this.stop_bits = stop;
        this.parity = parity;
        this.baud = baud;
        this.rx = ioio.openDigitalInput(rx);
        this.tx = ioio.openDigitalOutput(tx, false);
        this.rxPin = rx;
        this.txPin = tx;
        this.ioio = ioio;
        uartNum = module;
        init();
    }

    private void init() throws ConnectionLostException {
        calculateRates();

        configure = new IoioPacket(Constants.UART_CONFIGURE, new byte[] {
                (byte) (uartNum << 6 | (fourX ? 0x08 : 0x00)
                        | (stop_bits == TWO_STOP_BITS ? 0x40 : 0) | (parity & 0x3)),
                (byte) (rate & 0xFF), 
                (byte) ((rate >> 8) & 0xFF)
        });

        setRx = new IoioPacket(Constants.UART_SET_RX, new byte[] {
                (byte) rxPin, (byte) (0x80 | uartNum)
        });

        setTx = new IoioPacket(Constants.UART_SET_TX, new byte[] {
                (byte) txPin, (byte) (0x80 | uartNum)
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
        ioio.sendPacket(setRx);
        ioio.sendPacket(setTx);       
        
        // We get back a packet of nonsense on init of rx.
        // might be better to fix in firmware? 
        try {
            this.openInputStream().read();
        } catch (IOException e) {
            // 
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

    public DataOutputStream openDataOutputStream() {
        return new DataOutputStream(openOutputStream());
    }

    public DataInputStream openDataInputStream() {
        return new DataInputStream(openInputStream());
    }

    public UARTOutputStream openOutputStream() {
        return new UARTOutputStream();
    }

    public UARTInputStream openInputStream() {
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
                return incoming.take();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return -1;
        }
    }

    
    private static final PacketFramer UART_PACKET_FRAMER = new PacketFramer() {
        @Override
        public IoioPacket frame(byte message, InputStream in) throws IOException {
            switch (message) {
                case Constants.UART_RX:
                    int firstbyte = Bytes.readByte(in); 
                    int length = firstbyte & 0x3F;
                    byte[] payload = new byte[length+2];
                    payload[0] = (byte) firstbyte;
                    Bytes.readFully(in, payload, 1);    
                    return new IoioPacket(message, payload);
                    
                case Constants.UART_CONFIGURE:
                    return new IoioPacket(message, Bytes.readBytes(in, 3));

                case Constants.UART_SET_RX:
                case Constants.UART_SET_TX:
                case Constants.UART_TX_STATUS:
                    return new IoioPacket(message, Bytes.readBytes(in, 2));
            }
            return null;
        }
    };

    
    @Override
    public void handlePacket(IoioPacket packet) {
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
                    onIoioTxBuffer = (( packet.payload[0] >> 2 ) | (packet.payload[1] << 8));
                    IoioLogger.log("set tx buffer remaining to: " + onIoioTxBuffer);
                }
                break;
        }
    }

    
    public void close() throws IOException {
       IoioPacket deconfigure = new IoioPacket(Constants.UART_CONFIGURE, new byte[] {
                (byte) (uartNum << 6 | (fourX ? 0x08 : 0x00)
                        | (stop_bits == TWO_STOP_BITS ? 0x40 : 0) | (parity & 0x3)),
                (byte) ((rate >> 8) & 0xFF)
        });

        try {
            ioio.sendPacket(deconfigure);
            rx.close();
            tx.close();
            ioio.unregisterListener(this);
        } catch (ConnectionLostException e) {
            // If we lose connection here, well mission accomplished.
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
                    try {
                        outgoing.wait();
                    } catch (InterruptedException e) {
                    }
                    int byte_cnt = outgoing.size();
                    if (onIoioTxBuffer < byte_cnt) {
                        byte_cnt = onIoioTxBuffer;
                    }
                    // Abort if there is nothing to do
                    if (byte_cnt > 0) {
                        IoioLogger.log("want to send " + byte_cnt + " bytes to UART");
                        byte[] payload = new byte[byte_cnt + 1];
                        int x;
                        // The limit of onIoioTxBuffer should keep this from
                        // overflowing.
                        payload[0] = (byte) (getUartNum() << 6 | (byte_cnt - 1));
                        
                        // trying to ferret out the issue
                        //payload[0] = (byte) (getUartNum() << 6 | (byte_cnt));
                        for (x = 0; x < byte_cnt; x++) {
                            payload[x + 1] = outgoing.poll();
                            IoioLogger.log("loaded char " + (char)payload[x+1]);
                        }
                        
                        // TODO(arshan): how serious is this race condition?
                        // remember this is set by packet handler (ie. different
                        // thread) we could just let that thread handle it.
                        // onIoioTxBuffer -= byte_cnt;
                        
                        try {
                            IoioPacket pkt =new IoioPacket(Constants.UART_TX, payload);
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
