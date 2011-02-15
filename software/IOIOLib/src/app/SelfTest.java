package app;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import ioio.lib.AnalogInput;
import ioio.lib.DigitalInput;
import ioio.lib.DigitalOutput;
import ioio.lib.IOIO;
import ioio.lib.IOIOException.OperationAbortedException;
import ioio.lib.IOIOImpl;
import ioio.lib.IOIOLogger;
import ioio.lib.Uart;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * High level tests and example of usage for the IOIOLib
 *
 * @author arshan
 */
public class SelfTest extends Activity {

	// Dont stop testing if there is a failure, try them all.
	private static final boolean FORCE_ALL_TESTS = true;

	// Some setup for the tests, this pins should be shorted to each other
	public static final int OUTPUT_PIN = 26;
	public static final int INPUT_PIN = 23;
	public static final int ANALOG_INPUT_PIN = 33;
	public static final int ANALOG_OUTPUT_PIN = 14;

	// note that these overload the above digital i/o connections, but input/output reversed
	public static final int UART_RX = 26;
	public static final int UART_TX = 23;

	// for repetitive tests, do this many
	public static final int REPETITIONS = 5;

	// UI, sort of.
	private LinearLayout layout_root;
	private TextView messageText;
	private TextView statusText;
	private TextView titleText;

