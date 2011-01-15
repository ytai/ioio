package ioio.latency_tester;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.math.BigInteger;
import java.net.ServerSocket;
import java.net.Socket;
import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.Random;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class LatencyTesterActivity extends Activity {
	private static final String LOG_TAG = "IOIO_LATENCY_TEST";
	
	private static final int UP_THROUGHPUT_START_BYTE = (int)'U';
	private static final int DOWN_THROUGHPUT_START_BYTE = (int)'D';
	private static final int BOTH_THROUGHPUT_START_BYTE = (int)'B';
	private static final int LATENCY_AVG_START_BYTE = (int)'L';
	private static final int LATENCY_ERROR_START_BYTE = (int)'L';
	private static final int ACK_BYTE = (int)'A';
	
	private static final int MAX_PACKET_SIZE = 2048;
	private static final int PACKETS_PER_TEST = 2048;
	
	private ProgressDialog progress;
	private LatencyCheckerThread thread;
	private final static Random random = new Random();
	
	private class LatencyCheckerThread extends Thread {
		private final static int PORT = 7149;
		
		private void updateProgressMessage(final String msg) {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					progress.setMessage(msg);
				}
			});
		}
		
		@Override
		public void run() {
			super.run();
			
			ServerSocket serverSocket = null;
			Socket socket = null;
			
			try {
				updateProgressMessage("Starting server");
				serverSocket = new ServerSocket(PORT);
				updateProgressMessage("Accepting connection");
				socket = serverSocket.accept();
				updateProgressMessage("Connected...");
				
				InputStream in = socket.getInputStream();
				OutputStream out = socket.getOutputStream();
				
				byte[] sizes = new byte[4];
				sizes[0] = (byte)(MAX_PACKET_SIZE >> 8);
				sizes[1] = (byte)MAX_PACKET_SIZE;
				sizes[2] = (byte)(PACKETS_PER_TEST >> 8);
				sizes[3] = (byte)PACKETS_PER_TEST;
				out.write(sizes);
				
				int read = in.read();
				if (read != ACK_BYTE) {
					Log.e(LOG_TAG, "Didn't get approval for sizes. Got: " + read);
				} else if (!testUploadThroughput(in, out) || !testDownThroughput(in, out) ||
						!testBothThroughput(in, out) || !testLatencyAverage(in, out) ||
						!testLatencyError(in, out)) {
					Log.w(LOG_TAG, "Failure in one of the tests");
				}
			} catch (IOException e) {
				Log.e(LOG_TAG, e.getMessage());
			} finally {
				try {
					socket.close();
					serverSocket.close();
				} catch (IOException e) {
					Log.e(LOG_TAG, e.getMessage());
				}
			}
			
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					progress.dismiss();
				}
			});
		}
		
		private boolean testUploadThroughput(InputStream in, OutputStream out) throws IOException {
			updateProgressMessage("Up throughput");
			if (!prepareForTransaction(UP_THROUGHPUT_START_BYTE, R.id.up_throughput_text, in, out)) {
				return false;
			}
			
			byte[] buf = new BigInteger(MAX_PACKET_SIZE * 5, random).toString(32).getBytes();
			
			long startTime = System.currentTimeMillis();
			for (int i = 0; i < PACKETS_PER_TEST; ++i) {
				out.write(buf);
			}
			
			double time = (System.currentTimeMillis() - startTime) / 1000.;
			double kb = PACKETS_PER_TEST * MAX_PACKET_SIZE / 1024.;
			double kbps = kb / time;
			
			if (in.read() != ACK_BYTE) {
				registerError(R.id.up_throughput_text);
				return false;
			}
			
			registerThroughputSuccess(R.id.up_throughput_text, kb, time, kbps);
			return true;
		}
		
		private boolean testDownThroughput(InputStream in, OutputStream out) throws IOException {
			updateProgressMessage("Down throughput");
			if (!prepareForTransaction(DOWN_THROUGHPUT_START_BYTE, R.id.down_throughput_text, in, out)) {
				return false;
			}
			
			byte[] buf = new byte[MAX_PACKET_SIZE];
			
			long startTime = System.currentTimeMillis();
			int readCount = 0;
			while (readCount < PACKETS_PER_TEST*MAX_PACKET_SIZE) {
				readCount += in.read(buf, 0, MAX_PACKET_SIZE);
			}
			if (readCount > PACKETS_PER_TEST*MAX_PACKET_SIZE) {
				registerError(R.id.down_throughput_text);
				return false;
			}
			
			double time = (System.currentTimeMillis() - startTime) / 1000.;
			double kb = PACKETS_PER_TEST * MAX_PACKET_SIZE / 1024.;
			double kbps = kb / time;
			
			if (in.read() != ACK_BYTE) {
				registerError(R.id.down_throughput_text);
				return false;
			}
			
			registerThroughputSuccess(R.id.down_throughput_text, kb, time, kbps);
			return true;
		}
		
		private boolean testBothThroughput(InputStream in, OutputStream out) throws IOException {
			updateProgressMessage("Both throughput");
			if (!prepareForTransaction(BOTH_THROUGHPUT_START_BYTE, R.id.both_throughput_text, in, out)) {
				return false;
			}
			
			byte[] buf = new byte[MAX_PACKET_SIZE];
			String randomData = new BigInteger(MAX_PACKET_SIZE * 5, random).toString(32);
			
			long startTime = System.currentTimeMillis();
			for (int i = 0; i < PACKETS_PER_TEST; ++i) {
				out.write(randomData.getBytes());
				int bytesRead = in.read(buf);
				if ((bytesRead != MAX_PACKET_SIZE) || !randomData.equals(new String(buf))) {
					Log.w(LOG_TAG, "Read buffer differs from sent buffer.");
					registerError(R.id.both_throughput_text);
					return false;
				}
			}
			
			double time = (System.currentTimeMillis() - startTime) / 1000.;
			double kb = PACKETS_PER_TEST * MAX_PACKET_SIZE * 2 / 1024.;
			double kbps = kb / time;
			
			if (in.read() != ACK_BYTE) {
				registerError(R.id.both_throughput_text);
				return false;
			}
			
			registerThroughputSuccess(R.id.both_throughput_text, kb, time, kbps);
			return true;
		}
		
		private boolean testLatencyAverage(InputStream in, OutputStream out) throws IOException {
			updateProgressMessage("Latency average");
			if (!prepareForTransaction(LATENCY_AVG_START_BYTE, R.id.latency_avg_text, in, out)) {
				return false;
			}
			
			long startTime = System.currentTimeMillis();
			for (int i = 0; i < PACKETS_PER_TEST; ++i) {
				int sendByte = i % 256;
				out.write(sendByte);
				int read = in.read();
				if (read != sendByte) {
					registerError(R.id.latency_avg_text);
					return false;
				}
			}
			long time = System.currentTimeMillis() - startTime;
			
			registerLatencySuccess(PACKETS_PER_TEST, time / 1000., time / PACKETS_PER_TEST);
			return true;
		}
		
		private boolean testLatencyError(InputStream in, OutputStream out) throws IOException {
			updateProgressMessage("Latency error");
			if (!prepareForTransaction(LATENCY_ERROR_START_BYTE, R.id.latency_error_text, in, out)) {
				return false;
			}
			
			long[] results = new long[PACKETS_PER_TEST - 1];
			for (int i = 0; i < PACKETS_PER_TEST; ++i) {
				int sendByte = i % 256;
				long startTime = System.currentTimeMillis();
				out.write(sendByte);
				if (in.read() != sendByte) {
					registerError(R.id.latency_error_text);
					return false;
				}
				// Not counting the first call.
				if (i != 0) {
					long time = System.currentTimeMillis() - startTime;
					results[i-1] = time;
				}
			}
			Arrays.sort(results);
			long minTime = results[0];
			long maxTime = results[PACKETS_PER_TEST - 2];
			int delta = (PACKETS_PER_TEST - 1) / 20;
			registerLatencyError(minTime, maxTime,
					results[19*delta], results[18*delta], results[16*delta], results[4*delta], results[2*delta], results[1*delta]);
			return true;
		}
		
		private boolean prepareForTransaction(int type, int testTextId, InputStream in, OutputStream out) throws IOException {
			out.write(type);
			int read = in.read();
			if (read != ACK_BYTE) {
				Log.e(LOG_TAG, "Read wrong byte: " + read);
				registerError(testTextId);
				return false;
			}
			return true;
		}
		
		private void registerThroughputSuccess(final int testTextId, double kb, double sec, double kbps) {
			double size = kb;
			String sizeCode = "kb";
			if (kb >= 1024) {
				size = kb / 1024.;
				sizeCode = "mb";
			}
			Double speed = kbps;
			String speedCode = "kb";
			if (kbps >= 1024) {
				speed = kbps / 1024.;
				speedCode = "mb";
			}
			DecimalFormat df= new DecimalFormat();
			df.setMaximumFractionDigits(3);
			
			final String s = String.format("%s(%s)/%s(s)  %s(%s/s)",
					df.format(size), sizeCode, df.format(sec), df.format(speed), speedCode); 
			
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					((TextView) findViewById(testTextId)).setText(s);
				}
			});
		}
		
		private void registerLatencySuccess(int packets, double sec, long msRoundTrip) {
			DecimalFormat df = new DecimalFormat();
			df.setMaximumFractionDigits(3);
			final String s = String.format(getString(R.string.default_latency_avg)
					.replace("--", "%s"), packets, df.format(sec), msRoundTrip);
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					((TextView) findViewById(R.id.latency_avg_text)).setText(s);
				}
			});
		}
		
		private void registerLatencyError(long minTime, long maxTime,
				long p5, long p10, long p20, long p80, long p90, long p95) {
			long error = (maxTime - minTime + 1) / 2;
			final String errStr = String.format(getString(R.string.default_latency_error)
					.replace("--", "%s"), error, maxTime, minTime);
			final String perStr = String.format(getString(R.string.default_latency_histogram)
					.replace("%", "%%").replace("--", "%s"), p5, p10, p20, p80, p90, p95);
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					((TextView) findViewById(R.id.latency_error_text)).setText(errStr);
					((TextView) findViewById(R.id.latency_histogram_text)).setText(perStr);
				}
			});
		}
		
		private void registerError(final int testTextId) {
			Log.w(LOG_TAG, "Error on field " + testTextId);
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					((TextView) findViewById(testTextId)).setText("Error while testing");
				}
			});
		}
	}
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Button startButton = (Button) findViewById(R.id.start_button);
        startButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				progress = ProgressDialog.show(LatencyTesterActivity.this, "Testing...", "");
				thread = new LatencyCheckerThread();
				thread.start();
			}
		});
    }
}