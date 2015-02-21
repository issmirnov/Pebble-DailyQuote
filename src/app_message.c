// Daily Quote - Pebble App Component. 
// Author - Ivan Smirnov (http://ivansmirnov.name), isgsmirnov@gmail.com

#include <pebble.h>

Window *window;	
TextLayer *text_layer;
	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1
};

// Write message to buffer & send
void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x1);
	
	dict_write_end(iter);
  	app_message_outbox_send();
}

static void main_window_load(Window *window) {
  text_layer = text_layer_create(GRect(0, 0, 144, 154));
	
	// Set the text, font, and text alignment
	text_layer_set_text(text_layer, "Loading quote...");
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));  
}

static void main_window_unload(Window *window) {
  // Destroy Window's child Layers here
  text_layer_destroy(text_layer);
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
    char* quote = tuple->value->cstring;
    // add hook to display message
    text_layer_set_text(text_layer, quote); 
	}}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

void single_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Loading quote...");
  send_message(); // async call to update quote
}

// Register click handlers
void config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_DOWN, single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, single_click_handler);
}


void init(void) {
  // Create a window and text layer
	window = window_create();
	
  // window set andlers, window load handlers
  window_set_window_handlers(window, (WindowHandlers) {
  .load = main_window_load,
  .unload = main_window_unload,
  });
  
	// Push the window
	window_stack_push(window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
  
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  // click handler
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  
  // get quote
	send_message();
}

void deinit(void) {
	app_message_deregister_callbacks();
	// Destroy the window
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}