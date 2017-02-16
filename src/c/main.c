#include <pebble.h>

//APP_LOG(APP_LOG_LEVEL_DEBUG, "LOG MESSAGE");

// window layers
static Window *main_window;
static Layer *canvas_layer;
static TextLayer *brand_layer;
static TextLayer *date_layer;
static TextLayer *time_layer;
static TextLayer *location_layer;
static TextLayer *water_layer;
static TextLayer *wr_layer;
static TextLayer *resist_layer;

// battery level
static int battery_level;
static Layer *battery_layer;
static TextLayer *battery_percentage;

// bluetooth icon
static BitmapLayer *bt_icon_layer;
static GBitmap *bt_icon_bitmap;
bool bt_startup = true;

// health events
static char steps_buffer[12];
static char sleep_buffer[11];
bool sleep_bool;

// saved settings
uint32_t hour_setting   = 0;
uint32_t date_setting   = 1;
uint32_t health_setting = 2;
uint32_t degree_setting = 3;
bool hour_bool;
bool date_bool;
bool health_bool;
bool degree_bool;

// load date/time
static char time12_buffer[6];
static char time24_buffer[6];
static char datedm_buffer[17];
static char datemd_buffer[17];

// load weather
static char location_cel_buffer[19];
static char location_fah_buffer[19];

// load options
static void load_options() {
  // load 24 or 12 hours
  if (persist_exists(hour_setting)) {
    char hour_buffer[5];
    persist_read_string(hour_setting, hour_buffer, sizeof(hour_buffer));
    if (strcmp(hour_buffer, "true") == 0) {
      hour_bool = true;
      text_layer_set_text(time_layer, time12_buffer);
    } else {
      hour_bool = false;
      text_layer_set_text(time_layer, time24_buffer);
    }
  } else {
    hour_bool = false;
  }

  // load date order
  if (persist_exists(date_setting)) {
    char date_buffer[5];
    persist_read_string(date_setting, date_buffer, sizeof(date_buffer));
    if (strcmp(date_buffer, "true") == 0) {
      date_bool = true;
      text_layer_set_text(date_layer, datemd_buffer);
    } else {
      date_bool = false;
      text_layer_set_text(date_layer, datedm_buffer);
    }
  } else {
    date_bool = false;
  }
  
  // load health
  if (persist_exists(health_setting)) {
    char health_buffer[5];
    persist_read_string(health_setting, health_buffer, sizeof(health_buffer));
    if (strcmp(health_buffer, "true") == 0) {
      health_bool = true;
      if (sleep_bool) {
        if (strlen(sleep_buffer) != 0) {
          text_layer_set_text(location_layer, sleep_buffer);
        } else {
          text_layer_set_text(location_layer, "sleep...");
        }
      } else {
        if (strlen(steps_buffer) != 0) {
          text_layer_set_text(location_layer, steps_buffer);
        } else {
          text_layer_set_text(location_layer, "steps...");
        }
      }
    } else {
      health_bool = false;
    }
  } else {
    health_bool = false;
  }

  // load location
  if (!health_bool) {
    if (persist_exists(degree_setting)) {
      char degree_buffer[5];
      persist_read_string(degree_setting, degree_buffer, sizeof(degree_buffer));
      if (strcmp(degree_buffer, "true") == 0) {
        degree_bool = true;
        if (strlen(location_cel_buffer) != 0) {
          text_layer_set_text(location_layer, location_cel_buffer);
        } else {
          text_layer_set_text(location_layer, "digital v2");
        }
      } else {
        degree_bool = false;
        if (strlen(location_fah_buffer) != 0) {
          text_layer_set_text(location_layer, location_fah_buffer);
        } else {
          text_layer_set_text(location_layer, "digital v2");
        }
      }
    } else {
      degree_bool = false;
    }
  }
}

