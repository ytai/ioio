package ioio.lib.test;

import ioio.api.AnalogInput;
import ioio.api.DigitalInput;
import ioio.api.DigitalOutput;
import ioio.api.IOIOLib;
import ioio.api.IOIOSingleton;
import ioio.api.PwmOutput;
import ioio.api.Uart;
import ioio.api.exception.ConnectionLostException;
import ioio.api.exception.OutOfResourceException;
import ioio.lib.IOIOLogger;
import ioio.lib.IOIOUart;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

/**
 * High level tests and example of usage for the IOIOLib
 * 
 * TODO(arshan): intercept Log.i output and put to screen
 * TODO(arshan): buttons for restart/pause/nextTest
 * 
 * @author arshan
 */
public class IOIOLibTest extends Activity {

	// Dont stop testing if there is a failure, try them all.
	private static final boolean FORCE_ALL_TESTS = false;

	// Some setup for the tests, this pins should be shorted to each other
	public static final int OUTPUT_PIN = 10;
	public static final int INPUT_PIN = 11;
	public static final int ANALOG_INPUT_PIN = 33;
	public static final int ANALOG_OUTPUT_PIN = 14;

	// note that these overload the above digital i/o connections, but input/output reversed
	public static final int UART_RX = 7; 
	public static final int UART_TX = 6;

	// for repetitive tests, do this many
	public static final int REPETITIONS = 5;

    private static final int PWM_OUT_PIN = 7;
	
    IOIOLib ioio;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	setupViews();
    	try {
    	    ioio = IOIOSingleton.waitForController();
    	}
    	catch (Exception e) {
    	    exception(e);
    	    e.printStackTrace();
    	}
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
                    ioio.waitForConnect();
                    status("Testing");
                    
                    testConnection();

                   

                    /*
    				testHardReset();
     			    testSoftReset();

    			    testDisconnectReconnect();

    				// should test hard reset too.
    			    //testDigitalOutput(); // for probing output with meter
     				testDigitalIO();
                    testAnalogInput();
                     testPWM();
                    // testServo();
                      
                     
                     */
     				testUart(); 
     				
    				msg("Tests Finished");

