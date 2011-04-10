package ioio.lib.impl;

import java.io.InputStream;
import java.io.OutputStream;

public interface Command {    
    public void deserialize(InputStream stream);
    public void serialize(OutputStream stream);
}
