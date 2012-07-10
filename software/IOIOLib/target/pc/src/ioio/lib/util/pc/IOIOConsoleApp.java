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

package ioio.lib.util.pc;

import ioio.lib.util.IOIOLooperProvider;

/**
 * A base class for creating console-based IOIO applications.
 * <p>
 * Use as follows:
 * <ul>
 * <li>Create your main class, extending {@link IOIOConsoleApp}.</li>
 * <li>Implement {@code static void main(String[] args)} in your class by
 * creating an instance of this class, and calling its
 * {@code void go(String[] args)} method.</li>
 * <li>Implement {@code void run(String[] args)} with your main thread logic.</li>
 * <li>Implement
 * {@code IOIOLooper createIOIOLooper(String connectionType, Object extra)} with
 * code that should run on IOIO-dedicated threads.</li>
 * </ul>
 * <p>
 * Example:
 * 
 * <pre>
 * public class MyIOIOConsoleApp extends IOIOConsoleApp {
 * 	// Boilerplate main().
 * 	public static void main(String[] args) throws Exception {
 * 		new MyIOIOConsoleApp().go(args);
 * 	}
 * 
 * 	&#064;Override
 * 	protected void run(String[] args) throws IOException {
 * 		// ... main thread logic here ...
 * 	}
 * 
 * 	&#064;Override
 * 	public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
 * 		return new BaseIOIOLooper() {
 * 			&#064;Override
 * 			protected void setup() throws ConnectionLostException,
 * 					InterruptedException {
 * 				// ... code to run when IOIO connects ...
 * 			}
 * 
 * 			&#064;Override
 * 			public void loop() throws ConnectionLostException,
 * 					InterruptedException {
 * 				// ... code to run repeatedly as long as IOIO is connected ...
 * 			}
 * 
 * 			&#064;Override
 * 			public void disconnected() {
 * 				// ... code to run when IOIO is disconnected ...
 * 			}
 * 
 * 			&#064;Override
 * 			public void incompatible() {
 * 				// ... code to run when a IOIO with an incompatible firmware
 * 				// version is connected ...
 * 			}
 * 		};
 * 	}
 * }
 * </pre>
 */
public abstract class IOIOConsoleApp implements IOIOLooperProvider {
	protected final void go(String[] args) throws Exception {
		IOIOPcApplicationHelper helper = new IOIOPcApplicationHelper(this);
		helper.start();
		try {
			run(args);
		} catch (Exception e) {
			throw e;
		} finally {
			helper.stop();
		}
	}

	protected abstract void run(String[] args) throws Exception;
}
