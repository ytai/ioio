package ioio.manager;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class IOIOManagerBTActivity extends Activity {
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		startService(new Intent(this, IOIOManagerBTService.class));
	}

	@Override
	protected void onStart() {
		super.onStart();
		finish();
	}
}
