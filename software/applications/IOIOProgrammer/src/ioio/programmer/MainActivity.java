/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *  
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */
package ioio.programmer;

import ioio.lib.api.IcspMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.AbstractIOIOActivity;
import ioio.programmer.IOIOFileProgrammer.ProgressListener;
import ioio.programmer.IOIOFileReader.FormatException;

import java.io.IOException;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AbstractIOIOActivity {
	public static final String TAG = "IOIOProgrammer";
	private TextView textView_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		textView_ = (TextView) findViewById(R.id.textView);
	}

	@Override
	protected IOIOThread createIOIOThread() {
		return new MainIOIOThread();
	}

	class MainIOIOThread extends IOIOThread implements ProgressListener {
		private int nTotalBlocks_;
		private int nBlocksDone_;

		@Override
		protected void setup() throws ConnectionLostException,
				InterruptedException {
			IcspMaster icsp = ioio_.openIcspMaster();
			setText("Waiting for slave...");
			while (true) {
				icsp.enterProgramming();
				int id = Scripts.getDeviceId(icsp);
				if ((id & 0xFF00) == 0x4100) {
					setText("\nConnected: 0x" + Integer.toHexString(id));
					break;
				}
				sleep(100);
				icsp.exitProgramming();
			}
			try {
				nTotalBlocks_ = IOIOFileProgrammer
						.countBlocks(new IOIOFileReader(getAssets().open(
								"test.ioio")));
				appendText("\nErasing...");
				Scripts.chipErase(icsp);
				appendText("\nProgramming...");
				nBlocksDone_ = 0;
				IOIOFileProgrammer.programIOIOFile(icsp, new IOIOFileReader(
						getAssets().open("test.ioio")), this);
				appendText("\nVerifying...");
				nBlocksDone_ = 0;
				if (IOIOFileProgrammer.verifyIOIOFile(icsp, new IOIOFileReader(
						getAssets().open("test.ioio")), this)) {
					appendText(" PASS");
				} else {
					appendText(" FAIL");
				}
			} catch (FormatException e) {
			} catch (IOException e) {
			} finally {
				icsp.exitProgramming();
				icsp.close();
			}
		}

		@Override
		public void blockDone() {
			++nBlocksDone_;
			appendText(" "
					+ Integer.toString(100 * nBlocksDone_ / nTotalBlocks_)
					+ "%");
		}

	}

	private void setText(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				textView_.setText(text);
			}
		});
	}

	private void appendText(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				textView_.append(text);
			}
		});
	}
}