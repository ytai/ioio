package ioio.connection_tester;

import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOConnectionManager.IOIOConnectionThreadProvider;
import ioio.lib.util.IOIOConnectionManager.Thread;
import ioio.lib.util.android.AndroidIOIOConnectionManager;
import android.app.Activity;
import android.os.Bundle;

public class IOIOConnectionTesterActivity extends Activity implements IOIOConnectionThreadProvider {
	static {
		IOIOConnectionRegistry
				.addBootstraps(new String[] {
						"ioio.lib.impl.SocketIOIOConnectionBootstrap",
						"ioio.lib.android.accessory.AccessoryConnectionBootstrap",
						"ioio.lib.android.bluetooth.BluetoothIOIOConnectionBootstrap" });
	}

	AndroidIOIOConnectionManager manager_ = new AndroidIOIOConnectionManager(this, this);
	TestResults results_ = new TestResults();
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        manager_.create();
    }

	@Override
	public Thread createThreadFromFactory(IOIOConnectionFactory factory) {
		return new TestThread(factory, results_);
	}

	@Override
	protected void onDestroy() {
		manager_.destroy();
		super.onDestroy();
	}

	@Override
	protected void onRestart() {
		super.onRestart();
		manager_.restart();
	}

	@Override
	protected void onStart() {
		super.onStart();
		manager_.start();
	}

	@Override
	protected void onStop() {
		manager_.stop();
		super.onStop();
	}
	
	
}