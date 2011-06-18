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

	class MainIOIOThread extends IOIOThread {
		@Override
		protected void setup() throws ConnectionLostException,
				InterruptedException {
			IcspMaster icsp = ioio_.openIcspMaster();
			icsp.enterProgramming();
			Scripts.chipErase(icsp);
			int[] buf1 = new int[64];
			int b = 0;
			for (int i = 0; i < buf1.length; ++i) {
				buf1[i] = (b++ << 16) | (b++ << 8) | b++; 
			}
			Scripts.writeBlock(icsp, 0x000000, buf1);

			int[] buf2 = new int[16];
			Scripts.readBlock(icsp, 0x000000, 16, buf2);
			StringBuffer strbuf = new StringBuffer();
			for (int i : buf2) {
				strbuf.append("0x" + Integer.toHexString(i) + "\n");
			}
			setText(strbuf.toString());
//			Scripts.entrySequence(icsp);
//			icsp.executeInstruction(0x212340); // mov #0x1234, w0
//			icsp.executeInstruction(0x883C20); // mov w0, 0x784 (VISI)
//			icsp.executeInstruction(0x000000); // nop
//			try {
//				icsp.readVisi();
//				setText("Success: got 0x"
//						+ Integer.toHexString(icsp.getResultQueue()
//								.take()));
//			} catch (InterruptedException e) {
//			}
//			int id = Scripts.getDeviceId(icsp);
//			setText("ID: 0x" + Integer.toHexString(id));
			icsp.exitProgramming();
			icsp.close();
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
}