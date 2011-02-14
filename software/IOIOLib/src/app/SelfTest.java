package app;

import ioio.lib.AnalogInput;
import ioio.lib.DigitalInput;
import ioio.lib.DigitalOutput;
import ioio.lib.IOIO;

import java.io.IOException;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

/**
 * A simple self test for the IOIOLib
 * @author arshan
 */
public class SelfTest extends Activity {
	
	// Dont stop testing if there is a failure, try them all.
	private static final boolean FORCE_ALL_TESTS = false;
	
	// Some setup for the tests, this pins should be shorted to each other
	public static final int OUTPUT_PIN = 26;
	public static final int INPUT_PIN = 23;
	public static final int ANALOG_INPUT_PIN = 33;
	public static final int ANALOG_OUTPUT_PIN = 14;
	
	// for repetitive tests, do this many
	public static final int REPETITIONS = 5;

	// UI, sort of.
	private LinearLayout layout_root;
	private TextView messageText;
	private TextView statusText;
	private TextView titleText;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {        
    	super.onCreate(savedInstanceState);
    	setupViews();
    	// TODO(arshan): intercept Log.i output and put to screen
    	// TODO(arshan): buttons for restart/pause/nextTest
    }

    @Override  
    public void onStart() {
    	super.onStart();
    	
    	// Start a thread to run the tests.
    	new Thread() { 		
    		public void run() {
    			try {
    				
    				status("Connecting");    			
    				testConnection();
    				
    				status("Testing");
    				testHardReset();
    			    testSoftReset();
    				// should test hard reset too.
    				// testDigitalOutput(); // for probing output with meter
    				testDigitalInput();
    				// testAnalogInput();
    				msg("Tests Finished");

    				if (FORCE_ALL_TESTS) {
    					status("FORCED", Color.YELLOW);
    				}
    				else {
    					status("PASSED", Color.GREEN);
    				}
    				
    			}
    			catch (FailException fail) {
    				IOIO.getInstance().log("failed"); // to get timing info in logs
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
    	assertTrue(IOIO.getInstance().isConnected());    	
    }
    
    public void testHardReset() throws FailException {
    	msg("Starting Hard Reset Test");
    	IOIO ioio = IOIO.getInstance();
    	for (int x = 0; x < REPETITIONS; x++) {
			ioio.hardReset();
			sleep(1100); // experimentally this is taking a little more then 1000mS
			assertTrue(ioio.isConnected());
		}
    }
    
    /**
     * how fast and how often can we soft reset.
     * @throws FailException
     */
    public void testSoftReset() throws FailException {
    	msg("Starting Soft Reset Test");
    	IOIO ioio = IOIO.getInstance();
		for (int x = 0; x < REPETITIONS; x++) {
			sleep(100);
			ioio.softReset();
			sleep(700); // experimentally this is taking a little more then 500mS
			assertTrue(ioio.isConnected());
		}		
    }

    /**
     * Example of using a digital output.
     * @throws FailException
     */
    public void testDigitalOutput() throws FailException {
    	msg("Starting Digital Output Test");
    	DigitalOutput output = IOIO.getInstance().openDigitalOutput(OUTPUT_PIN);    
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
    public void testDigitalInput() throws FailException {
    	msg("Starting Digital Input Test");
    	IOIO ioio = IOIO.getInstance();
    	ioio.softReset();
    	sleep(1000); // wait for soft reset? debugging
    	DigitalInput input = IOIO.getInstance().openDigitalInput(INPUT_PIN);
    	DigitalOutput output = IOIO.getInstance().openDigitalOutput(OUTPUT_PIN);
    	try {
			for (int x = 0; x < REPETITIONS; x++) {
				output.write(!output.read());
				sleep(100); // experimentally seems to take a bit more then 80mS
				//sleep(1000); // shouldnt be necessary, but it is. hmmm
				Log.i("IOIO SelfTest", "doing input compare @" + System.currentTimeMillis());
				assertEquals(output.read(), input.read());
			}			
		} 
		catch (IOException e) {
			exception(e);
		}
    }
    
    public void testAnalogInput() throws FailException {
    	msg("Starting Analog Input Test");
    	IOIO ioio = IOIO.getInstance();
    	ioio.softReset();
    	AnalogInput input = IOIO.getInstance().openAnalogInput(ANALOG_INPUT_PIN);
    	DigitalOutput output = IOIO.getInstance().openDigitalOutput(ANALOG_OUTPUT_PIN);
    	try {
			for (int x = 0; x < REPETITIONS; x++) {
				sleep(100);
				output.write(!output.read());
				sleep(100); // shouldnt be necessary
				assertTrue(3.3f == input.read());
			}			
		} 
		catch (IOException e) {
			exception(e);
		}
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
    	if (val) {
    		if (FORCE_ALL_TESTS) {
    			status("FAILED", Color.RED);
    			msg("FAILED");
    		}
    		else {
    			throw new FailException();
    		}
    	}
//    	msg(val?"fail":"pass");
    }
    
    private void assertEquals(boolean x, boolean y) throws FailException {
    	if (x!=y) {
    		msg("not equal : " + x + " vs. " + y);
    		throw new FailException();
    	}
    }
    
    private class FailException extends Exception {}
}