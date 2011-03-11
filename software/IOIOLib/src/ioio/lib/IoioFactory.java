package ioio.lib;

import ioio.lib.pic.IoioImpl;

/**
 * Factory class for getting a handle to the IOIO
 * @author arshan
 *
 */
public class IoioFactory {

    private static Ioio singleton = null;

    public static Ioio makeIoio() {
        if (singleton == null) {
            singleton = new IoioImpl();
        }
        return singleton;
    }
}
