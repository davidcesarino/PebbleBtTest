# PebbleBtTest

I created this small test sample to demonstrate an issue with the Pebble AppMessage system. [The original discussion is on Stackoverflow][12]. Same question is [here][1].

Nothing fancy on **what I'm currently doing:**

  1. My watchface is [notified][3] that the bluetooth connection with the phone is up, using `.pebble_app_connection_handler`.
  2. In that [bluetooth callback][4] I set up, I [send][5] a [message to the phone][6] using `app_message_outbox_send()`. When BT connection is up, of course.
  3. My Android app has a `BroadcastReceiver` that listens to these messages and [calls an `IntentService`][7].
  4. This `IntentService` [calculates the data][8], [pushes it to the watch][9] and [sets itself to run again][10] after some time.

**What I expect**:

  1. When the BT connection is up, the connection handler to be called.
  2. `app_message_outbox_send()` return a value telling if the message initiation had any errors. Normally, this is `APP_MSG_OK`, but it can be `APP_MSG_BUSY`, and I'm perfectly aware of that.
  3. The app message callbacks (`app_message_register_inbox_received` and friends) be called to indicate if the asynchronous process of sending message to phone really worked. This is stated in the [docs][11].

**What I'm seeing**: 

The expected steps happen when the watchface is loaded, because I trigger the update manually. However, when the update is triggered by a BT connection event, expected steps 1 and 2 happen, but not the step 3.

This is particularly aggravating when I get `APP_MSG_OK` in step 2, because I should reasonably expect that everything on the watch went OK, and I should prepare myself to receive *something* inside the app message callbacks. Basically, I'm told by the docs to wait for a call that *never* arrives.

**This happens 100% of the time**.

Thank you for any help. I have another solution that works, using the watch to keep track of the update interval, but I believe this one allows me to save more battery by making use of recent Android features.

  [1]: https://forums.getpebble.com/discussion/31237/not-receving-app-message-callbacks-nexus-5x-basalt-3-9-2-and-aplite-3-9-1
  [2]: https://github.com/davidcesarino/PebbleBtTest
  [3]: https://github.com/davidcesarino/PebbleBtTest/blob/master/pebble/src/pebble3_app.c#L123
  [4]: https://github.com/davidcesarino/PebbleBtTest/blob/master/pebble/src/pebble3_app.c#L63
  [5]: https://github.com/davidcesarino/PebbleBtTest/blob/master/pebble/src/pebble3_app.c#L65
  [6]: https://github.com/davidcesarino/PebbleBtTest/blob/master/pebble/src/pebble3_app.c#L48
  [7]: https://github.com/davidcesarino/PebbleBtTest/blob/master/app/src/main/java/net/davidcesarino/android/pebblebttest/PebbleBroadcastReceiver.java#L40
  [8]: https://github.com/davidcesarino/PebbleBtTest/blob/master/app/src/main/java/net/davidcesarino/android/pebblebttest/PebbleUpdateService.java#L38
  [9]: https://github.com/davidcesarino/PebbleBtTest/blob/master/app/src/main/java/net/davidcesarino/android/pebblebttest/PebbleUpdateService.java#L39
  [10]: https://github.com/davidcesarino/PebbleBtTest/blob/master/app/src/main/java/net/davidcesarino/android/pebblebttest/PebbleUpdateService.java#L47
  [11]: https://developer.pebble.com/docs/c/Foundation/AppMessage/#app_message_outbox_send
  [12]: https://stackoverflow.com/q/35486745/489607
