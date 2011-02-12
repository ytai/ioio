package app;

import ioio.lib.DigitalInput;
import ioio.lib.DigitalOutput;
import ioio.lib.IOIO;
import ioio.lib.IOIOException;

import java.io.IOException;
import java.net.ServerSocket;


import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * A simple self test for the IOIOLib
 * @author arshan
 */
public class SelfTest extends Activity {
	
	// Dont stop testing if there is a failure, try them all.
	private static final boolean FORCE_ALL_TESTS = true;
	
	public static final int OUTPUT_PIN = 2;
	public static final int INPUT_PIN = 3;
	
	// for repetitive tests, do this many
	public static final int REPETITIONS = 20;
	
	private LinearLayout layout_root;
	private TextView messageText;
	private TextView statusText;
	private TextView titleText;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {        
    	super.onCreate(savedInstanceState);
    	setupViews();
    	
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
    				testDigitalOutput();
    				testDigitalInput();
    				msg("Tests Finished");

    				if (FORCE_ALL_TESTS) {
    					status("FORCED", Color.YELLOW);
    				}
    				else {
    					status("PASSED", Color.GREEN);
    				}
    				
    			}
    			catch (FailException fail) {
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
    	messageText.setScrollContainer(true);
    	messageText.setHorizontallyScrolling(true);
    	messageText.setVerticalScrollBarEnabled(true);
    	
    	layout.addView(titleText);    	
    	layout.addView(statusText);
    	layout.addView(messageText);
    	
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
    
    public void testConnection() throws FailException {
    	msg("Connecting to IOIO");
    	assertTrue(IOIO.getInstance().isConnected());    	
    }
    
    public void testDigitalOutput() throws FailException {

    	msg("Starting Digital Output Test");
    	DigitalOutput output = IOIO.getInstance().openDigitalOutput(OUTPUT_PIN);    
    	try {
    		for (int x = 0; x < REPETITIONS; x++) {
    			sleep(100);
    			output.write(true);
    			assertTrue(output.read());
    			sleep(300);
    			output.write(false);
    			assertFalse(output.read());    			
    		}
    	}
    	catch (IOIOException e) {
    		status("Exception", Color.BLUE);
    		msg(e.toString());
    	}
    	
    	output.close();
    }

    public void testDigitalInput() throws FailException {
    	msg("Starting Digital Input Test");
    	DigitalInput input = IOIO.getInstance().openDigitalInput(INPUT_PIN);
    	DigitalOutput output = IOIO.getInstance().openDigitalOutput(OUTPUT_PIN);
    	try {
			for (int x = 0; x < REPETITIONS; x++) {
				output.write(!output.read());
				assertTrue(output.read() == input.read());
				sleep(100);
			}			
		} catch (IOIOException e) {
			exception(e);
		}
    }
    
    private void sleep(int ms) {
    	try {
    		Thread.yield();
			Thread.sleep(ms);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
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
    
    private class FailException extends Exception {}
}