    				if (FORCE_ALL_TESTS) {
    					status("FORCED", Color.YELLOW);
    				}
    				else {
    					status("PASSED", Color.GREEN);
    				}

    			} catch (FailException fail) {
    				IOIOLogger.log("failed"); // to get timing info in logs
    				fail.printStackTrace();
    				status("FAILED", Color.RED);
    				for (StackTraceElement line : fail.getStackTrace()) {
    					msg(line.toString());
    				}
    			} catch (Exception e) {
    				exception(e);
    				e.printStackTrace();
    			} finally {
//    			    ioio.disconnect();
    			}
			}
		}.start();

    }

    public void testConnection() throws FailException {
    	msg("Testing connection to Ioio");
    	assertTrue(ioio.isConnected());
    }

    public void testHardReset() throws FailException {
    	msg("Starting Hard Reset Test");
    	for (int x = 0; x < REPETITIONS; x++) {
			try {
                ioio.hardReset();
            } catch (ConnectionLostException e) {
                exception(e);
            }
			IOIOLogger.log("hard reset complete");
			sleep(500);
			try {
                ioio.waitForConnect();
            } catch (Exception e) {
                e.printStackTrace();
                IOIOLogger.log("exception in hard reset");
                exception(e);
            }
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
			try {
                ioio.softReset();
            } catch (ConnectionLostException e) {
                e.printStackTrace();
                exception(e);
            }
			sleep(500);
			try {
                ioio.waitForConnect();
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                IOIOLogger.log("soft reset failed");
                assertFalse(true);
            }
			assertTrue(ioio.isConnected());
		}
    }

    /**
     * Example of using a digital output, slow so that can be measured with a meter
     * @throws FailException
     */
    public void testDigitalOutput() throws FailException {
    	msg("Starting Digital Output Test");
    	DigitalOutput output = null;
    	try {
            output = ioio.openDigitalOutput(OUTPUT_PIN, false);
            for (int x = 0; x < REPETITIONS; x++) {
    			output.write(true);
    			sleep(500);
    			
    			output.write(false);
    			sleep(500);
    		}
    		output.close();
    	} catch (Exception e) {
    		exception(e);
    	}
    }

    /**
     * Example of using a Digital Input
     * @throws FailException
     */
    public void testDigitalIO() throws FailException {
    	msg("Starting Digital I/O Test");
    	try {
            ioio.softReset();
        } catch (ConnectionLostException e) {
            e.printStackTrace();
            exception(e);
        }
    	sleep(100); // wait for soft reset? 
        try {
        	ioio.waitForConnect();
        	DigitalInput input = ioio.openDigitalInput(INPUT_PIN);
        	DigitalOutput output = ioio.openDigitalOutput(OUTPUT_PIN, false);
        	boolean value = true;
			for (int x = 0; x < REPETITIONS; x++) {
				output.write(value);
				sleep(100);
				assertEquals(value, input.read());
				value = !value;
			}
			input.close();
			output.close();
		} catch (Exception e) {
            exception(e);
        }
    }

    public void testAnalogInput() throws FailException {
    	msg("Starting Analog Input Test");
        try {
            ioio.softReset();
        } catch (ConnectionLostException e) {
            e.printStackTrace();
            exception(e);
        }
        sleep(100); // wait for soft reset? debugging
        try {
            ioio.waitForConnect();
            boolean bit = false;
            DigitalOutput output = ioio.openDigitalOutput(ANALOG_OUTPUT_PIN, bit);         
            AnalogInput input = ioio.openAnalogInput(ANALOG_INPUT_PIN);
            sleep(100);
           
			for (int x = 0; x < REPETITIONS; x++) {
				sleep(100);
                output.write(bit);
				sleep(200);
                IOIOLogger.log("analog pins : [" + bit + "] " + input.read());
				assertTrue(bit ? input.read() > 0.9f : input.read() < 0.1f);
                bit = !bit;
			}
			output.close();
			input.close();
        } catch (Exception e) {
            exception(e);
        }
    }

 
    public void testUart() throws FailException {
    	msg("Starting UART Test");
    	testUartAtBaud(IOIOUart.BAUD_9600);
    	softReset(); // BUG? is this cause SW doesnt tear down or FW? 
    	testUartAtBaud(IOIOUart.BAUD_19200);
    	softReset();
        testUartAtBaud(IOIOUart.BAUD_38400);          
        softReset();
        testUartAtBaud(IOIOUart.BAUD_115200);          
    }

    public void softReset() {
        try {
            ioio.softReset();
        } catch (ConnectionLostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } 
        sleep(100);
    }
    protected void testUartAtBaud(int baud) {
    
        boolean cache_test = true;
        msg("Testing UART at " + baud);
        try {
        	// TODO(arshan): try this at different settings?
            Uart uart = ioio.openUart(UART_RX, UART_TX,
        			baud, IOIOUart.NO_PARITY, IOIOUart.ONE_STOP_BIT );
        	InputStream in = uart.openInputStream();
        	OutputStream out = uart.openOutputStream();
        	
        	int c;
        	
        	String TEST = "The quick red fox jumped over the lazy grey dog; Also !@#$%^*()_+=-`~\\|][{}" ;
        	
        	for (int x = 0; x < TEST.length(); x++) {
    			out.write(TEST.charAt(x));
    			c = in.read();
    			assertTrue(c == TEST.charAt(x));
        	}
        	msg("passed inline test");
        	
        	// TEST = "Short";
        	TEST = 
        	    "The quick red fox jumped over the lazy grey dog; Also !@#$%^*()_+=-`~\\|][{}" +
        	    "The quick red fox jumped over the lazy grey dog; Also !@#$%^*()_+=-`~\\|][{}" +
        	    "The quick red fox jumped over the lazy grey dog; Also !@#$%^*()_+=-`~\\|][{}" +
        	    "The quick red fox jumped over the lazy grey dog; Also !@#$%^*()_+=-`~\\|][{}" +
        	    "The quick red fox jumped over the lazy grey dog; Also !@#$%^*()_+=-`~\\|][{}" ;
        	if (cache_test) {
        	// now without blocking ... tests the caching
        	for (int x = 0; x < TEST.length(); x++) {
                out.write(TEST.charAt(x));
            }
        	
       
        	msg("all bytes in cache");
        	c = 'a';
        	for (int x = 0; x < TEST.length(); x++) {
//                out.write(c);
        	    c = in.read();
        	    sleep(50);
               //  msg("got back : " + (char)c);              
                assertTrue(c == TEST.charAt(x));
            }
        	msg("passed cached test");
        	}
        	
        	uart.close();
    	} catch (Exception e) {
    	    e.printStackTrace();
    	    exception(e);
    	}
    }

    private void testDisconnectReconnect() throws FailException {
        msg("Starting disconnect/connect test");
        ioio.disconnect();
        IOIOLogger.log("disconnected");
        assertFalse(ioio.isConnected());
        sleep(1000);
        try {
            ioio.waitForConnect();
            IOIOLogger.log("connected");
        } catch (Exception e) {
            e.printStackTrace();
            IOIOLogger.log("operation aborted");
        }
        assertTrue(ioio.isConnected());
    }

    private void testPWM() throws FailException {
        msg("Starting PWM tests");
        PwmOutput pwmOutput = null;
        final int SLEEP_TIME = 500;
        try {
            ioio.waitForConnect();
            // 10ms / 100Hz for the servo.
            pwmOutput = ioio.openPwmOutput(PWM_OUT_PIN, 100);
            msg("Moving right");
            for (int i = 0; i <= 5; i++) {
                pwmOutput.setDutyCycle((15 + i) / 100.f);
                msg("Increasing speed");
                sleep(SLEEP_TIME);
            }
            for (int i = 5; i > 0; i--) {
                pwmOutput.setDutyCycle((15 + i) / 100.f);
                msg("Decreasing speed");
                sleep(SLEEP_TIME);
            }
            msg("Moving left");
            for (int i = 0; i >= -5; i--) {
                pwmOutput.setDutyCycle((15 + i) / 100.f);
                sleep(SLEEP_TIME);
                msg("increasing speed");
            }
            for (int i = -4; i <= 0; i++) {
                pwmOutput.setDutyCycle((15 + i) / 100.f);
                sleep(SLEEP_TIME);
                msg("decreasing speed");
            }
            status("stopped");
        } catch (OutOfResourceException e) {
            exception(e);
        } catch (Exception e) {
            exception(e);
        } finally {
            if (pwmOutput != null) {
                try {
                    pwmOutput.close();
                } catch (IOException e) {
                   exception(e);
                }
            }
        }
    }

    public void testSpi() {
        
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
    
    
 // UI, sort of.
    private LinearLayout layout_root;
    private TextView messageText;
    private TextView statusText;
    private TextView titleText;


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
        IOIOLogger.log(txt);
    	runOnUiThread(
    			new Runnable() {
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