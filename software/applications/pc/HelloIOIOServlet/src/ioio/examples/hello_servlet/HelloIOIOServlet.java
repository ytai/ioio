package ioio.examples.hello_servlet;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.IOIOConnectionManager.Thread;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.catalina.Context;
import org.apache.catalina.startup.Tomcat;

public class HelloIOIOServlet extends IOIOServlet {

	boolean ledOn = true;
	private static final long serialVersionUID = 1L;

	
	/**
	 * Run this main method just like any other java main method. Then, use your web-browser
	 * to navigate to the following URLs:
	 * http://localhost:8181/off
	 * http://localhost:8181/on
	 * 
	 * This will turn the LED on or off. At startup, the IOIO's LED will turn on, which indicates
	 * that the servlet is running and ready.
	 */
	public static void main(String[] args) throws Exception {
		Tomcat tomcat = new Tomcat();
		final int port = 8181;
		tomcat.setPort(port);

		Context ctx = tomcat.addContext("/", new File(".").getAbsolutePath());

		Tomcat.addServlet(ctx, "IOIOSample", new HelloIOIOServlet());
		ctx.addServletMapping("/*", "IOIOSample");

		tomcat.start();
		tomcat.getServer().await();
	}

	@Override
	public void doGet(HttpServletRequest req, HttpServletResponse response) throws IOException, ServletException {
		// Set the response message's MIME type.
		response.setContentType("text/html;charset=UTF-8");
		// Allocate a output writer to write the response message into the
		// network socket.
		PrintWriter out = response.getWriter();

		// Write the response message, in an HTML document.
		try {
			out.println("<!DOCTYPE html>");
			out.println("<html><head>");
			out.println("<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>");
			out.println("<title>IOIO</title></head>");
			out.println("<body>");
			out.println("<big><big>");
			out.println("<b><a href=\"/on\">on</a></b>");
			out.println("<br><br><br><br>");					//I put many breaks to make it easier on android phone.
			out.println("<b><a href=\"/off\">off</a></b>");
			out.println("</big></big>");
			out.println("</body></html>");
		} finally {
			out.close(); // Always close the output writer
		}

		//Update internal state as needed
		if (req.getRequestURL().toString().endsWith("on"))
			ledOn = true;
		else
			ledOn = false;
	}

	@Override
	public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
		return new BaseIOIOLooper() {
			private DigitalOutput led_;

			@Override
			protected void setup() throws ConnectionLostException,
					InterruptedException {
				led_ = ioio_.openDigitalOutput(IOIO.LED_PIN, true);
			}

			@Override
			public void loop() throws ConnectionLostException,
					InterruptedException {
				led_.write(!ledOn);
				Thread.sleep(10);
			}
		};
	}
}