// update options/weather
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // collect options
  Tuple *hour_tuple   = dict_find(iterator, MESSAGE_KEY_HOUR);
  Tuple *date_tuple   = dict_find(iterator, MESSAGE_KEY_DATE);
  Tuple *degree_tuple = dict_find(iterator, MESSAGE_KEY_DEGREE);
  Tuple *health_tuple = dict_find(iterator, MESSAGE_KEY_HEALTH);

  // save options
  if(hour_tuple && date_tuple && degree_tuple) {
    char *hour_string   = hour_tuple->value->cstring;
    char *date_string   = date_tuple->value->cstring;
    char *degree_string = degree_tuple->value->cstring;
    persist_write_string(hour_setting,   hour_string);
    persist_write_string(date_setting,   date_string);
    persist_write_string(degree_setting, degree_string);
    if (health_tuple) {
      char *health_string = health_tuple->value->cstring;
      persist_write_string(health_setting, health_string);
    }
    load_options();
  }

  // collect weather
  Tuple *location_tuple = dict_find(iterator, MESSAGE_KEY_LOCATION);
  Tuple *temp_cel_tuple = dict_find(iterator, MESSAGE_KEY_TEMP_CEL);
  Tuple *temp_fah_tuple = dict_find(iterator, MESSAGE_KEY_TEMP_FAH);

  // display location
  if (location_tuple && temp_cel_tuple && temp_fah_tuple) {
    static char location_buffer[12], temp_fah_buffer[7], temp_cel_buffer[6];
    snprintf(location_buffer, sizeof(location_buffer), "%s", location_tuple->value->cstring);
    snprintf(temp_fah_buffer, sizeof(temp_fah_buffer), "%d°F", (int)temp_fah_tuple->value->int32);
    snprintf(temp_cel_buffer, sizeof(temp_cel_buffer), "%d°C", (int)temp_cel_tuple->value->int32);
    snprintf(location_fah_buffer, sizeof(location_fah_buffer), "%s", location_buffer);
    snprintf(location_cel_buffer, sizeof(location_cel_buffer), "%s", location_buffer);
    int fah_length = strlen(location_fah_buffer);
    int cel_length = strlen(location_cel_buffer);
    snprintf(location_fah_buffer+fah_length, (sizeof location_fah_buffer) - fah_length, " %s", temp_fah_buffer);
    snprintf(location_cel_buffer+cel_length, (sizeof location_cel_buffer) - cel_length, " %s", temp_cel_buffer);
    if (!health_bool) {
      if (!degree_bool) {
        text_layer_set_text(location_layer, location_fah_buffer);
      } else {
        text_layer_set_text(location_layer, location_cel_buffer);
      }
    }
  }
}

// health event changed
#if defined(PBL_HEALTH)
  static void health_handler(HealthEventType event, void *context) {
    if (event == HealthEventMovementUpdate) {
      sleep_bool = false;
      snprintf(steps_buffer, sizeof(steps_buffer), "%d steps", (int)health_service_sum_today(HealthMetricStepCount));
      if (health_bool) {
        text_layer_set_text(location_layer, steps_buffer);
      }
    } else if (event == HealthEventSleepUpdate) {
      sleep_bool = true;
      static int seconds, minutes, hours, remainder;
      static char mins_buffer[4], hour_buffer[4];
      seconds = (int)health_service_sum_today(HealthMetricSleepSeconds);
      hours = seconds / 3600;
      remainder = seconds % 3600;
      minutes = remainder / 60;
      snprintf(hour_buffer, sizeof(hour_buffer), "%dh", (int)hours);
      snprintf(mins_buffer, sizeof(mins_buffer), "%dm", (int)minutes);
      snprintf(sleep_buffer, sizeof(sleep_buffer), "zZZ %s", hour_buffer);
      int length = strlen(sleep_buffer);
      snprintf(sleep_buffer+length, (sizeof sleep_buffer) - length, "%s", mins_buffer);
      if (health_bool) {
        text_layer_set_text(location_layer, sleep_buffer);
      }
    }
  }
#endif

// bluetooth connection change
static void bluetooth_callback(bool connected) {
  layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), connected);
  if(!bt_startup) {
    vibes_double_pulse();
  }
  bt_startup = false;
}

// collect battery level
static void battery_callback(BatteryChargeState state) {
  battery_level = state.charge_percent;
  layer_mark_dirty(battery_layer);
}

