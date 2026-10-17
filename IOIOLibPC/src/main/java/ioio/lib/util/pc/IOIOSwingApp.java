package ioio.lib.util.pc;

import ioio.lib.util.IOIOLooperProvider;

import java.awt.Window;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/**
 * A base class for creating Swing-based IOIO applications.
 * <p>
 * Use as follows:
 * <ul>
 * <li>Create your main class, extending {@link IOIOSwingApp}.</li>
 * <li>Implement {@code static void main(String[] args)} in your class by
 * creating an instance of this class, and calling its
 * {@code void go(String[] args)} method.</li>
 * <li>Implement {@code Window createMainWindow(String args[])} with code to create your main window.
 * For proper cleanup on exit, make sure to set JFrame.DISPOSE_ON_CLOSE as the default close operation of your window.</li>
 * <li>Implement
 * {@code IOIOLooper createIOIOLooper(String connectionType, Object extra)} with
 * code that should run on IOIO-dedicated threads.</li>
 * </ul>
 * <p>
 * Example:
 *
 * <pre>
 * public class MyIOIOSwingApp extends IOIOSwingApp {
 * 	// Boilerplate main().
 * 	public static void main(String[] args) throws Exception {
 * 		new MyIOIOSwingApp().go(args);
 *    }
 *
 * 	&#064;Override
 * 	protected Window createMainWindow(String args[]) {
 * 		// ... create main window ...
 *    }
 *
 * 	&#064;Override
 * 	public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
 * 		return new BaseIOIOLooper() {
 * 			&#064;Override
 * 			protected void setup() throws ConnectionLostException,
 * 					InterruptedException {
 * 				// ... code to run when IOIO connects ...
 *            }
 *
 * 			&#064;Override
 * 			public void loop() throws ConnectionLostException,
 * 					InterruptedException {
 * 				// ... code to run repeatedly as long as IOIO is connected ...
 *            }
 *
 * 			&#064;Override
 * 			public void disconnected() {
 * 				// ... code to run when IOIO is disconnected ...
 *            }
 *
 * 			&#064;Override
 * 			public void incompatible() {
 * 				// ... code to run when a IOIO with an incompatible firmware
 * 				// version is connected ...
 *            }
 *        };
 *    }
 * }
 * </pre>
 */
public abstract class IOIOSwingApp extends WindowAdapter implements IOIOLooperProvider {
    /** The helper managing the underlying IOIO connection(s). */
    public IOIOPcApplicationHelper helper_ = new IOIOPcApplicationHelper(this);

    /**
     * Boilerplate entry point. Call this from your {@code main()} method.
     *
     * @param args the command-line arguments, as passed to {@code main()}.
     */
    public void go(final String[] args) {
        javax.swing.SwingUtilities.invokeLater(() -> createMainWindow(args).addWindowListener(IOIOSwingApp.this));
    }

    /**
     * Override this method to create and return your application's main window.
     *
     * @param args the command-line arguments, as passed to {@code main()}.
     * @return the application's main window.
     */
    protected abstract Window createMainWindow(String[] args);

    /**
     * Invoked when the main window is being closed; stops the IOIO connection helper.
     *
     * @param event the window event.
     */
    @Override
    public void windowClosing(WindowEvent event) {
        helper_.stop();
    }

    @Override
    public void windowOpened(WindowEvent event) {
        helper_.start();
    }

}
