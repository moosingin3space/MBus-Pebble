#include <pebble.h>
#include <pebble_fonts.h>

#define KEY_STOPID 0
#define KEY_STOPNAME 1
#define KEY_ARRIVAL 2
#define KEY_SIGN 3
  
static Window *s_main_window;
static TextLayer *s_stop_label;
static TextLayer *s_stop_layer;
static TextLayer *s_departs_label;
static TextLayer *s_arrival;
static TextLayer *s_sign;
static GFont s_temp_font;
static GFont s_temp_font_12;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  if(tick_time->tm_min % 1 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    dict_write_uint8(iter, 0, 0);
    
    app_message_outbox_send();
  }
}

static void main_window_load(Window *window) {
  s_temp_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_15));
  s_temp_font_12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_12));
  
  s_stop_label = text_layer_create(GRect(5, 25, 139, 20));
  text_layer_set_text_alignment(s_stop_label, GTextAlignmentCenter);
  text_layer_set_text(s_stop_label, "Nearest Stop:");
  text_layer_set_font(s_stop_label, s_temp_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_stop_label));
  
  s_stop_layer = text_layer_create(GRect(5, 45, 139, 20));
  text_layer_set_text_alignment(s_stop_layer, GTextAlignmentCenter);
  text_layer_set_text(s_stop_layer, "Loading...");
  text_layer_set_font(s_stop_layer, s_temp_font_12);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_stop_layer));
  
  s_departs_label = text_layer_create(GRect(5, 90, 139, 20));
  text_layer_set_text_alignment(s_departs_label, GTextAlignmentCenter);
  text_layer_set_text(s_departs_label, "Arriving in:");
  text_layer_set_font(s_departs_label, s_temp_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_departs_label));
  
  s_arrival = text_layer_create(GRect(5, 110, 139, 25));
  text_layer_set_text(s_arrival, "Loading...");
  text_layer_set_text_alignment(s_arrival, GTextAlignmentCenter);
  text_layer_set_font(s_arrival, s_temp_font_12);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_arrival));
  
  s_sign = text_layer_create(GRect(5, 70, 139, 25));
  text_layer_set_text_alignment(s_sign, GTextAlignmentCenter);
  text_layer_set_font(s_sign, s_temp_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_sign));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_stop_label);
  text_layer_destroy(s_stop_layer);
  text_layer_destroy(s_arrival);
  text_layer_destroy(s_sign);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char stop_id_buffer[64];
  static char stop_name_buffer[64];
  static char stop_arrival_time[64];
  static char sign_buffer[64];
  
  Tuple *t = dict_read_first(iterator);
  
  while(t != NULL) {
    switch(t->key) {
      case KEY_STOPID:
        strncpy(stop_id_buffer, t->value->cstring, sizeof(stop_id_buffer));
        break;
      case KEY_STOPNAME:
        strncpy(stop_name_buffer, t->value->cstring, sizeof(stop_name_buffer));
        break;
      case KEY_ARRIVAL:
        strncpy(stop_arrival_time, t->value->cstring, sizeof(stop_arrival_time));
        break;
      case KEY_SIGN:
        strncpy(sign_buffer, t->value->cstring, sizeof(sign_buffer));
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
        break;
    }
    
    t = dict_read_next(iterator); 
  }
  
  APP_LOG(APP_LOG_LEVEL_INFO, "arriving in %s", stop_arrival_time);
  vibes_double_pulse();
  text_layer_set_text(s_stop_layer, stop_name_buffer);
  text_layer_set_text(s_arrival, stop_arrival_time);
  text_layer_set_text(s_sign, sign_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
static void init() {
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  update_time();
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  window_destroy(s_main_window);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}