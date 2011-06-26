package ioio.manager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;

public class ZipExtractor {
	public static void extract(File inFile, File outDir) throws ZipException,
			IOException {
		ZipFile zip = new ZipFile(inFile);
		Enumeration<? extends ZipEntry> entries = zip.entries();
		while (entries.hasMoreElements()) {
			ZipEntry entry = entries.nextElement();
			String outFileName = outDir.getAbsolutePath() + '/'
					+ entry.getName();
			if (entry.isDirectory()) {
				if (!new File(outFileName).mkdirs()) {
					throw new IOException("Failed to create directory "
							+ outFileName);
				}
			} else {
				InputStream in = zip.getInputStream(entry);
				OutputStream out = new FileOutputStream(outFileName);
				byte[] buf = new byte[64];
				int numRead;
				while (-1 != (numRead = in.read(buf))) {
					out.write(buf, 0, numRead);
				}
				out.close();
			}
		}
	}
}
