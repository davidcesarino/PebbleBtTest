#include <pebble.h>
#include "mini-printf.h"

// SDK Version  |   PebbleKit JS app (Bytes)  |  PebbleKit Android/iOS app (Bytes)
// --------------------------------------------------------------------------------
// 2.7 - 3.4    |   Inbox: 2026, outbox: 656  |  Inbox: 126, outbox: 656
// --------------------------------------------------------------------------------
// 3.6+         |                   Inbox: 8200, outbox: 8200
// --------------------------------------------------------------------------------
#define APPMESSAGE_IN_SIZE             126
#define APPMESSAGE_OUT_SIZE            126

#define KEY_BEACON 0x11

static Window *s_window;

static TextLayer *s_textlayer_beacon;
static TextLayer *s_textlayer_in_rcv;
static TextLayer *s_textlayer_in_drp;
static TextLayer *s_textlayer_out_fl;
static TextLayer *s_textlayer_out_st;
static TextLayer *s_textlayer_data;

static char beacon_msg[20];
static char in_rcv_msg[20];
static char in_drp_msg[20];
static char out_fl_msg[20];
static char out_st_msg[20];

static int beacons_sent;
static int in_rcv;
static int in_drp;
static int out_fl;
static int out_st;
static int send_code;
static int timeout;
static int retry = 3;

static AppTimer * timer;

void send_beacon(void *data) {
    beacons_sent++;

    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    int request_value = KEY_BEACON;
    dict_write_int(iter, request_value, &request_value, sizeof(int), true);
    send_code = app_message_outbox_send();

    if (send_code == 64 && retry > 0) {
        timeout = timeout * 2;
        timer = app_timer_register(timeout, send_beacon, NULL);
        retry--;
    } else {
        timeout = 2500;
        retry = 3;
    }

    mini_snprintf(beacon_msg, 20, "[B]=%d,%d,%d", send_code, beacons_sent, retry);
    text_layer_set_text(s_textlayer_beacon, beacon_msg);
}

void bluetooth_connection_callback(bool connected) {
    if (connected) {
        send_beacon(NULL);
    } else {
        mini_snprintf(beacon_msg, 20, "[C]=%d,%d", send_code, beacons_sent);
        text_layer_set_text(s_textlayer_beacon, beacon_msg);
    }
}

void inbox_received_callback(DictionaryIterator *received, void *context) {
    in_rcv++;
    mini_snprintf(in_rcv_msg, 20, "[IR]=%d,%d", send_code, in_rcv);
    text_layer_set_text(s_textlayer_in_rcv, in_rcv_msg);

    Tuple *tuple = dict_find(received, KEY_BEACON);
    if (tuple) {
        static char value[4];
        mini_snprintf(value, 4, "%d", tuple->value->uint8);
        text_layer_set_text(s_textlayer_data, value);
    } else {
        text_layer_set_text(s_textlayer_data, "In without Tuple.");
    }
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
    in_drp++;
    mini_snprintf(in_drp_msg, 20, "[ID]=%d,%d", send_code, in_drp);
    text_layer_set_text(s_textlayer_in_drp, in_drp_msg);
}

void outbox_failed_callback(DictionaryIterator *failed, AppMessageResult reason,
        void *context) {
    out_fl++;
    mini_snprintf(out_fl_msg, 20, "[OF]=%d,%d", send_code, out_fl);
    text_layer_set_text(s_textlayer_out_fl, out_fl_msg);
}

void outbox_sent_callback(DictionaryIterator *sent, void *context) {
    out_st++;
    mini_snprintf(out_st_msg, 20, "[OS]=%d,%d", send_code, out_st);
    text_layer_set_text(s_textlayer_out_st, out_st_msg);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(s_window);
    s_textlayer_beacon = text_layer_create(GRect(2,  2, 120, 15));
    s_textlayer_in_drp = text_layer_create(GRect(2, 17, 120, 15));
    s_textlayer_in_rcv = text_layer_create(GRect(2, 32, 120, 15));
    s_textlayer_out_fl = text_layer_create(GRect(2, 47, 120, 15));
    s_textlayer_out_st = text_layer_create(GRect(2, 62, 120, 15));
    s_textlayer_data   = text_layer_create(GRect(2, 77, 120, 15));

    layer_add_child(window_layer, text_layer_get_layer(s_textlayer_beacon));
    layer_add_child(window_layer, text_layer_get_layer(s_textlayer_in_drp));
    layer_add_child(window_layer, text_layer_get_layer(s_textlayer_in_rcv));
    layer_add_child(window_layer, text_layer_get_layer(s_textlayer_out_fl));
    layer_add_child(window_layer, text_layer_get_layer(s_textlayer_out_st));
    layer_add_child(window_layer, text_layer_get_layer(s_textlayer_data));

    connection_service_subscribe((ConnectionHandlers) {
            .pebble_app_connection_handler = bluetooth_connection_callback
    });
    bluetooth_connection_callback(connection_service_peek_pebble_app_connection());
}

static void window_unload(Window *window) {
    connection_service_unsubscribe();
    text_layer_destroy(s_textlayer_beacon);
    text_layer_destroy(s_textlayer_in_drp);
    text_layer_destroy(s_textlayer_in_rcv);
    text_layer_destroy(s_textlayer_out_fl);
    text_layer_destroy(s_textlayer_out_st);
    text_layer_destroy(s_textlayer_data);
}

static void init(void) {
    beacons_sent = 0;
    timeout = 2500;
    in_rcv = 0;
    in_drp = 0;
    out_fl = 0;
    out_st = 0;
    send_code = -1;

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_open(APPMESSAGE_IN_SIZE, APPMESSAGE_OUT_SIZE);

    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
    });
    window_stack_push(s_window, true);
}

static void deinit(void) {
    window_destroy(s_window);
    app_message_deregister_callbacks();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}

