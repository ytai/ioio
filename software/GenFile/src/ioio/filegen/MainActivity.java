package ioio.filegen;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		try {
			super.onCreate(savedInstanceState);
			setContentView(R.layout.main);
			FileOutputStream fos = openFileOutput("sample.ioio", MODE_WORLD_READABLE);
			InputStream is = getAssets().open("sample.ioio");
			int i;
			while (-1 != (i = is.read())) {
				fos.write(i);
			}
			is.close();
			fos.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}