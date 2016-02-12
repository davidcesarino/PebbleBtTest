package net.davidcesarino.android.pebblebttest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.getpebble.android.kit.Constants;
import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.UUID;

import static com.getpebble.android.kit.Constants.APP_UUID;
import static com.getpebble.android.kit.Constants.INTENT_APP_RECEIVE;

public final class PebbleBroadcastReceiver extends BroadcastReceiver {

    public final static UUID MY_UUID = UUID.fromString("cccccccc-dddd-aaaa-eeee-998877665544");
    public final static byte KEY_BEACON = 0x11;

    @Override
    public void onReceive(Context context, Intent i) {
        if (i != null && i.getAction() != null && i.getAction().equals(INTENT_APP_RECEIVE)) {

            final UUID uuid = (UUID) i.getSerializableExtra(APP_UUID);
            if (uuid == null || !uuid.equals(MY_UUID)) return;

            final String json = i.getStringExtra(Constants.MSG_DATA);
            final int transactionId = i.getIntExtra(Constants.TRANSACTION_ID, Integer.MIN_VALUE);

            if (json == null || json.length() <= 0 || transactionId == Integer.MIN_VALUE) {
                Log.w("PebbleBtTest", "Nacking corrupted information to our app.");
                PebbleKit.sendNackToPebble(context, transactionId);
                return;
            }

            Log.w("PebbleBtTest", "Acking OK, service should start now.");
            PebbleKit.sendAckToPebble(context, transactionId);
            context.startService(new Intent(context, PebbleUpdateService.class));
        }
    }

}
