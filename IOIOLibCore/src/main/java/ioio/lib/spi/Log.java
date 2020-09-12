/*
 * Copyright 2015 Ytai Ben-Tsvi. All rights reserved.
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
package ioio.lib.spi;

import java.io.PrintWriter;
import java.io.StringWriter;

public class Log {
    public static final int ASSERT = 7;
    public static final int ERROR = 6;
    public static final int WARN = 5;
    public static final int INFO = 4;
    public static final int DEBUG = 3;
    public static final int VERBOSE = 2;
    private static ILogger log_;

    static {
        try {
            Log.log_ = (ILogger) Class.forName("ioio.lib.spi.LogImpl").newInstance();
        } catch (Exception e) {
            throw new RuntimeException(
                    "Cannot instantiate the LogImpl class. This is likely a result of failing to " +
                            "include a proper platform-specific IOIOLib* library.", e);
        }
    }

    public static void e(String tag, String message) {
        write(ERROR, tag, message);
    }

    public static void e(String tag, String message, Throwable exception) {
        write(ERROR, tag, message, exception);
    }

    public static void w(String tag, String message) {
        write(WARN, tag, message);
    }

    public static void w(String tag, String message, Throwable exception) {
        write(WARN, tag, message, exception);
    }

    public static void i(String tag, String message) {
        write(INFO, tag, message);
    }

    public static void i(String tag, String message, Throwable exception) {
        write(INFO, tag, message, exception);
    }

    public static void d(String tag, String message) {
        write(DEBUG, tag, message);
    }

    public static void d(String tag, String message, Throwable exception) {
        write(DEBUG, tag, message, exception);
    }

    public static void v(String tag, String message) {
        write(VERBOSE, tag, message);
    }

    public static void v(String tag, String message, Throwable exception) {
        write(VERBOSE, tag, message, exception);
    }

    private static void write(int level, String tag, String message) {
        log_.write(level, tag, message);
    }

    private static void write(int level, String tag, String message, Throwable exception) {
        StringWriter writer = new StringWriter();
        PrintWriter printWriter = new PrintWriter(writer);
        printWriter.println(message);
        exception.printStackTrace(new PrintWriter(writer));
        writer.flush();
        write(level, tag, writer.toString());
    }

    public interface ILogger {
        void write(int level, String tag, String message);
    }

}
