#pragma once

#include <pebble.h>

/* Timely header file - structs and function prototypes
 * ( to be split out into pieces, for dynamic overlays, at a later date )
 */

// Create a struct to hold our persistent settings...
typedef struct persist { // 46 bytes
  uint8_t version;                // version key
  uint8_t inverted;               // Invert display
  uint8_t day_invert;             // Invert colors on today's date
  uint8_t grid;                   // Show the grid
  uint8_t vibe_hour;              // vibrate at the top of the hour?
  uint8_t dayOfWeekOffset;        // first day of our week
  uint8_t date_format;            // date format
  uint8_t show_am_pm;             // Show AM/PM below time
  uint8_t show_day;               // Show day name below time
  uint8_t show_week;              // Show week number below time
  uint8_t week_format;            // week format (calculation, e.g. ISO 8601)
  uint8_t vibe_pat_disconnect;    // vibration pattern for disconnect
  uint8_t vibe_pat_connect;       // vibration pattern for connect
  char strftime_format[32];       // custom date_format string (date_format = 255)
  uint8_t track_battery;          // track battery information
} __attribute__((__packed__)) persist;

typedef struct persist_months_lang { // 252 bytes
  char monthsNames[12][21];       // 252: 10-20 UTF8 characters for each of 12 months
} __attribute__((__packed__)) persist_months_lang;

typedef struct persist_days_lang { // 238 bytes
  char DaysOfWeek[7][34];         // 238: 16-33 UTF8 characters for each of 7 weekdays
} __attribute__((__packed__)) persist_days_lang;

typedef struct persist_general_lang { // 253 bytes
  char statuses[2][26];           //  40: 12-25 characters for each of  2 statuses
  char abbrTime[2][12];           //  24:  5-11 characters for each of  2 abbreviations
  char abbrDaysOfWeek[7][6];      //  42:  2- 5 characters for each of  7 weekdays abbreviations
  char abbrMonthsNames[12][11];   // 132:  5-11 characters for each of 12 months abbreviations
  char language[3];               //   3:  2 characters for language
} __attribute__((__packed__)) persist_general_lang;

typedef struct persist_debug { // 6 bytes
  bool general;
  bool language;
  bool reserved_1;
  bool reserved_2;
  bool reserved_3;
  bool reserved_4;
} __attribute__((__packed__)) persist_debug;

typedef struct persist_adv_settings { // 243 bytes
  uint8_t week_pattern;
  uint8_t invertStatBar;
  uint8_t invertTopSlot;
  uint8_t invertBotSlot;
  uint8_t showStatus;
  uint8_t showStatusBat;
  uint8_t showDate;
  uint8_t DND_start;
  uint8_t DND_stop;
  uint8_t DND_accel_off;
  uint8_t vibe_hour_start;
  uint8_t vibe_hour_stop;
  uint8_t vibe_hour_days;
  uint8_t idle_reminder;
  uint8_t idle_pattern;
  char idle_message[32];
  uint8_t idle_start;
  uint8_t idle_stop;
  int8_t clock2_tz;
  char clock2_desc[32];
  uint8_t weather_format;
  uint8_t weather_update;
  char weather_lat[8];
  char weather_lon[8];
  uint8_t clock_font;
  uint8_t token_type[2];
  char token_code[2][65];
  uint8_t slots[10];
} __attribute__((__packed__)) persist_adv_settings;

typedef struct weather_data {
  int16_t current;
  char condition[2];
  int16_t requests;
  int16_t failures;
} __attribute__((__packed__)) weather_data;

// Public Function Prototypes
int daysInMonth(int mon, int year);
struct tm *get_time();
void setColors(GContext* ctx);
void setInvColors(GContext* ctx);
void weather_layer_update_callback(Layer *me, GContext* ctx);
void splash_layer_update_callback(Layer *me, GContext* ctx);
void calendar_layer_update_callback(Layer *me, GContext* ctx);
void update_date_text();
void update_time_text();
void update_day_text(TextLayer *which_layer);
void update_month_text(TextLayer *which_layer);
void update_week_text(TextLayer *which_layer);
void update_ampm_text(TextLayer *which_layer);
void update_seconds_text(TextLayer *which_layer);
char *get_doy_text();
char *get_dliy_text();
void update_doy_text(TextLayer *which_layer);
void update_dliy_text(TextLayer *which_layer);
void update_doy_dliy_text(TextLayer *which_layer);
void update_timezone_text(TextLayer *which_layer);
void process_show_week();
void process_show_day();
void process_show_ampm();
void position_connection_layer();
void position_date_layer();
void position_day_layer();
void position_time_layer();
void update_datetime_subtext();
void datetime_layer_update_callback(Layer *me, GContext* ctx);
void statusbar_visible();
void toggle_weather();
void toggle_statusbar();
void battery_layer_update_callback(Layer *me, GContext* ctx);
void set_status_charging_icon();
void generate_vibe(uint32_t vibe_pattern_number);
void update_connection();
bool period_check(uint8_t start_incr, uint8_t stop_incr, bool retval_on_equal);
bool dnd_period_check();
bool hourvibe_period_check();
void set_layer_attr(TextLayer *textlayer, GTextAlignment Alignment);
void set_layer_attr_sfont(TextLayer *textlayer, char *font_key, GTextAlignment Alignment);
void handle_vibe_suppression();
void my_out_sent_handler(DictionaryIterator *sent, void *context);
void my_out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
void in_js_ready_handler(DictionaryIterator *received, void *context);
void in_weather_handler(DictionaryIterator *received, void *context);
void in_timezone_handler(DictionaryIterator *received, void *context);
void in_configuration_handler(DictionaryIterator *received, void *context);
void my_in_rcv_handler(DictionaryIterator *received, void *context);
void my_in_drp_handler(AppMessageResult reason, void *context);
int main(void);