    IOIO ioio;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	setupViews();
    	ioio = IOIOImpl.getInstance();
    	// TODO(arshan): intercept Log.i output and put to screen
    	// TODO(arshan): buttons for restart/pause/nextTest
    }

    @Override
    public void onStart() {
    	super.onStart();

    	// Start a thread to run the tests.
    	new Thread() {
    		@Override
            public void run() {
    			try {
    				status("Connecting");
                    ioio.connect();
    				testConnection();

    				status("Testing");
    				testHardReset();
    			    testSoftReset();

    			    // testDisconnectReconnect();

    				// should test hard reset too.
    				// testDigitalOutput(); // for probing output with meter
    				testDigitalIO();
    				testAnalogInput();
    				// testPWM(); // TODO(TF)
    				// testUart(); // needs a loopback
    				msg("Tests Finished");

    				if (FORCE_ALL_TESTS) {
    					status("FORCED", Color.YELLOW);
    				}
    				else {
    					status("PASSED", Color.GREEN);
    				}

    			}
    			catch (FailException fail) {
    				IOIOLogger.log("failed"); // to get timing info in logs
    				fail.printStackTrace();
    				status("FAILED", Color.RED);
    				for (StackTraceElement line : fail.getStackTrace()) {
    					msg(line.toString());
    				}
    			}
    			catch (Exception e) {
    				exception(e);
    				e.printStackTrace();
    			}
    			}
    		}.start();

    }

    public void testConnection() throws FailException {
    	msg("Connecting to IOIO");
    	assertTrue(ioio.isConnected());
    }

    public void testHardReset() throws FailException {
    	msg("Starting Hard Reset Test");
    	for (int x = 0; x < REPETITIONS; x++) {
			ioio.hardReset();
			sleep(1100); // experimentally this is taking a little more then 1000mS
			assertTrue(ioio.isConnected());
		}
    }

    /**
     * How fast and how often can we soft reset.
     * @throws FailException
     */
    public void testSoftReset() throws FailException {
    	msg("Starting Soft Reset Test");
		for (int x = 0; x < REPETITIONS; x++) {
			sleep(100);
			ioio.softReset();
			sleep(700); // experimentally this is taking a little more then 500mS
			assertTrue(ioio.isConnected());
		}
    }

    /**
     * Example of using a digital output, slow so that can be measured with a meter
     * @throws FailException
     */
    public void testDigitalOutput() throws FailException {
    	msg("Starting Digital Output Test");
    	DigitalOutput output = ioio.openDigitalOutput(OUTPUT_PIN);
    	try {
    		for (int x = 0; x < REPETITIONS; x++) {
    			output.write(true);
    			sleep(500);
    			assertTrue(output.read());
    			output.write(false);
    			sleep(500);
    			assertFalse(output.read());
    		}
    	}
    	catch (IOException e) {
    		status("Exception", Color.BLUE);
    		msg(e.toString());
    		e.printStackTrace();
    		throw new FailException();
    	}
    	output.close();
    }

    /**
     * Example of using a Digital Input
     * @throws FailException
     */
    public void testDigitalIO() throws FailException {
    	msg("Starting Digital I/O Test");
    	ioio.softReset();
    	sleep(1000); // wait for soft reset? debugging
    	DigitalInput input = ioio.openDigitalInput(INPUT_PIN);
    	DigitalOutput output = ioio.openDigitalOutput(OUTPUT_PIN);
    	try {
			for (int x = 0; x < REPETITIONS; x++) {
				output.write(!output.read());
				sleep(100); // experimentally seems to take a bit more then 75mS
				IOIOLogger.log("doing input compare"); // to get timing info in the log
				assertEquals(output.read(), input.read());
			}
		}
		catch (IOException e) {
			exception(e);
		}
    }

    public void testAnalogInput() throws FailException {
    	msg("Starting Analog Input Test");
    	ioio.softReset();
    	sleep(800);

        AnalogInput input = ioio.openAnalogInput(ANALOG_INPUT_PIN);
        AnalogInput inputp1 = ioio.openAnalogInput(ANALOG_INPUT_PIN + 1);
        AnalogInput inputm1 = ioio.openAnalogInput(ANALOG_INPUT_PIN - 1);
    	DigitalOutput output = ioio.openDigitalOutput(ANALOG_OUTPUT_PIN);
    	try {
    	    boolean bit = false;
			for (int x = 0; x < REPETITIONS; x++) {
				sleep(100);
				bit = !output.read();
                output.write(bit);
				sleep(200);
                IOIOLogger.log("analog pins : [" + bit + "] " + input.read() + "/" + inputm1.read() + " / " + inputp1.read());
				assertTrue(bit ? input.read() > 0.9f : input.read() < 0.1f);
			}
		}
		catch (IOException e) {
			exception(e);
		}
    }

    public void testUart() throws FailException {
    	msg("Starting UART Test");
    	// TODO(TF): try this at different settings?
    	Uart uart = ioio.openUart(UART_RX, UART_TX,
    			Uart.BAUD_9600, Uart.NO_PARITY, Uart.ONE_STOP_BIT );
    	InputStream in = uart.openInputStream();
    	OutputStream out = uart.openOutputStream();
    	// TODO(TF): do the corner case chars too
    	String TEST = "IOIOabcdefghijklmnopqrstuvqzyzIOIOABCDEFGHIJKLMNOPQRSTUVWXYZIOIO";
    	for (int x = 0; x < TEST.length(); x++) {
    		try {
				out.write(TEST.charAt(x));
				assertTrue(in.read() == TEST.charAt(x));
			} catch (IOException e) {
				e.printStackTrace();
				exception(e);
			}
    	}
    }

    private void testDisconnectReconnect() throws FailException {
        msg("Starting disconnect/connect test");
        ioio.disconnect();
        assertFalse(ioio.isConnected());
        try {
            ioio.connect();
        } catch (OperationAbortedException e) {
            e.printStackTrace();
            throw new FailException();
        }
        assertTrue(ioio.isConnected());
    }

    /*
     * Utility methods below.
     */
    private void sleep(int ms) {
    	try {
    		Thread.yield();
			Thread.sleep(ms);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
    }

    private void setupViews() {
    	LinearLayout layout = new LinearLayout(this);
    	layout.setOrientation(LinearLayout.VERTICAL);
    	layout_root = layout;

    	titleText = new TextView(this);
    	titleText.setTextSize(20);
    	titleText.setText("IOIO Self Test");

    	statusText = new TextView(this);
    	statusText.setTextSize(15);

    	messageText = new TextView(this);
    	messageText.setTextSize(12);
    	ScrollView scrolling = new ScrollView(this);
    	scrolling.addView(messageText);

    	layout.addView(titleText);
    	layout.addView(statusText);
    	layout.addView(scrolling);

    	setContentView(layout);
    	layout.setVisibility(LinearLayout.VISIBLE);
    	layout.requestFocus();
    }



    private void msg(final String txt) {
    	runOnUiThread(
    			new Runnable() {
    				@Override
                    public void run() {
    					messageText.append(txt+"\n");
    				}
    			});
    }

    private void status(String txt) {
    	status(txt, Color.WHITE);
    }

    private void status(final String txt, final int color) {
    	runOnUiThread(
    			new Runnable() {
    				@Override
                    public void run() {
    					// more dramatic, maybe just for fail?
    					//if (color != Color.WHITE) {
    					//	layout_root.setBackgroundColor(color);
    					//}
    				    statusText.setTextColor(color);
    					statusText.setText(txt);
    				}
    			});
    }

    private void error(String txt) {
    	status(txt, Color.RED);
    }

    private void exception(Exception e) {
    	status("Exception", Color.BLUE);
    	msg(e.toString());
    }

    private void assertTrue(boolean val) throws FailException{
    	if (!val) {
    		if (FORCE_ALL_TESTS) {
    			status("FAILED", Color.RED);
    			msg("FAILED");
    		}
    		else {
    			throw new FailException();
    		}
    	}
//    	msg(val?"pass":"fail");
    }

    private void assertFalse(boolean val) throws FailException {
        assertTrue(!val);
    }

    private void assertEquals(boolean x, boolean y) throws FailException {
    	if (x!=y) {
    		msg("not equal : " + x + " vs. " + y);
    		throw new FailException();
    	}
    }

    private class FailException extends Exception {}
}