// draw battery level
static void battery_update_proc(Layer *layer, GContext *ctx) {
  // get canvas size
  GRect bounds = layer_get_bounds(layer);
  int my = bounds.size.h;
  int cx = bounds.size.w/2;
  int cy = bounds.size.h/2;
  
  // collect battery level
  static char battery_buffer[5];
  snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", (int8_t)battery_level);

  // display battery percentage
  battery_percentage = text_layer_create(GRect(cx+20,cy-76,30,my));
  text_layer_set_background_color(battery_percentage, GColorClear);
  text_layer_set_text_alignment(battery_percentage, GTextAlignmentCenter);
  text_layer_set_font(battery_percentage, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(battery_percentage, battery_buffer);
  layer_add_child(layer, text_layer_get_layer(battery_percentage));
}

// update date/time
static void update_datetime() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // define date/time
  strftime(time24_buffer, sizeof(time24_buffer), "%H:%M", tick_time);
  strftime(time12_buffer, sizeof(time12_buffer), "%I:%M", tick_time);
  strftime(datedm_buffer, sizeof(datedm_buffer), "%A %d %b", tick_time);
  strftime(datemd_buffer, sizeof(datemd_buffer), "%A %b %d", tick_time);

  // display time
  if (!hour_bool) {
    text_layer_set_text(time_layer, time24_buffer);
  } else {
    text_layer_set_text(time_layer, time12_buffer);
  }

  // display date
  if (!date_bool) {
    text_layer_set_text(date_layer, datedm_buffer);
  } else {
    text_layer_set_text(date_layer, datemd_buffer);
  }
}
static void mins_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_datetime();
}

// drawing canvas
static void draw_canvas(Layer *layer, GContext *ctx) {
  // get canvas size
  GRect bounds = layer_get_bounds(layer);
  int mx = bounds.size.w;
  int cx = bounds.size.w/2;
  int cy = bounds.size.h/2;
  
  // set colours
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorWhite);

  // draw battery
  GRect battery_header = GRect(cx+15,cy-72,10,11);
  GRect battery_square = GRect(cx+20,cy-77,30,21);
  graphics_draw_rect(ctx, battery_header);
  graphics_draw_rect(ctx, battery_square);
  graphics_fill_rect(ctx, battery_header, 1, GCornersAll);
  graphics_fill_rect(ctx, battery_square, 1, GCornersAll);

  // draw time box
  GRect time_square = GRect(0,cy-23,mx,50);
  graphics_draw_rect(ctx, time_square);
  graphics_fill_rect(ctx, time_square, 0, GCornersAll);
  
  // draw water resist boxes
  GRect water_square  = GRect(cx-17,cy+62,2,17);
  GRect wr_square     = GRect(cx-12,cy+62,24,17);
  GRect resist_square = GRect(cx+15,cy+62,2,17);
  graphics_draw_rect(ctx, water_square);
  graphics_draw_rect(ctx, wr_square);
  graphics_draw_rect(ctx, resist_square);
  graphics_fill_rect(ctx, water_square,  0, GCornersAll);
  graphics_fill_rect(ctx, wr_square,     0, GCornersAll);
  graphics_fill_rect(ctx, resist_square, 0, GCornersAll);

  // draw lines
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(0,cy-49), GPoint(mx,cy-49));
  graphics_draw_line(ctx, GPoint(0,cy-48), GPoint(mx,cy-48));
  graphics_draw_line(ctx, GPoint(0,cy+53), GPoint(mx,cy+53));
  graphics_draw_line(ctx, GPoint(0,cy+54), GPoint(mx,cy+54));

  // draw guidelines
  //int my = bounds.size.h;
  //graphics_draw_line(ctx, GPoint(cx-50,0), GPoint(cx-50,my));
  //graphics_draw_line(ctx, GPoint(cx+50,0), GPoint(cx+50,my));
}

