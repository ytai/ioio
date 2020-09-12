package ioio.bridge;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketTimeoutException;

import purejavacomm.CommPort;
import purejavacomm.CommPortIdentifier;
import purejavacomm.NoSuchPortException;
import purejavacomm.SerialPort;

public class Main {
    private static SerialPort port_ = null;
    private static Socket socket_ = null;

    /**
     * @param args
     * @throws IOException
     * @throws InterruptedException
     */
    public static void main(String[] args) throws InterruptedException,
            IOException {
        System.out.println("IOIO Emulator Bridge, V1.03");
        if (args.length != 1) {
            System.err.println("Usage: java -jar bridge.jar <serial_port>");
            System.err.println("Don't forget to first run:");
            System.err.println("adb forward tcp:4545 tcp:4545");
            System.exit(1);
        }
        System.out.println("Press Ctrl-C at any point to exit");
        System.out.println();
        final String portName = args[0];

        while (true) {
            runBridge(portName);
            Thread.sleep(1000);
        }
    }

    private static void runBridge(String portName) throws InterruptedException,
            IOException {
        try {
            System.err.print("Connecting to IOIO... ");
            port_ = openPort(portName);
            System.err.println("Connected!");
            System.err.print("Connecting to Android application... ");
            socket_ = openSocket();
            System.err.println("Connected!");
            Thread t1 = new BridgeThread(socket_.getInputStream(),
                    port_.getOutputStream(), port_);
            Thread t2 = new BridgeThread(new GracefullyClosingInputStream(
                    port_.getInputStream()), socket_.getOutputStream(), socket_);
            t1.start();
            t2.start();
            System.err.println("Bridge is running...");
            t1.join();
            t2.join();
            System.err.println("Bridge closed.");
        } finally {
            cleanup();
        }
    }

    protected static void cleanup() {
        if (port_ != null) {
            port_.close();
        }
        if (socket_ != null) {
            try {
                socket_.close();
            } catch (IOException e) {
            }
        }
    }

    private static Socket openSocket() {
        while (true) {
            try {
                Socket socket = new Socket("localhost", 4545);
                Thread.sleep(500);

                // This is a hacky test:
                // The Android emulator (or ADB?) will let us open a connection,
                // but then close it as soon as we try to read.
                // So we set a timeout and try to read. If the timeout expires,
                // this is actually a good thing!
                socket.setSoTimeout(10);
                try {
                    socket.getInputStream().read();
                } catch (SocketTimeoutException e) {
                    socket.setSoTimeout(0);
                    return socket;
                }
            } catch (Exception e) {
            }
            // If we got here, we failed to open the socket_. Wait a second and
            // retry.
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e1) {
            }
        }
    }

    private static SerialPort openPort(String portName) {
        SerialPort port = null;
        while (true) {
            try {
                CommPortIdentifier identifier = CommPortIdentifier
                        .getPortIdentifier(portName);
                CommPort commPort = identifier.open(Main.class.getName(), 1000);
                port = (SerialPort) commPort;
                port.enableReceiveThreshold(1);
                port.enableReceiveTimeout(500);
                port.setDTR(false);
                port.setDTR(true);
                Thread.sleep(100);
                return port;
            } catch (NoSuchPortException e) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e1) {
                }
            } catch (Exception e) {
                if (port != null) {
                    port.close();
                }
            }
        }
    }

    private static class BridgeThread extends Thread {
        private final InputStream in_;
        private final OutputStream out_;
        private final Object toClose_;

        public BridgeThread(InputStream in, OutputStream out, Object toClose) {
            in_ = in;
            out_ = out;
            toClose_ = toClose;
        }

        @Override
        public void run() {
            byte[] buf = new byte[1024];
            try {
                int r;
                while ((r = in_.read(buf)) != -1) {
                    out_.write(buf, 0, r);
                }
            } catch (IOException e) {
            } finally {
                try {
                    if (toClose_ instanceof Socket) {
                        ((Socket) toClose_).close();
                    } else if (toClose_ instanceof SerialPort) {
                        ((SerialPort) toClose_).close();
                    }
                } catch (IOException e) {
                }
            }
        }

    }

    // This is a hack:
    // On Windows, PJC will sometimes block a read until its timeout (which is ideally infinite in
    // our case) despite the fact that data is available.
    // The workaround is to set a timeout on the InputStream and to read in a loop until something
    // is actually read.
    // Since a timeout is indistinguishable from an end-of-stream when using the no-argument read(),
    // we set a flag to designate that this is a real
    // close, prior to actually closing, causing the read loop to exit upon the next timeout.
    private static class GracefullyClosingInputStream extends InputStream {
        private final InputStream underlying_;
        private boolean closed_ = false;

        public GracefullyClosingInputStream(InputStream is) {
            underlying_ = is;
        }

        @Override
        public int read(byte[] b) throws IOException {
            while (!closed_) {
                int i = underlying_.read(b);
                if (i > 0) {
                    return i;
                }
            }
            return -1;
        }

        @Override
        public int read(byte[] b, int off, int len) throws IOException {
            while (!closed_) {
                int i = underlying_.read(b, off, len);
                if (i > 0) {
                    return i;
                }
            }
            return -1;
        }

        @Override
        public long skip(long n) throws IOException {
            return underlying_.skip(n);
        }

        @Override
        public int available() throws IOException {
            return underlying_.available();
        }

        @Override
        public void close() throws IOException {
            closed_ = true;
            underlying_.close();
        }

        @Override
        public synchronized void mark(int readlimit) {
            underlying_.mark(readlimit);
        }

        @Override
        public synchronized void reset() throws IOException {
            underlying_.reset();
        }

        @Override
        public boolean markSupported() {
            return underlying_.markSupported();
        }

        @Override
        public int read() throws IOException {
            while (!closed_) {
                int i = underlying_.read();
                if (i >= 0) {
                    return i;
                }
            }
            return -1;
        }
    }
}
