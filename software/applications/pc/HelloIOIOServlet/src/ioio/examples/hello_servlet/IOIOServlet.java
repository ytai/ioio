package ioio.examples.hello_servlet;

import javax.servlet.http.HttpServlet;

import ioio.lib.util.IOIOLooperProvider;
import ioio.lib.util.pc.IOIOPcApplicationHelper;

/**
 * A base class for creating Servlet-based IOIO applications.
 * <p>
 * Use as follows:
 * <ul>
 * <li>Create your main class, extending {@link IOIOServlet}.</li>
 * <li>Implement {@code static void main(String[] args)} in your class by
 * creating an instance of Tomcat and adding your servlet to the Tomcat.</li>
 * <li>Implement the service(HttpServletRequest req, HttpServletResponse resp) to handle the http requests.
 * The IOIOServlet base class should clean up the IOIO thread when the Tomcat and the servlet shuts down.</li>
 * <li>Implement
 * {@code IOIOLooper createIOIOLooper(String connectionType, Object extra)} with
 * code that should run on IOIO-dedicated threads.</li>
 * </ul>
 * <p>
 * Example:
 * 
 * <pre>
 * public class MyIOIOServlet extends IOIOServlet {
 * 	public static void main(String[] args) throws Exception {
 * 		Tomcat tomcat = new Tomcat();
 *		final int port = 8080;		//Pick any free port
 *		tomcat.setPort(port);
 *		 
 *		Context ctx = tomcat.addContext("/", new File(".").getAbsolutePath());
 *		
 *		Tomcat.addServlet(ctx, "IOIOSample", new HelloIOIOServlet());
 *		ctx.addServletMapping("/*", "IOIOSample");		//This servlet will handle all requsts coming to http://ip:8080/*
 *		 
 *		tomcat.start();
 *		tomcat.getServer().await();						//This blocks this thread until the tomcat service is stopped.
 * 	}
 * 
 * 	&#064;Override
 * 	protected void service(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
 * 		// ... Handle HttpServletRequests  ...
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

public abstract class IOIOServlet extends HttpServlet implements IOIOLooperProvider {
	
	private static final long serialVersionUID = 1L;
	private boolean servletRunning = true;
	
	public IOIOServlet()
	{
		Thread th = new Thread() {
            public synchronized void run() {
            	runIOIOHelper();
            }
        };
        th.start();
	}
	
	private final void runIOIOHelper() {
		IOIOPcApplicationHelper helper = new IOIOPcApplicationHelper(this);
		helper.start();
		try {
			while (servletRunning) {
				Thread.sleep(10);
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			helper.stop();
		}
	}
	
	/**destroy is called when the servlet is stopped. We should flag the IOIOPcApplicationHelper to stop as well */
	@Override
	public void destroy()
	{
		servletRunning = false;
	}
}
