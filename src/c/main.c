#include <pebble.h>

static Window *s_main_window;
static Layer *graph_layer;
const int ACCEL_STEP_MS = 500;
const int halfwayVert =82;

int currX, currY, currZ = 0;
int prevX = 0;
int sample = 0;
int x_coord = 0;

int graphing_x = 0;

static int cx_points[20];
static int px_points[20];

static void draw_graph(GContext *ctx){
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 3);
  //graphics_draw_line(ctx, GPoint(0, 0), GPoint(140, 165));
/*
  graphics_draw_line(ctx, GPoint(95, 6), GPoint(100, 2));
graphics_draw_line(ctx, GPoint(100, 2), GPoint(105, 4));
graphics_draw_line(ctx, GPoint(105, 0), GPoint(110, 1));
graphics_draw_line(ctx, GPoint(110, 1), GPoint(115, 6));
graphics_draw_line(ctx, GPoint(115, 6), GPoint(120, 8));
graphics_draw_line(ctx, GPoint(120, 8), GPoint(125, 6));
graphics_draw_line(ctx, GPoint(125, 6), GPoint(130, 2));
graphics_draw_line(ctx, GPoint(130, 2), GPoint(135, 4));
graphics_draw_line(ctx, GPoint(135, 4), GPoint(140, 3));
graphics_draw_line(ctx, GPoint(140, 0), GPoint(145, 1));
graphics_draw_line(ctx, GPoint(145, 1), GPoint(150, 6));
graphics_draw_line(ctx, GPoint(150, 6), GPoint(155, 8));
*/ 

  
  for (int a=0; a < sample; a++){
    int toX = x_coord+5;
    APP_LOG(APP_LOG_LEVEL_INFO, "graphics_draw_line(ctx, GPoint(%d, %d), GPoint(%d, %d))", x_coord, px_points[a], toX, cx_points[a]);
    graphics_draw_line(ctx, GPoint(x_coord, px_points[a]), GPoint(toX, cx_points[a]));
    x_coord = x_coord+5;
  }

}

static void graph_layer_callback(Layer *layer, GContext *ctx) {
  
  draw_graph(ctx);
}

static void accel_callback(void *data) {
  AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };
  accel_service_peek(&accel);
  
  
  
  prevX = currX;
  currX = abs(accel.x + halfwayVert);
  
  if (currX > prevX){
    cx_points[sample] = currX-prevX;
  } else {
    cx_points[sample] = prevX-currX;
  }
  
  px_points[sample] = cx_points[sample-1];
  
  //APP_LOG(APP_LOG_LEVEL_INFO, "c:%d p:%d", cx_points[sample], px_points[sample]);
  
  if (sample < 19){
    sample++;
  } else {
    sample = 0;
    x_coord = 0;
  }
  
  layer_mark_dirty(graph_layer);
  app_timer_register(ACCEL_STEP_MS, accel_callback, NULL);
}

static void main_window_load(Window *window) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Main wondow setup");
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //graphLayer
  graph_layer = layer_create(bounds);
  
  accel_data_service_subscribe(0, NULL);
  app_timer_register(ACCEL_STEP_MS, accel_callback, NULL);

  layer_set_update_proc(graph_layer, graph_layer_callback);
  
  layer_add_child(window_get_root_layer(window), graph_layer);

}

static void main_window_unload(Window *window) {
  layer_destroy(graph_layer);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  accel_data_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
 