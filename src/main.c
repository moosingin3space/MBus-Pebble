#include <pebble.h>
#include <pebble_fonts.h>

#define KEY_STOPNAME 0
#define KEY_BUSNAME 1
#define KEY_TIME 2
  
static Window *s_main_window;
static TextLayer *s_stop_label;
static TextLayer *s_stop_layer;
static TextLayer *s_departs_label;
static TextLayer *s_arrival;
static TextLayer *s_sign;
static GFont s_temp_font;

static void main_window_load(Window *window) {
  s_temp_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DOS_VGA_15));
  
  s_stop_label = text_layer_create(GRect(5, 25, 139, 20));
  text_layer_set_text_alignment(s_stop_label, GTextAlignmentCenter);
  text_layer_set_text(s_stop_label, "Nearest Stop:");
  text_layer_set_font(s_stop_label, s_temp_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_stop_label));
  
  s_stop_layer = text_layer_create(GRect(5, 45, 139, 20));
  text_layer_set_text_alignment(s_stop_layer, GTextAlignmentCenter);
  text_layer_set_text(s_stop_layer, "Loading...");
  text_layer_set_font(s_stop_layer, s_temp_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_stop_layer));
  
  s_departs_label = text_layer_create(GRect(5, 90, 139, 20));
  text_layer_set_text_alignment(s_departs_label, GTextAlignmentCenter);
  text_layer_set_text(s_departs_label, "Arriving in:");
  text_layer_set_font(s_departs_label, s_temp_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_departs_label));
  
  s_arrival = text_layer_create(GRect(5, 110, 139, 20));
  text_layer_set_text_alignment(s_arrival, GTextAlignmentCenter);
  text_layer_set_font(s_arrival, s_temp_font);
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
  static char stop_name_buffer[20];
  static char stop_busName_buffer[20];
  static char stop_time_buffer[20];
  
  Tuple *t = dict_read_first(iterator);
  
  while(t != NULL) {
    switch(t->key) {
      case KEY_STOPNAME:
        snprintf(stop_name_buffer, sizeof(stop_name_buffer), "%s", t->value->cstring);
      case KEY_BUSNAME:
        snprintf(stop_busName_buffer, sizeof(stop_busName_buffer), "%s", t->value->cstring);
        break;
      case KEY_TIME:
        snprintf(stop_time_buffer, sizeof(stop_time_buffer), "%s", t->value->cstring);
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
        break;
    }
    
    t = dict_read_next(iterator); 
  }
  
  vibes_double_pulse();
  text_layer_set_text(s_stop_layer, stop_name_buffer);
  text_layer_set_text(s_arrival, stop_time_buffer);
  text_layer_set_text(s_sign, stop_busName_buffer);
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