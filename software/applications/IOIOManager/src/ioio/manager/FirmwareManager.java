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
package ioio.manager;

import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class FirmwareManager {
	private static FirmwareManager instance_;
	private File appLayerDir_;
	private File imageDir_;
	private File activeImagesDir_;
	private String activeBundleName_;
	private Context context_;
	private SharedPreferences preferences_;

	public class ImageFile {
		private File file_;

		public ImageFile(File file) {
			file_ = file;
		}

		public String getName() {
			String name = file_.getName();
			return name.substring(0, name.lastIndexOf('.'));
		}

		public File getFile() {
			return file_;
		}
	}

	public class ImageBundle {
		private final File dir_;
		private final String name_;

		public ImageBundle(File dir, String name) {
			dir_ = dir;
			name_ = name;
		}

		public ImageBundle(File dir) {
			this(dir, dir.getName());
		}

		public String getName() {
			return name_;
		}

		public boolean isActive() {
			if (name_ != null) {
				return name_.equals(activeBundleName_);
			}
			return activeBundleName_ == null;
		}

		public ImageFile[] getImages() {
			File[] files = getImageFiles(dir_);
			ImageFile[] result = new ImageFile[files.length];
			for (int i = 0; i < result.length; ++i) {
				result[i] = new ImageFile(files[i]);
			}
			return result;
		}
	}

	public static FirmwareManager getInstance(Context context)
			throws IOException {
		if (instance_ == null) {
			instance_ = new FirmwareManager(context.getApplicationContext());
		}
		return instance_;
	}

	private FirmwareManager(Context context) throws IOException {
		context_ = context;
		activeImagesDir_ = context.getFilesDir();
		appLayerDir_ = new File(context.getFilesDir().getAbsolutePath()
				+ "/app_layer");
		imageDir_ = new File(context.getFilesDir().getAbsolutePath() + "/image");
		preferences_ = context_.getSharedPreferences("FirmwareManager", 0);
		activeBundleName_ = preferences_.getString("activeBundleName", null);
		if (!appLayerDir_.exists()) {
			if (!appLayerDir_.mkdir()) {
				throw new IOException(String.format(
						context_.getString(R.string.failed_create_dir),
						appLayerDir_.getAbsolutePath()));
			}
		} else if (!appLayerDir_.isDirectory()) {
			throw new IllegalStateException(appLayerDir_.getAbsolutePath()
					+ " is not a directory");
		}
		if (!imageDir_.exists()) {
			if (!imageDir_.mkdir()) {
				throw new IOException(String.format(
						context_.getString(R.string.failed_create_dir),
						imageDir_.getAbsolutePath()));
			}
		} else if (!imageDir_.isDirectory()) {
			throw new IllegalStateException(imageDir_.getAbsolutePath()
					+ " is not a directory");
		}
	}

	public ImageBundle addAppBundle(String path) throws IOException {
		File inFile = new File(path);
		String name = inFile.getName();
		name = name.substring(0, name.lastIndexOf('.'));
		File outDir = new File(appLayerDir_.getAbsolutePath() + '/' + name);
		if (outDir.exists()) {
			throw new IOException(String.format(
					context_.getString(R.string.bundle_already_exists), name));
		}
		outDir.mkdirs();
		ZipExtractor.extract(inFile, outDir);
		return new ImageBundle(outDir);
	}

	public ImageBundle addImageBundle(String path) throws IOException {
		File inFile = new File(path);
		String name = inFile.getName();
		name = name.substring(0, name.lastIndexOf('.'));
		File outDir = new File(imageDir_.getAbsolutePath() + '/' + name);
		if (outDir.exists()) {
			throw new IOException(String.format(
					context_.getString(R.string.bundle_already_exists), name));
		}
		outDir.mkdirs();
		ZipExtractor.extract(inFile, outDir);
		return new ImageBundle(outDir);
	}

	public ImageBundle[] getAppBundles() {
		ImageBundle unnamed = null;
		if (activeBundleName_ == null) {
			unnamed = new ImageBundle(context_.getFilesDir(), null);
			if (unnamed.getImages().length == 0) {
				unnamed = null;
			}
		}
		File[] files = appLayerDir_.listFiles();
		ImageBundle[] result = new ImageBundle[files.length
				+ (unnamed != null ? 1 : 0)];
		for (int i = 0; i < files.length; ++i) {
			result[i] = new ImageBundle(files[i]);
		}
		if (unnamed != null) {
			result[result.length - 1] = unnamed;
		}
		return result;
	}

	public ImageBundle[] getImageBundles() {
		File[] files = imageDir_.listFiles();
		ImageBundle[] result = new ImageBundle[files.length];
		for (int i = 0; i < files.length; ++i) {
			result[i] = new ImageBundle(files[i]);
		}
		return result;
	}

	public void setActiveAppBundle(String name) throws IOException {
		File bundleDir = new File(appLayerDir_.getAbsolutePath() + '/' + name);
		if (!bundleDir.exists() || !bundleDir.isDirectory()) {
			throw new IllegalArgumentException(
					"Bundle does not exist or is not a directory: " + name);
		}
		File[] imageFiles = getImageFiles(bundleDir);
		for (File f : imageFiles) {
			copyFileToFilesDir(f, Context.MODE_WORLD_READABLE);
			try {
				createFingerprint(f.getName());
			} catch (NoSuchAlgorithmException e) {
				throw new RuntimeException(e);
			}
		}
		activeBundleName_ = name;
		Editor editor = preferences_.edit();
		editor.putString("activeBundleName", name);
		editor.commit();
	}

	public void removeAppBundle(String name) throws IOException {
		File bundleDir = new File(appLayerDir_.getAbsolutePath() + '/' + name);
		if (!bundleDir.exists() || !bundleDir.isDirectory()) {
			throw new IllegalArgumentException(
					"Bundle does not exist or is not a directory: " + name);
		}
		if (name.equals(activeBundleName_)) {
			clearActiveBundle();
		}
		if (!recursiveDelete(bundleDir)) {
			throw new IOException(
					context_.getString(R.string.recursive_delete_failed));
		}
	}

	public void removeImageBundle(String name) throws IOException {
		File bundleDir = new File(imageDir_.getAbsolutePath() + '/' + name);
		if (!bundleDir.exists() || !bundleDir.isDirectory()) {
			throw new IllegalArgumentException(
					"Bundle does not exist or is not a directory: " + name);
		}
		if (!recursiveDelete(bundleDir)) {
			throw new IOException(
					context_.getString(R.string.recursive_delete_failed));
		}
	}

	public void clearActiveBundle() throws IOException {
		Editor editor = preferences_.edit();
		editor.putString("activeBundleName", null);
		editor.commit();
		activeBundleName_ = null;
		for (File f : getImageFiles(activeImagesDir_)) {
			if (!f.delete()) {
				throw new IOException(String.format(
						context_.getString(R.string.failed_to_delete_file),
						f.getAbsolutePath()));
			}
		}
		for (File f : getFingerprintFiles(activeImagesDir_)) {
			if (!f.delete()) {
				throw new IOException(String.format(
						context_.getString(R.string.failed_to_delete_file),
						f.getAbsolutePath()));
			}
		}
	}

	private static File[] getImageFiles(File dir) {
		File[] files = dir.listFiles(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String filename) {
				return filename.endsWith(".ioio");
			}
		});
		return files;
	}

	private static File[] getFingerprintFiles(File dir) {
		File[] files = dir.listFiles(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String filename) {
				return filename.endsWith(".fp");
			}
		});
		return files;
	}

	private void copyFileToFilesDir(File src, int mode) throws IOException {
		OutputStream out = context_.openFileOutput(src.getName(), mode);
		InputStream in = new FileInputStream(src);
		byte[] buf = new byte[64];
		int numRead;
		while (-1 != (numRead = in.read(buf))) {
			out.write(buf, 0, numRead);
		}
		out.close();
	}

	private static boolean recursiveDelete(File f) {
		if (f.isDirectory()) {
			for (File i : f.listFiles()) {
				if (!recursiveDelete(i)) {
					return false;
				}
			}
			return f.delete();
		} else {
			return f.delete();
		}
	}

	private void createFingerprint(String filename) throws IOException,
			NoSuchAlgorithmException {
		String baseFilename = filename.substring(0, filename.lastIndexOf('.'));
		InputStream in = context_.openFileInput(filename);
		String fingerprintFilename = baseFilename + ".fp";
		OutputStream out = context_.openFileOutput(fingerprintFilename,
				Context.MODE_WORLD_READABLE);
		try {
			MessageDigest digester = MessageDigest.getInstance("MD5");
			byte[] bytes = new byte[1024];
			int byteCount;
			while ((byteCount = in.read(bytes)) > 0) {
				digester.update(bytes, 0, byteCount);
			}
			byte[] digest = digester.digest();
			out.write(digest);
		} finally {
			in.close();
			out.close();
		}
	}

}
