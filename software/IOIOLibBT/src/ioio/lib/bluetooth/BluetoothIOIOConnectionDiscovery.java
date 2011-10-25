package ioio.lib.bluetooth;

import ioio.lib.util.IOIOConnectionDiscovery;

import java.util.Collection;
import java.util.Set;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.util.Log;

public class BluetoothIOIOConnectionDiscovery implements
		IOIOConnectionDiscovery {

	private static final String TAG = "BluetoothIOIOConnectionDiscovery";

	@Override
	public void getSpecs(Collection<IOIOConnectionSpec> result) {
		try {
			final BluetoothAdapter adapter = BluetoothAdapter
					.getDefaultAdapter();
			Set<BluetoothDevice> bondedDevices = adapter.getBondedDevices();
			for (BluetoothDevice device : bondedDevices) {
				if (device.getName().startsWith("IOIO")) {
					result.add(new IOIOConnectionSpec(
							BluetoothIOIOConnection.class.getName(),
							new Object[] { device.getName(),
									device.getAddress() }));
				}
			}
		} catch (SecurityException e) {
			Log.e(TAG,
					"Did you forget to declare uses-permission of android.permission.BLUETOOTH?");
			throw e;
		}
	}
}
