package ioio.tests.torture;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIO.VersionType;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;
import ioio.tests.torture.ResourceAllocator.Board;

import android.content.Context;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends IOIOActivity {
    private static final String TAG = "IOIO:TortureTest";
    private PowerManager.WakeLock wakeLock_;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wakeLock_ = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK
                | PowerManager.ON_AFTER_RELEASE, TAG);
    }

    @Override
    protected IOIOLooper createIOIOLooper() {
        return new Looper();
    }

    class Looper extends BaseIOIOLooper {
        private final TestThread[] workers_ = new TestThread[8];

        @Override
        protected void setup() {
            wakeLock_.acquire(10*60*1000L /*10 minutes*/);
            ResourceAllocator alloc_ = new ResourceAllocator(Board.valueOf(ioio_
                    .getImplVersion(IOIO.VersionType.HARDWARE_VER)));
            TestProvider provider = new TestProvider(MainActivity.this, ioio_, alloc_);
            showVersions();
            for (int i = 0; i < workers_.length; ++i) {
                workers_[i] = new TestThread(provider, "Test-" + i);
                workers_[i].start();
            }
        }

        @Override
        public void loop() {
        }

        @Override
        public void disconnected() {
            Log.i(TAG, "IOIO disconnected, killing workers");
            for (TestThread testThread : workers_) {
                if (testThread != null) {
                    testThread.abort();
                }
            }
            try {
                for (TestThread testThread : workers_) {
                    if (testThread != null) {
                        testThread.join();
                    }
                }
                Log.i(TAG, "All workers dead");
            } catch (InterruptedException e) {
                Log.w(TAG, "Interrupted. Some workers may linger.");
            }
            wakeLock_.release();
        }

        @Override
        public void incompatible() {
            Log.e(TAG, "Incompatibility detected");
        }

        private void showVersions() {
            final String versionText = "hw: " + ioio_.getImplVersion(VersionType.HARDWARE_VER)
                    + "\n" + "bl: " + ioio_.getImplVersion(VersionType.BOOTLOADER_VER) + "\n"
                    + "fw: " + ioio_.getImplVersion(VersionType.APP_FIRMWARE_VER) + "\n" + "lib: "
                    + ioio_.getImplVersion(VersionType.IOIOLIB_VER);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ((TextView) findViewById(R.id.versions)).setText(versionText);
                }
            });
        }
    }
}
