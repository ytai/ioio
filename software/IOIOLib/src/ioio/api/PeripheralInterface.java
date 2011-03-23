package ioio.api;

import ioio.api.PeripheralException.OperationAbortedException;
import ioio.lib.IOIOImpl;

import java.net.SocketException;

/**
 * Factory class for getting a handle to the IOIO
 *
 * @author arshan
 */
public class PeripheralInterface {

    private static IOIOLib singleton = null;

    public static IOIOLib waitForController() throws OperationAbortedException, SocketException {
        if (singleton == null) {
            singleton = new IOIOImpl();
        }
        singleton.waitForConnect();
        return singleton;
    }
    
}
