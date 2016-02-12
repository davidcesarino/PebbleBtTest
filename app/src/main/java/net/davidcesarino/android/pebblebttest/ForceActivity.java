package net.davidcesarino.android.pebblebttest;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class ForceActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_force);

        Button button = (Button) findViewById(R.id.button1);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.w("PebbleBtTest", "User triggered manual update.");
                Context c = ForceActivity.this;
                c.startService(new Intent(c, PebbleUpdateService.class));
            }
        });

        button = (Button) findViewById(R.id.button2);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.w("PebbleBtTest", "Removing update callbacks.");
                Context c = ForceActivity.this;
                AlarmManager manager = (AlarmManager) c.getSystemService(Context.ALARM_SERVICE);
                Intent i = new Intent(getApplicationContext(), PebbleUpdateService.class);
                PendingIntent p = PendingIntent.getService(getApplicationContext(), 0, i, 0);
                manager.cancel(p);
            }
        });
    }
}
