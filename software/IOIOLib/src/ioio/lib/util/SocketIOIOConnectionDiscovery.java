package ioio.lib.util;

import ioio.lib.api.IOIOFactory;
import ioio.lib.impl.SocketIOIOConnection;

import java.util.Collection;

public class SocketIOIOConnectionDiscovery implements IOIOConnectionDiscovery {

	@Override
	public void getSpecs(Collection<IOIOConnectionSpec> result) {
		result.add(new IOIOConnectionSpec(SocketIOIOConnection.class.getName(),
				new Object[] { new Integer(IOIOFactory.IOIO_PORT) }));
	}
}
