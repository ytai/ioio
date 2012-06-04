package ioio.lib.util.pc;

import ioio.lib.util.IOIOApplicationHelper;
import ioio.lib.util.IOIOLooperProvider;

import java.awt.Window;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public abstract class IOIOSwingApp extends WindowAdapter implements IOIOLooperProvider {
	public IOIOApplicationHelper helper_ = new IOIOPcApplicationHelper(this);

	public void go(final String args[]) {
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				createMainWindow(args).addWindowListener(IOIOSwingApp.this);
			}
		});
	}

	protected abstract Window createMainWindow(String args[]);

	@Override
	public void windowClosing(WindowEvent event) {
		helper_.stop();
	}

	@Override
	public void windowOpened(WindowEvent event) {
		helper_.start();
	}

}
