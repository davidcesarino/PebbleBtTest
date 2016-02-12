package net.davidcesarino.android.pebblebttest;

import android.app.AlarmManager;
import android.app.IntentService;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.SystemClock;
import android.util.Log;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.Random;
import java.util.UUID;

import static net.davidcesarino.android.pebblebttest.PebbleBroadcastReceiver.KEY_BEACON;
import static net.davidcesarino.android.pebblebttest.PebbleBroadcastReceiver.MY_UUID;

public class PebbleUpdateService extends IntentService {

    private static final long UPDATE_INTERVAL_30S = 1000 * 30;

    public PebbleUpdateService() {
        super("PebbleUpdateService");
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        if (!PebbleKit.isWatchConnected(getApplicationContext())) {
            Log.w("PebbleBtTest", "Skipping Pebble update. Reason: watch not connected.");
            return;
        }

        PebbleDictionary dictionary = new PebbleDictionary();
        Random random = new Random();
        dictionary.addUint8(KEY_BEACON, (byte) (1 + random.nextInt(250)));
        PebbleKit.sendDataToPebble(getApplicationContext(), MY_UUID, dictionary);

        Log.w("PebbleBtTest", "Scheduling next Pebble update for ~30s from now.");
        AlarmManager alarmManager = (AlarmManager) getApplicationContext()
                .getSystemService(Context.ALARM_SERVICE);
        Intent selfCall = new Intent(getApplicationContext(), PebbleUpdateService.class);
        PendingIntent pending = PendingIntent.getService(getApplicationContext(), 0, selfCall, 0);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            alarmManager.setExactAndAllowWhileIdle(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                    SystemClock.elapsedRealtime() + UPDATE_INTERVAL_30S, pending);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            alarmManager.setExact(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                        SystemClock.elapsedRealtime() + UPDATE_INTERVAL_30S, pending);
        } else {
            alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                    SystemClock.elapsedRealtime() + UPDATE_INTERVAL_30S, pending);
        }
    }
}