// window load
static void main_window_load(Window *window) {
  // collect window size
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int mx = bounds.size.w;
  int my = bounds.size.h;
  int cx = bounds.size.w/2;
  int cy = bounds.size.h/2;

  // drawing canvas
  canvas_layer = layer_create(bounds);
  layer_set_update_proc(canvas_layer, draw_canvas);
  layer_add_child(window_layer, canvas_layer);

  // brand layer
  brand_layer = text_layer_create(GRect(-cx+4,cy-84,mx,my));
  text_layer_set_background_color(brand_layer, GColorClear);
  text_layer_set_text_color(brand_layer, GColorWhite);
  text_layer_set_text_alignment(brand_layer, GTextAlignmentRight);
  text_layer_set_font(brand_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(brand_layer, "pebble");
  layer_add_child(window_layer, text_layer_get_layer(brand_layer));
  
  // battery layer
  battery_layer = layer_create(bounds);
  layer_set_update_proc(battery_layer, battery_update_proc);
  layer_add_child(window_get_root_layer(window), battery_layer);
  battery_callback(battery_state_service_peek());

  // date layer
  date_layer = text_layer_create(GRect(0,cy-48,mx,my));
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  // time layer
  time_layer = text_layer_create(GRect(0,cy-30,mx,my));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  // location layer
  location_layer = text_layer_create(GRect(0,cy+27,mx,my));
  text_layer_set_background_color(location_layer, GColorClear);
  text_layer_set_text_color(location_layer, GColorWhite);
  text_layer_set_text_alignment(location_layer, GTextAlignmentCenter);
  text_layer_set_font(location_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(location_layer, "digital v2");
  layer_add_child(window_layer, text_layer_get_layer(location_layer));
  
  // water resist layers
  water_layer = text_layer_create(GRect(-cx-20,cy+62,mx,my));
  wr_layer = text_layer_create(GRect(0,cy+58,mx,my));
  resist_layer = text_layer_create(GRect(cx+20,cy+62,mx,my));
  text_layer_set_background_color(water_layer, GColorClear);
  text_layer_set_background_color(wr_layer, GColorClear);
  text_layer_set_background_color(resist_layer, GColorClear);
  text_layer_set_text_color(water_layer, GColorWhite);
  text_layer_set_text_color(resist_layer, GColorWhite);
  text_layer_set_text_alignment(water_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(wr_layer, GTextAlignmentCenter);
  text_layer_set_font(water_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_font(wr_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_font(resist_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(water_layer, "water");
  text_layer_set_text(wr_layer, "WR");
  text_layer_set_text(resist_layer, "resist");
  layer_add_child(window_layer, text_layer_get_layer(water_layer));
  layer_add_child(window_layer, text_layer_get_layer(wr_layer));
  layer_add_child(window_layer, text_layer_get_layer(resist_layer));
  
  // bluetooth layer
  bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT_ICON);
  bt_icon_layer = bitmap_layer_create(GRect(cx-12,cy+62,24,17));
  bitmap_layer_set_bitmap(bt_icon_layer, bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bt_icon_layer));
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  
  // health layer
  #if defined(PBL_HEALTH)
    if(health_service_events_subscribe(health_handler, NULL)) {
      HealthActivityMask activities = health_service_peek_current_activities();
      if((activities & HealthActivitySleep) || (activities & HealthActivityRestfulSleep)) {
        health_handler(HealthEventSleepUpdate, NULL);
      } else {
        health_handler(HealthEventMovementUpdate, NULL);
      }
    }
  #endif
}

// window unload
static void main_window_unload(Window *window) {
  // unsubscribe from events
  #if defined(PBL_HEALTH)
    health_service_events_unsubscribe();
  #endif
  // destroy image layers
  bitmap_layer_destroy(bt_icon_layer);
  gbitmap_destroy(bt_icon_bitmap);
  // destroy text layers
  text_layer_destroy(location_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(battery_percentage);
  text_layer_destroy(brand_layer);
  // destroy canvas layers
  layer_destroy(battery_layer);
  layer_destroy(canvas_layer);
}

// init
static void init() {
  // create window
  main_window = window_create();
  window_set_background_color(main_window, GColorBlack);

  // load/unload window
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // send window to screen
  window_stack_push(main_window, true);

  // load options
  load_options();

  // update date/time
  update_datetime();
  tick_timer_service_subscribe(MINUTE_UNIT, mins_tick_handler);

  // update battery level
  battery_state_service_subscribe(battery_callback);

  // check bluetooth connection
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });

  // update options/weather
  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(64,0);  
}

// deinit
static void deinit() {
  // unsubscribe from events
  connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  // destroy window
  window_destroy(main_window);
}

// main
int main(void) {
  init();
  app_event_loop();
  deinit();
}