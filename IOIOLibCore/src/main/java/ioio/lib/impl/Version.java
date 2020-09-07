package ioio.lib.impl;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

/**
 * Build version information.
 */
public class Version {
	private static String VERSION;

	static public String get() {
		if (VERSION == null) {
			VERSION = "UNDEFINED";
			try {
				InputStream stream = IOIOImpl.class.getResourceAsStream("/version.properties");
				if (stream == null) {
					throw new IOException();
				}
			    Properties properties = new Properties();
				properties.load(stream);
				VERSION = properties.getProperty("version", VERSION);
			} catch (Exception e) {
			}

		}
		return VERSION;
	}
}
