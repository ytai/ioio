package ioio.examples.hello_service;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOService;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;

import androidx.core.app.NotificationCompat;

/**
 * An example IOIO service. While this service is alive, it will attempt to
 * connect to a IOIO and blink the LED. A notification will appear on the
 * notification bar, enabling the user to stop the service.
 */
public class HelloIOIOService extends IOIOService {
    @Override
    protected IOIOLooper createIOIOLooper() {
        return new BaseIOIOLooper() {
            private DigitalOutput led_;

            @Override
            protected void setup() throws ConnectionLostException {
                led_ = ioio_.openDigitalOutput(IOIO.LED_PIN);
            }

            @Override
            public void loop() throws ConnectionLostException,
                    InterruptedException {
                led_.write(false);
                Thread.sleep(500);
                led_.write(true);
                Thread.sleep(500);
            }
        };
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        int result = super.onStartCommand(intent, flags, startId);
        NotificationManager notificationManager = (NotificationManager) getBaseContext().getSystemService(Context.NOTIFICATION_SERVICE);
        if (intent != null && intent.getAction() != null
                && intent.getAction().equals("stop")) {
            // User clicked the notification. Need to stop the service.
            if (notificationManager != null) {
                notificationManager.cancel(0);
            }
            stopSelf();
        } else {
            NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(getBaseContext());

            int pendingIntentFlags;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                pendingIntentFlags = PendingIntent.FLAG_IMMUTABLE | PendingIntent.FLAG_UPDATE_CURRENT;
            } else {
                pendingIntentFlags = PendingIntent.FLAG_UPDATE_CURRENT;
            }

            notificationBuilder
                    .setSmallIcon(R.drawable.ic_launcher)
                    .setTicker("IOIO service running")
                    .setWhen(System.currentTimeMillis())
                    .setContentTitle("IOIO Service")
                    .setContentText("Click to stop")
                    .setContentIntent(PendingIntent.getService(this, 0, new Intent(
                            "stop", null, this, this.getClass()), pendingIntentFlags));

            if (notificationManager != null) {
                notificationManager.notify(1, notificationBuilder.build());
            }

        }
        return result;
    }

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

}
