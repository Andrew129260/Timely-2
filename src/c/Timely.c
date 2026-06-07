#include <pebble.h>
#include "Timely.h"

#define DEBUGLOG 0
#define TRANSLOG 0
#define CONFIG_VERSION "3.5.5"

static Window *window;

static Layer *battery_layer;
static Layer *datetime_layer;
static TextLayer *date_layer;
static TextLayer *time_layer;
static TextLayer *week_layer;
static TextLayer *ampm_layer;
static TextLayer *day_layer;
static Layer *calendar_layer;
static Layer *splash_layer;
static Layer *weather_layer;
static Layer *statusbar;
static Layer *slot_status;
static Layer *slot_top;
static Layer *slot_bot;
static GFont unifont_16;
static GFont unifont_16_bold;
static GFont cal_normal;
static GFont cal_bold;
static GFont climacons;

static BitmapLayer *bmp_connection_layer;
static GBitmap *image_connection_icon;
static GBitmap *image_noconnection_icon;
static BitmapLayer *bmp_charging_layer;
static GBitmap *image_charging_icon;
static GBitmap *image_hourvibe_icon;
static GBitmap *image_dnd_icon;
static TextLayer *text_connection_layer;
static TextLayer *text_phone_battery_layer;

// battery info, instantiate to 'worst scenario' to prevent false hopes
static uint8_t battery_percent = 10;
static bool battery_charging = false;
static bool battery_plugged = false;
AppTimer *battery_sending = NULL;
AppTimer *timezone_request = NULL;
AppTimer *weather_request = NULL;
AppTimer *bottom_toggle = NULL;
// connected info
static bool bluetooth_connected = false;
// suppress vibration
static bool vibe_suppression = true;
#define TIMEZONE_UNINITIALIZED 80
static int8_t timezone_offset = TIMEZONE_UNINITIALIZED;
struct tm *currentTime;
static int8_t seconds_shown = 0;
static bool dnd_period_active = false;
static bool vibe_period_active = false;
static bool showing_statusbar = true;
static int device_width = 144;
static int device_height = 168;
static int layout_slot_height = 72;
static int stat_batt_left = 96;
// Layout values scaled from the 144x168 Aplite baseline to the actual device size
static int s_slot_top_height = 24;
static int s_batt_top        = 4;
static int s_batt_height     = 15;
static int s_batt_nib_height = 5;
static int s_batt_width      = 44;

// define the persistent storage key(s)
#define PK_SETTINGS      0
#define PK_LANG_GEN      1 
#define PK_LANG_DATETIME 2 
#define PK_LANG_MONTHS   3
#define PK_LANG_DAYS     4
#define PK_DEBUGGING     5
#define PK_ADV_SETTINGS  6
#define PK_PHONE_BATT    7

// Message Type Values (Payloads) matching PebbleKit JS
#define MSG_VAL_SEND_BATT_PERCENT    100
#define MSG_VAL_TIMEZONE_OFFSET      103
#define MSG_VAL_SEND_WATCH_VERSION   104
#define MSG_VAL_REQUEST_WEATHER      106

// primary coordinates
#define LAYOUT_STAT           0 
#define LAYOUT_SLOT_TOP      24 
#define STAT_BATT_TOP         4
#define STAT_BATT_WIDTH      44 
#define STAT_BATT_HEIGHT     15
#define STAT_BATT_NIB_WIDTH   3 
#define STAT_BATT_NIB_HEIGHT  5 
#define STAT_BT_ICON_LEFT    -2 
#define STAT_BT_ICON_TOP      2
#define STAT_CHRG_ICON_LEFT  76
#define STAT_CHRG_ICON_TOP    2

// relative coordinates (relative to SLOTs)
#define REL_CLOCK_DATE_LEFT       2
#define REL_CLOCK_DATE_TOP        0
#define REL_CLOCK_DATE_HEIGHT    30 
#define REL_CLOCK_TIME_LEFT       0
#define REL_CLOCK_TIME_TOP        7
#define REL_CLOCK_TIME_HEIGHT    60 
#define REL_CLOCK_SUBTEXT_TOP    56 
// Proportional scaling helpers
#define SY(n) ((n) * device_height / 168)
#define SX(n) ((n) * device_width  / 144)

#define SLOT_ID_CLOCK_1  0
#define SLOT_ID_CALENDAR 1
#define SLOT_ID_WEATHER  2
#define SLOT_ID_CLOCK_2  3

weather_data weather = {
  .current    = 999,
  .condition = {'h'},
  .requests = 0,
  .failures = 0,
};

persist settings = {
  .version    = 13,
  .inverted   = 1, 
  .day_invert = 1, 
  .grid       = 1, 
  .vibe_hour  = 0, 
  .dayOfWeekOffset = 0, 
  .date_format = 0, 
  .show_am_pm  = 0, 
  .show_day    = 0, 
  .show_week   = 0, 
  .week_format = 0, 
  .vibe_pat_disconnect = 2, 
  .vibe_pat_connect = 0, 
  .strftime_format = "%Y-%m-%d",
  .track_battery = 1, 
};

persist_months_lang lang_months = {
  .monthsNames = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" },
};

persist_days_lang lang_days = {
  .DaysOfWeek = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" },
};

persist_general_lang lang_gen = {
  .statuses = { "Linked", "NOLINK" },
  .abbrTime = { "AM", "PM" },
  .abbrDaysOfWeek = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" },
  .abbrMonthsNames = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" },
  .language = "EN",
};

persist_debug debug = {
  .general = false,     
  .language  = false,   
  .reserved_1 = false,  
  .reserved_2 = false,  
  .reserved_3 = false,  
  .reserved_4 = false,  
};

persist_adv_settings adv_settings = {
  .week_pattern = 0,    
  .invertStatBar = 0,   
  .invertTopSlot = 0,   
  .invertBotSlot = 0,   
  .showStatus = 1,      
  .showStatusBat = 100, 
  .showDate = 1,        
  .DND_start = 0,       
  .DND_stop  = 0,       
  .DND_accel_off = 0,   
  .vibe_hour_start = 0, 
  .vibe_hour_stop  = 0, 
  .vibe_hour_days  = 0, 
  .idle_reminder = 0,   
  .idle_pattern = 0,    
  .idle_message = { "Let's Move!" }, 
  .idle_start = 0,      
  .idle_stop  = 0,      
  .clock2_tz = 0,       
  .clock2_desc = { "Second Clock" }, 
  .weather_format = 0,  
  .weather_update = 15, 
  .weather_lat = "",    
  .weather_lon = "",    
  .clock_font = 1,      
  .token_type = { 0, 0 },   
  .token_code = { "", "" }, 
  .slots = { 0, 1, 2, 3, 0, 1, 0, 1, 0, 1 } 
};

// Safe generic integer parser for Clay variables
int32_t get_int(Tuple *t) {
  if (!t) return 0;
  if (t->type == TUPLE_UINT) {
    if (t->length == 1) return t->value->uint8;
    if (t->length == 2) return t->value->uint16;
    if (t->length == 4) return t->value->uint32;
  } else if (t->type == TUPLE_INT) {
    if (t->length == 1) return t->value->int8;
    if (t->length == 2) return t->value->int16;
    if (t->length == 4) return t->value->int32;
  }
  return 0;
}

// How many days are/were in the month
int daysInMonth(int mon, int year) {
    mon++; 
    if (mon == 4 || mon == 6 || mon == 9 || mon == 11) { return 30; }
    else if (mon == 2) {
        if (year % 400 == 0) return 29;
        else if (year % 100 == 0) return 28;
        else if (year % 4 == 0) return 29;
        else return 28;
    } else {
        return 31;
    }
}

struct tm *get_time() {
    time_t tt = time(0);
    return localtime(&tt);
}

void setColors(GContext* ctx) {
    window_set_background_color(window, settings.inverted ? GColorWhite : GColorBlack);
    graphics_context_set_stroke_color(ctx, settings.inverted ? GColorBlack : GColorWhite);
    graphics_context_set_fill_color(ctx, settings.inverted ? GColorWhite : GColorBlack);
    graphics_context_set_text_color(ctx, settings.inverted ? GColorBlack : GColorWhite);
}

void setInvColors(GContext* ctx) {
    window_set_background_color(window, settings.inverted ? GColorBlack : GColorWhite);
    graphics_context_set_stroke_color(ctx, settings.inverted ? GColorWhite : GColorBlack);
    graphics_context_set_fill_color(ctx, settings.inverted ? GColorBlack : GColorWhite);
    graphics_context_set_text_color(ctx, settings.inverted ? GColorWhite : GColorBlack);
}

void weather_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me; 
  static char temp_current[10] = "N/A  ";
  static char cond_current[] = "0";
  if (weather.current < 900) {
    snprintf(temp_current, sizeof(temp_current), "%d\u00b0", weather.current);
  } else {
    snprintf(temp_current, sizeof(temp_current), "N/A");
  }
  snprintf(cond_current, sizeof(cond_current), "%s", weather.condition);

  setColors(ctx);
  graphics_draw_text(ctx, cond_current, climacons, GRect(2, SY(16), SY(34), SY(34)), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, temp_current, fonts_get_system_font(device_height > 168 ? FONT_KEY_GOTHIC_28 : FONT_KEY_GOTHIC_24), GRect(2, SY(42), SY(36), SY(36)), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void splash_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me; 
    setColors(ctx);
    graphics_draw_text(ctx, "Timely 2", fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(0, 0, device_width, SY(36)), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, CONFIG_VERSION, fonts_get_system_font(FONT_KEY_GOTHIC_28), GRect(0, SY(32), device_width, SY(36)), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}
void calendar_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;

    int mon = currentTime->tm_mon;
    int year = currentTime->tm_year + 1900;
    int daysThisMonth = daysInMonth(mon, year);
    int specialDay = currentTime->tm_wday - settings.dayOfWeekOffset; 
    
    int show_last = 1; 
    int show_next = 1; 
    switch ( adv_settings.week_pattern ) {
      case 0: break;
      case 1: show_last = 2; show_next = 0; break;
      case 2: show_last = 0; show_next = 2; break;
    }

    int calendar[21];
    int cellNum = 0;   
    int daysVisPrevMonth = 0;
    int daysVisNextMonth = 0;
    int daysPriorToToday = specialDay; 
    int daysAfterToday   = (6 - specialDay) % 7; 

    if (currentTime->tm_wday < settings.dayOfWeekOffset) { 
        daysPriorToToday += 7 * (show_last+1); 
        specialDay += 7;
    } else {
      daysPriorToToday += 7 * show_last; 
    }
    daysAfterToday += 7 * show_next; 

    if ( daysPriorToToday >= currentTime->tm_mday ) {
      int daysInPrevMonth = daysInMonth(mon - 1,year); 
      daysVisPrevMonth = daysPriorToToday - currentTime->tm_mday + 1;
      for (int i = 0; i < daysVisPrevMonth; i++, cellNum++ ) {
        calendar[cellNum] = daysInPrevMonth + i - daysVisPrevMonth + 1;
      }
    }

    int firstDayShownThisMonth = daysVisPrevMonth + currentTime->tm_mday - daysPriorToToday;
    for (int i = firstDayShownThisMonth; i < currentTime->tm_mday; i++, cellNum++ ) {
      calendar[cellNum] = i;
    }

    calendar[cellNum] = currentTime->tm_mday;
    cellNum++;

    if ( currentTime->tm_mday + daysAfterToday > daysThisMonth ) {
      daysVisNextMonth = currentTime->tm_mday + daysAfterToday - daysThisMonth;
    }

    int daysLeftThisMonth = daysAfterToday - daysVisNextMonth;
    for (int i = 0; i < daysLeftThisMonth; i++, cellNum++ ) {
      calendar[cellNum] = i + currentTime->tm_mday + 1;
    }

    for (int i = 0; i < daysVisNextMonth; i++, cellNum++ ) {
      calendar[cellNum] = i + 1;
    }

    #define CAL_DAYS   7   
    #define CAL_GAP    1   
    #define CAL_LEFT   2   
    int cal_height = SY(18);  
    int cal_width = (device_width - 2 * CAL_LEFT) / CAL_DAYS;

    int weeks  =  3;  
        
    GFont current = cal_normal;
    int font_vert_offset = 0;
    if (strcmp(lang_gen.language,"RU") == 0 ) { font_vert_offset = -2; }

    if (settings.grid) {
      setInvColors(ctx);
      graphics_fill_rect(ctx, GRect (CAL_LEFT + CAL_GAP, cal_height - CAL_GAP, device_width - 2 * (CAL_LEFT + CAL_GAP), cal_height * weeks), 0, GCornerNone);
      setColors(ctx);
    }
    for (int col = 0; col < CAL_DAYS; col++) {
      int weekday = col + settings.dayOfWeekOffset;
      if (weekday > 6) { weekday -= 7; }

      if (col == specialDay) {
        current = cal_bold;
        font_vert_offset = -3;
        if (strcmp(lang_gen.language,"RU") == 0 ) { font_vert_offset = -2; }
      }

      graphics_draw_text(ctx, lang_gen.abbrDaysOfWeek[weekday], current, GRect(cal_width * col + CAL_LEFT + CAL_GAP, CAL_GAP + font_vert_offset, cal_width, cal_height), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      if (col == specialDay) {
        if (strcmp(lang_gen.language,"RU") == 0 ) {
          graphics_draw_text(ctx, lang_gen.abbrDaysOfWeek[weekday], current, GRect(cal_width * col + CAL_LEFT + CAL_GAP + 1, CAL_GAP + font_vert_offset, cal_width, cal_height), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
        }
        current = cal_normal;
        font_vert_offset = 0;
        if (strcmp(lang_gen.language,"RU") == 0 ) { font_vert_offset = -2; }
      }
    }

    GFont normal = fonts_get_system_font(device_height > 168 ? FONT_KEY_GOTHIC_18 : FONT_KEY_GOTHIC_14);
    GFont bold   = fonts_get_system_font(device_height > 168 ? FONT_KEY_GOTHIC_24_BOLD : FONT_KEY_GOTHIC_18_BOLD);
    current = normal;
    font_vert_offset = 0;

    int week = 0;
    int specialRow = show_last+1;
    
    for (int row = 1; row <= 3; row++) {
      week++;
      for (int col = 0; col < CAL_DAYS; col++) {
        if ( row == specialRow && col == specialDay) {
          if (settings.day_invert) {
            setInvColors(ctx);
          }
          current = bold;
          font_vert_offset = -3;
        }

        graphics_fill_rect(ctx, GRect (cal_width * col + CAL_LEFT + CAL_GAP, cal_height * week, cal_width - CAL_GAP, cal_height - CAL_GAP), 0, GCornerNone);

        char date_text[3];
        snprintf(date_text, sizeof(date_text), "%d", calendar[col + 7 * (row - 1)]);
        graphics_draw_text(ctx, date_text, current, GRect(cal_width * col + CAL_LEFT, cal_height * week - CAL_GAP + font_vert_offset, cal_width, cal_height), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        if ( row == specialRow && col == specialDay) {
          setColors(ctx);
          current = normal;
          font_vert_offset = 0;
        }
      }
    }
}

void update_date_text() {
    const char *datestr[] = {
      "%m.%d.%Y", "%m-%d-%Y", "%m/%d/%Y", "%m %d %Y", "%m%d%Y", 
      "%m.%d.%y", "%m-%d-%y", "%m/%d/%y", "%m %d %y", "%m%d%y", 
      "%m.%e.%Y", "%m-%e-%Y", "%m/%e/%Y", "%m %e %Y", "%m%e%Y", 
      "%m.%e.%y", "%m-%e-%y", "%m/%e/%y", "%m %e %y", "%m%e%y", 
      "%d.%m.%Y", "%d-%m-%Y", "%d/%m/%Y", "%d %m %Y", "%d%m%Y", 
      "%d.%m.%y", "%d-%m-%y", "%d/%m/%y", "%d %m %y", "%d%m%y", 
      "%e.%m.%Y", "%e-%m-%Y", "%e/%m/%Y", "%e %m %Y", "%e%m%Y", 
      "%e.%m.%y", "%e-%m-%y", "%e/%m/%y", "%e %m %y", "%e%m%y", 
      "%Y.%m.%d", "%Y-%m-%d", "%Y/%m/%d", "%Y %m %d", "%Y%m%d", 
      "%y.%m.%d", "%y-%m-%d", "%y/%m/%d", "%y %m %d", "%y%m%d", 
      "%Y.%m.%e", "%Y-%m-%e", "%Y/%m/%e", "%Y %m %e", "%Y%m%e", 
      "%y.%m.%e", "%y-%m-%e", "%y/%m/%e", "%y %m %e", "%y%m%e", 
    };
    char date_text[24];
    static char date_string[64];

    if (settings.date_format < 195) { 
      char date_text_2[24];
      switch ( settings.date_format ) {
      case 0: 
        strftime(date_text, sizeof(date_text), "%d, %Y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s", lang_months.monthsNames[currentTime->tm_mon], date_text); 
        break;
      case 1: 
        strftime(date_text, sizeof(date_text), "%d, '%y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s", lang_months.monthsNames[currentTime->tm_mon], date_text); 
        break;
      case 2: 
        strftime(date_text, sizeof(date_text), "%d, %Y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s", lang_gen.abbrMonthsNames[currentTime->tm_mon], date_text); 
        break;
      case 3: 
        strftime(date_text, sizeof(date_text), "%d, '%y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s", lang_gen.abbrMonthsNames[currentTime->tm_mon], date_text); 
        break;
      case 11: 
        strftime(date_text, sizeof(date_text), "%d", currentTime); 
        strftime(date_text_2, sizeof(date_text_2), "%Y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s %s", date_text, lang_months.monthsNames[currentTime->tm_mon], date_text_2); 
        break;
      case 12: 
        strftime(date_text, sizeof(date_text), "%d", currentTime); 
        strftime(date_text_2, sizeof(date_text_2), "'%y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s %s", date_text, lang_months.monthsNames[currentTime->tm_mon], date_text_2); 
        break;
      case 13: 
        strftime(date_text, sizeof(date_text), "%d", currentTime); 
        strftime(date_text_2, sizeof(date_text_2), "%Y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s %s", date_text, lang_gen.abbrMonthsNames[currentTime->tm_mon], date_text_2); 
        break;
      case 14: 
        strftime(date_text, sizeof(date_text), "%d", currentTime); 
        strftime(date_text_2, sizeof(date_text_2), "'%y", currentTime); 
        snprintf(date_string, sizeof(date_string), "%s %s %s", date_text, lang_gen.abbrMonthsNames[currentTime->tm_mon], date_text_2); 
        break;
      }
    } else { 
      if ((settings.date_format >= 195) && (settings.date_format <= 254)) { 
        strftime(date_text, sizeof(date_text), datestr[settings.date_format-195], currentTime);
      } else if (settings.date_format==255) {
        strftime(date_text, sizeof(date_text), settings.strftime_format, currentTime);  
      }
      snprintf(date_string, sizeof(date_string), "%s", date_text); 
    }
    text_layer_set_text(date_layer, date_string);
}

void update_time_text() {
  static char time_text[] = "00:00";
  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, currentTime);

  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  text_layer_set_text(time_layer, time_text);
}

void update_day_text(TextLayer *which_layer) {
  text_layer_set_text(which_layer, lang_days.DaysOfWeek[currentTime->tm_wday]);
}

void update_month_text(TextLayer *which_layer) {
  text_layer_set_text(which_layer, lang_months.monthsNames[currentTime->tm_mon]);
}

void update_week_text(TextLayer *which_layer) {
  static char week_text[] = "W00";
  char week_format[] = "W%V"; 
  if (settings.week_format == 1) {
    week_format[2] = 'U';
  } else if (settings.week_format == 2) {
    week_format[2] = 'W';
  }
  strftime(week_text, sizeof(week_text), week_format, currentTime);
  text_layer_set_text(which_layer, week_text);
}

void update_ampm_text(TextLayer *which_layer) {
  if (currentTime->tm_hour < 12 ) {
    text_layer_set_text(which_layer, lang_gen.abbrTime[0]); 
  } else {
    text_layer_set_text(which_layer, lang_gen.abbrTime[1]); 
  }
}

void update_seconds_text(TextLayer *which_layer) {
  static char seconds_text[] = "00"; 
  strftime(seconds_text, sizeof(seconds_text), "%S", currentTime);
  text_layer_set_text(which_layer, seconds_text);
}

char * get_doy_text() {
  static char doy_text[] = "D000";
  strftime(doy_text, sizeof(doy_text), "D%j", currentTime);
  return doy_text;
}

char * get_dliy_text() {
  static char dliy_text[14] = "R000";
  int daysThisFeb = daysInMonth(1, currentTime->tm_year + 1900);
  int daysThisYear = 365;
  if (daysThisFeb == 29) { daysThisYear = 366; }
  int daysSinceJanFirst = currentTime->tm_yday; 
  int daysLeftThisYear = daysThisYear - daysSinceJanFirst - 1;
  snprintf(dliy_text, sizeof(dliy_text), "R%03d", daysLeftThisYear);
  return dliy_text;
}

void update_doy_text(TextLayer *which_layer) {
  text_layer_set_text(which_layer, get_doy_text());
}

void update_dliy_text(TextLayer *which_layer) {
  text_layer_set_text(which_layer, get_dliy_text());
}

void update_doy_dliy_text(TextLayer *which_layer) {
  static char doy_dliy_text[] = "D000/R000";
  snprintf(doy_dliy_text, sizeof(doy_dliy_text), "%s/%s", get_doy_text(), get_dliy_text());
  text_layer_set_text(which_layer, doy_dliy_text);
}

void update_timezone_text(TextLayer *which_layer) {
  static char timezone_text[12];
  int tz_hours = 0;
  int tz_mins  = 0;
  tz_mins  = timezone_offset % 4;
  tz_hours = (timezone_offset - tz_mins)/ 4;
  tz_mins  = tz_mins * 15;
  if (timezone_offset == TIMEZONE_UNINITIALIZED) {
    snprintf(timezone_text, sizeof(timezone_text), "UTC ?");
  } else if (timezone_offset > 0) {
    if (tz_mins == 0) {
      snprintf(timezone_text, sizeof(timezone_text), "UTC-%d", tz_hours);
    } else {
      snprintf(timezone_text, sizeof(timezone_text), "UTC-%d:%d", tz_hours, tz_mins);
    }
  } else {
    if (tz_mins == 0) {
      snprintf(timezone_text, sizeof(timezone_text), "UTC+%d", abs(tz_hours));
    } else {
      snprintf(timezone_text, sizeof(timezone_text), "UTC+%d:%d", abs(tz_hours), abs(tz_mins));
    }
  }
  text_layer_set_text(which_layer, timezone_text);
}
void process_show_week() {
  switch ( settings.show_week ) {
  case 0: return;
  case 1: update_week_text(week_layer); break;
  case 2: update_timezone_text(week_layer); break;
  case 3: update_ampm_text(week_layer); break;
  case 4: update_doy_text(week_layer); break;
  case 5: update_dliy_text(week_layer); break;
  case 6: update_seconds_text(week_layer); break;
  }
}

void process_show_day() {
  switch ( settings.show_day ) {
  case 0: return;
  case 1: update_day_text(day_layer); break;
  case 2: update_month_text(day_layer); break;
  case 3: update_timezone_text(day_layer); break;
  case 4: update_week_text(day_layer); break;
  case 5: update_ampm_text(day_layer); break;
  case 6: update_doy_dliy_text(day_layer); break;
  }
}

void process_show_ampm() {
  switch ( settings.show_am_pm ) {
  case 0: return;
  case 1: update_ampm_text(ampm_layer); break;
  case 2: update_timezone_text(ampm_layer); break;
  case 3: update_week_text(ampm_layer); break;
  case 4: update_doy_text(ampm_layer); break;
  case 5: update_dliy_text(ampm_layer); break;
  case 6: update_seconds_text(ampm_layer); break;
  }
}

void position_connection_layer() {
  int connection_vert_offset = 0;
  if ( strcmp(lang_gen.language,"RU") == 0 ) {
    connection_vert_offset = SY(2);
  }
  layer_set_frame( text_layer_get_layer(text_connection_layer), GRect(20+STAT_BT_ICON_LEFT, connection_vert_offset, stat_batt_left - (20 + STAT_BT_ICON_LEFT), SY(22)) );
}

void position_date_layer() {
  int date_vert_offset = 0;
  if ( strcmp(lang_gen.language,"RU") == 0 ) {
    date_vert_offset = showing_statusbar ? -4 : 0;
  } else {
    date_vert_offset = showing_statusbar ? -9 : -5;
  }
  layer_set_frame( text_layer_get_layer(date_layer), GRect(REL_CLOCK_DATE_LEFT, REL_CLOCK_DATE_TOP + date_vert_offset, device_width - 4, SY(REL_CLOCK_DATE_HEIGHT)) );
}

void position_day_layer() {
  int day_vert_offset = (strcmp(lang_gen.language,"RU") == 0) ? SY(-2) : 0;
  layer_set_frame( text_layer_get_layer(day_layer), GRect(REL_CLOCK_DATE_LEFT, SY(REL_CLOCK_SUBTEXT_TOP) + day_vert_offset, device_width - 4, SY(REL_CLOCK_DATE_HEIGHT)) );
}

void position_time_layer() {
  int time_offset, weather_offset;
  if (!settings.show_day && !settings.show_week && !settings.show_am_pm) {
    time_offset    = SY(12);
    weather_offset = 0;
  } else {
    time_offset    = SY(2);
    weather_offset = SY(-10);
  }
  layer_set_frame( text_layer_get_layer(time_layer), GRect(REL_CLOCK_TIME_LEFT, SY(REL_CLOCK_TIME_TOP) + time_offset, device_width, SY(REL_CLOCK_TIME_HEIGHT)) );
  layer_set_frame( weather_layer, GRect(REL_CLOCK_TIME_LEFT, weather_offset, device_width, layout_slot_height) );
}

void update_datetime_subtext() {
    process_show_week();
    process_show_day();
    process_show_ampm();
    position_time_layer();
}

void datetime_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;
    setColors(ctx);
    update_date_text();
    update_time_text();
    update_datetime_subtext();
}

void statusbar_visible() {
  if (adv_settings.showStatus == 0) {
    showing_statusbar = false;
  } else if (adv_settings.showStatus == 1) {
    showing_statusbar = true;
  } else if (battery_percent <= adv_settings.showStatusBat) {
    showing_statusbar = true;
  } else {
    showing_statusbar = false;
  }
}

void toggle_weather() {
  if (adv_settings.weather_update) {
    text_layer_set_text_alignment(time_layer, GTextAlignmentRight);
    layer_set_hidden(weather_layer, false);
  } else {
    layer_set_hidden(weather_layer, true);
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  }
}

// Helper function to safely isolate the preprocessor logic
void update_battery_text_size(bool is_large) {
#if defined(PBL_PLATFORM_EMERY)
  if (is_large) {
    layer_set_frame(text_layer_get_layer(text_phone_battery_layer), GRect(4, s_batt_top - 6, 80, s_batt_height + 10));
    text_layer_set_font(text_phone_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  } else {
    layer_set_frame(text_layer_get_layer(text_phone_battery_layer), GRect(4, s_batt_top - 3, 64, s_batt_height + 4));
    text_layer_set_font(text_phone_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  }
#endif
}

void toggle_statusbar() {
  if (showing_statusbar) {
    layer_set_hidden(statusbar, false);
    layer_add_child(datetime_layer, text_layer_get_layer(date_layer));
    
    // IF SUBTEXT IS ON: Date shifts Right, leaving huge room for Phone Battery
    if (adv_settings.weather_update && (settings.show_day || settings.show_week || settings.show_am_pm)) {
      text_layer_set_text_alignment(date_layer, GTextAlignmentRight);
      update_battery_text_size(true); // Call helper to resize UP
    } else {
      text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
      update_battery_text_size(false); // Call helper to resize DOWN
    }
    
    layer_add_child(statusbar, bitmap_layer_get_layer(bmp_charging_layer));
    layer_add_child(statusbar, battery_layer);
    
  } else {
    layer_set_hidden(statusbar, true);
    layer_add_child(slot_status, text_layer_get_layer(date_layer));
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
    update_battery_text_size(false); // Default to smaller size
    
    layer_add_child(datetime_layer, bitmap_layer_get_layer(bmp_charging_layer));
    layer_add_child(datetime_layer, battery_layer);
  }
  position_date_layer();
}

void slot_status_layer_update_callback(Layer *me, GContext* ctx) {}
void statusbar_layer_update_callback(Layer *me, GContext* ctx) {}
void slot_top_layer_update_callback(Layer *me, GContext* ctx) {}
void slot_bot_layer_update_callback(Layer *me, GContext* ctx) {}

void battery_layer_update_callback(Layer *me, GContext* ctx) {
  setColors(ctx);
  
  // 1. Draw Outline
  graphics_draw_rect(ctx, GRect(stat_batt_left, s_batt_top, s_batt_width, s_batt_height));
  graphics_draw_rect(ctx, GRect(stat_batt_left + s_batt_width - 1,
                                s_batt_top + (s_batt_height - s_batt_nib_height)/2,
                                STAT_BATT_NIB_WIDTH,
                                s_batt_nib_height));

  // Calculate the fill width dynamically
  uint8_t battery_meter = battery_percent * (s_batt_width - 4) / 100;

  // 2. Define the exact dynamic colors based on the user's Dark/Light theme
  GColor fill_color = settings.inverted ? GColorBlack : GColorWhite;
  GColor empty_color = settings.inverted ? GColorWhite : GColorBlack;

  // 3. Draw the solid background fill
  graphics_context_set_fill_color(ctx, fill_color);
  graphics_fill_rect(ctx, GRect(stat_batt_left + 2, s_batt_top + 2, battery_meter, s_batt_height - 4), 0, GCornerNone);

  // 4. Prepare the text payload
  char batt_str[5];
  snprintf(batt_str, sizeof(batt_str), "%d", battery_percent);
  GFont batt_font = fonts_get_system_font(device_height > 168 ? FONT_KEY_GOTHIC_18_BOLD : FONT_KEY_GOTHIC_14);
  
  // Determine base text bounds
  int text_y_offset = (device_height > 168) ? 3 : 2;
  GRect tb = GRect(stat_batt_left, s_batt_top - text_y_offset, s_batt_width, s_batt_height + 4);

  // Determine colors so the text contrasts with the MAJORITY of the battery bar
  GColor text_color = (battery_percent >= 50) ? empty_color : fill_color;
  GColor outline_color = (battery_percent >= 50) ? fill_color : empty_color;

  // 5. Draw the 1-pixel outline (Left, Right, Up, Down) for perfect contrast
  graphics_context_set_text_color(ctx, outline_color);
  graphics_draw_text(ctx, batt_str, batt_font, GRect(tb.origin.x - 1, tb.origin.y, tb.size.w, tb.size.h), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, batt_str, batt_font, GRect(tb.origin.x + 1, tb.origin.y, tb.size.w, tb.size.h), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, batt_str, batt_font, GRect(tb.origin.x, tb.origin.y - 1, tb.size.w, tb.size.h), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, batt_str, batt_font, GRect(tb.origin.x, tb.origin.y + 1, tb.size.w, tb.size.h), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // 6. Draw the main centered text over the outline
  graphics_context_set_text_color(ctx, text_color);
  graphics_draw_text(ctx, batt_str, batt_font, tb, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void request_weather(void *data) {
  // CACHING FIX: We no longer reset the condition to 'h' (loading) here. 
  // If the request fails, the old weather icon and temp stay gracefully on screen.
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) return;

  if (dict_write_uint8(iter, MESSAGE_KEY_message_type, MSG_VAL_REQUEST_WEATHER) != DICT_OK) return;
  if (dict_write_uint8(iter, MESSAGE_KEY_weather_fmt, adv_settings.weather_format) != DICT_OK) return;
  
  app_message_outbox_send();
  weather.requests++;
  weather_request = NULL;
}

static void request_timezone(void *data) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) return;
  
  if (dict_write_uint8(iter, MESSAGE_KEY_message_type, MSG_VAL_TIMEZONE_OFFSET) != DICT_OK) return;
  
  app_message_outbox_send();
  timezone_request = NULL;
}

static void watch_version_send(void *data) {
  DictionaryIterator *iter;
  AppMessageResult result = app_message_outbox_begin(&iter);

  if (iter == NULL || result != APP_MSG_OK) { return; }

  if (dict_write_uint8(iter, MESSAGE_KEY_message_type, MSG_VAL_SEND_WATCH_VERSION) != DICT_OK) return;
  if (dict_write_uint8(iter, MESSAGE_KEY_send_watch_version, settings.version) != DICT_OK) return;
  if (dict_write_cstring(iter, MESSAGE_KEY_send_config_version, CONFIG_VERSION) != DICT_OK) return;
  
  app_message_outbox_send();
}

static void battery_status_send(void *data) {
  static uint8_t sent_battery_percent = 10;
  static bool sent_battery_charging = false;
  static bool sent_battery_plugged = false;
  if (!settings.track_battery) return; 
  
  if ( (battery_percent  == sent_battery_percent  )
     & (battery_charging == sent_battery_charging )
     & (battery_plugged  == sent_battery_plugged  ) ) {
    battery_sending = NULL;
    return; 
  }
  DictionaryIterator *iter;
  AppMessageResult result = app_message_outbox_begin(&iter);

  if (iter == NULL || result != APP_MSG_OK) { return; }

  if (dict_write_uint8(iter, MESSAGE_KEY_message_type, MSG_VAL_SEND_BATT_PERCENT) != DICT_OK) return;
  if (dict_write_uint8(iter, MESSAGE_KEY_send_batt_percent, battery_percent) != DICT_OK) return;
  if (dict_write_uint8(iter, MESSAGE_KEY_send_batt_charging, battery_charging ? 1: 0) != DICT_OK) return;
  if (dict_write_uint8(iter, MESSAGE_KEY_send_batt_plugged, battery_plugged ? 1: 0) != DICT_OK) return;
  
  app_message_outbox_send();
  sent_battery_percent  = battery_percent;
  sent_battery_charging = battery_charging;
  sent_battery_plugged  = battery_plugged;
  battery_sending = NULL;
}

void set_status_charging_icon() {
  if (battery_charging) { 
    layer_set_hidden(bitmap_layer_get_layer(bmp_charging_layer), false);
    bitmap_layer_set_bitmap(bmp_charging_layer, image_charging_icon);
  } else { 
    if (dnd_period_active) {
      layer_set_hidden(bitmap_layer_get_layer(bmp_charging_layer), false);
      bitmap_layer_set_bitmap(bmp_charging_layer, image_dnd_icon);
    } else {
      if (battery_plugged) { 
        layer_set_hidden(bitmap_layer_get_layer(bmp_charging_layer), true);
      } else { 
        if (settings.vibe_hour && vibe_period_active) {
          layer_set_hidden(bitmap_layer_get_layer(bmp_charging_layer), false);
          bitmap_layer_set_bitmap(bmp_charging_layer, image_hourvibe_icon);
        } else {
          layer_set_hidden(bitmap_layer_get_layer(bmp_charging_layer), true);
        }
      }
    }
  }
}
static void toggle_slot_bottom(void *data) {
  watch_version_send(NULL); 
  static Layer* last = NULL;
  Layer* which = (Layer*)data;
  if (last != NULL) { layer_set_hidden(last, true); } 
  last = which; 
  if (last != NULL) { layer_set_hidden(last, false); } 
  bottom_toggle = NULL;
}

static void handle_battery(BatteryChargeState charge_state) {
  battery_percent = charge_state.charge_percent;
  battery_charging = charge_state.is_charging;
  battery_plugged = charge_state.is_plugged;

  if (battery_sending == NULL) {
    battery_sending = app_timer_register(5000, &battery_status_send, NULL);
  }

  set_status_charging_icon();


  layer_mark_dirty(battery_layer);
  statusbar_visible();
  toggle_statusbar();
  handle_vibe_suppression();
}

void generate_vibe(uint32_t vibe_pattern_number) {
  if (vibe_suppression) { return; }
  vibes_cancel();
  switch ( vibe_pattern_number ) {
  case 0: return;
  case 1: vibes_short_pulse(); break;
  case 2: vibes_double_pulse(); break;
  case 3: 
    vibes_enqueue_custom_pattern( (VibePattern) {
      .durations = (uint32_t []) {200, 100, 200, 100, 200},
      .num_segments = 5
    } );
    break;
  case 4: vibes_long_pulse(); break;
  case 5: 
    vibes_enqueue_custom_pattern( (VibePattern) {
      .durations = (uint32_t []) {50, 200, 50, 200, 50, 200, 50},
      .num_segments = 7
    } );
    break;
  case 6: 
    vibes_enqueue_custom_pattern( (VibePattern) {
      .durations = (uint32_t []) {100, 200, 100, 200, 100, 200, 100},
      .num_segments = 7
    } );
    break;
  case 7: 
    vibes_enqueue_custom_pattern( (VibePattern) {
      .durations = (uint32_t []) {500, 250, 500, 250, 500, 250, 500},
      .num_segments = 7
    } );
    break;
  default: return;
  }
}

void toggle_phone_battery_display() {
  if (settings.track_battery) {
    layer_set_hidden(text_layer_get_layer(text_phone_battery_layer), false);
    layer_set_hidden(text_layer_get_layer(text_connection_layer), true);
  } else {
    layer_set_hidden(text_layer_get_layer(text_phone_battery_layer), true);
    layer_set_hidden(text_layer_get_layer(text_connection_layer), false);
  }
}

void update_connection() {
  text_layer_set_text(text_connection_layer, bluetooth_connected ? lang_gen.statuses[0] : lang_gen.statuses[1]) ;
  if (bluetooth_connected) {
    generate_vibe(settings.vibe_pat_connect);
    bitmap_layer_set_bitmap(bmp_connection_layer, image_connection_icon);
  } else {
    generate_vibe(settings.vibe_pat_disconnect);
    bitmap_layer_set_bitmap(bmp_connection_layer, image_noconnection_icon);
  }
}

static void handle_bluetooth(bool connected) {
  if (bluetooth_connected != connected) {
    bluetooth_connected = connected;
    update_connection();
    if (bluetooth_connected == true) {
      if ( (timezone_request == NULL) & (timezone_offset == TIMEZONE_UNINITIALIZED) ) {
        timezone_request = app_timer_register(5000, &request_timezone, NULL); 
      }
    }
  }
}

static void set_unifont() {
  if ( strcmp(lang_gen.language,"RU") == 0 ) {
    text_layer_set_font(day_layer,unifont_16);
    text_layer_set_font(text_connection_layer, unifont_16);
    text_layer_set_font(date_layer, unifont_16);
    cal_normal = unifont_16; 
    cal_bold   = unifont_16_bold;
  } else if (device_height > 168) { 
    text_layer_set_font(day_layer,             fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_font(text_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_font(date_layer,            fonts_get_system_font(FONT_KEY_GOTHIC_28));
    cal_normal = fonts_get_system_font(FONT_KEY_GOTHIC_18);
    cal_bold   = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  } else { 
    text_layer_set_font(day_layer,             fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_font(text_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_font(date_layer,            fonts_get_system_font(FONT_KEY_GOTHIC_24));
    cal_normal = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    cal_bold   = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  }
  position_connection_layer();
  position_date_layer();
  position_time_layer();
  position_day_layer();
}

bool period_check(uint8_t start_incr, uint8_t stop_incr, bool retval_on_equal) {
  if (start_incr == stop_incr) { return retval_on_equal; }
  bool inside_period = false;
  uint8_t current_min_incr = (currentTime->tm_min - (currentTime->tm_min%10))/10;
  uint8_t current_incr = currentTime->tm_hour * 6 + current_min_incr;
  if (start_incr > stop_incr) { 
    if (current_incr >= start_incr || current_incr <= stop_incr) {
      inside_period = true;
    }
  } else { 
    if (current_incr >= start_incr && current_incr <= stop_incr) {
      inside_period = true;
    }
  }
  return inside_period;
}

bool dnd_period_check() {
  dnd_period_active = period_check(adv_settings.DND_start, adv_settings.DND_stop, false);
  return dnd_period_active;
}
bool hourvibe_period_check() {
  vibe_period_active = period_check(adv_settings.vibe_hour_start, adv_settings.vibe_hour_stop, true);
  return vibe_period_active;
}

void set_layer_attr(TextLayer *textlayer, GTextAlignment Alignment) {
  text_layer_set_text_alignment(textlayer, Alignment);
  text_layer_set_text_color(textlayer, settings.inverted ? GColorBlack : GColorWhite);
  text_layer_set_background_color(textlayer, GColorClear);
}

void set_layer_attr_sfont(TextLayer *textlayer, char *font_key, GTextAlignment Alignment) {
  set_layer_attr(textlayer, Alignment);
  text_layer_set_font(textlayer, fonts_get_system_font(font_key));
}

void set_layer_attr_cfont(TextLayer *textlayer, uint32_t FontResHandle, GTextAlignment Alignment) {
  set_layer_attr(textlayer, Alignment);
  text_layer_set_font(textlayer, fonts_load_custom_font(resource_get_handle(FontResHandle)));
}

static void window_load(Window *window) {

#if defined(PBL_PLATFORM_EMERY)
  unifont_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_CONDENSED_65));
  unifont_16_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_CONDENSED_65));
#else
  unifont_16 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  unifont_16_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
#endif

  climacons  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CLIMACONS_32));
  cal_normal = unifont_16;
  cal_bold   = unifont_16_bold;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  device_width       = bounds.size.w;
  device_height      = bounds.size.h;

  int ox = 0;

  s_slot_top_height  = LAYOUT_SLOT_TOP    * device_height / 168;
  s_batt_top         = STAT_BATT_TOP      * device_height / 168;
  s_batt_height      = STAT_BATT_HEIGHT   * device_height / 168;
  s_batt_nib_height  = STAT_BATT_NIB_HEIGHT * device_height / 168;
  s_batt_width       = STAT_BATT_WIDTH    * device_width  / 144;
  
  layout_slot_height = (device_height - s_slot_top_height) / 2;
  int layout_slot_bot = s_slot_top_height + layout_slot_height;
  stat_batt_left     = device_width - s_batt_width - STAT_BATT_NIB_WIDTH - 1;

  slot_status = layer_create(GRect(ox, LAYOUT_STAT, device_width, s_slot_top_height));
  layer_set_update_proc(slot_status, slot_status_layer_update_callback);
  layer_add_child(window_layer, slot_status);

  statusbar = layer_create(GRect(0, 0, device_width, s_slot_top_height));
  layer_set_update_proc(statusbar, statusbar_layer_update_callback);
  layer_add_child(slot_status, statusbar);
  GRect stat_bounds = layer_get_bounds(statusbar);

  slot_top = layer_create(GRect(ox, s_slot_top_height, device_width, layout_slot_height));
  layer_set_update_proc(slot_top, slot_top_layer_update_callback);
  layer_add_child(window_layer, slot_top);
  GRect slot_top_bounds = layer_get_bounds(slot_top);

  slot_bot = layer_create(GRect(ox, layout_slot_bot, device_width, layout_slot_height));
  layer_set_update_proc(slot_bot, slot_bot_layer_update_callback);
  layer_add_child(window_layer, slot_bot);
  GRect slot_bot_bounds = layer_get_bounds(slot_bot);

  bmp_connection_layer = bitmap_layer_create( GRect((device_width / 2) - 10, STAT_BT_ICON_TOP, 20, 20) );
  layer_add_child(statusbar, bitmap_layer_get_layer(bmp_connection_layer));
  
  image_connection_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_LINKED_ICON);
  image_noconnection_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_UNLINK_ICON);

  bmp_charging_layer = bitmap_layer_create( GRect(SX(STAT_CHRG_ICON_LEFT), STAT_CHRG_ICON_TOP, 20, 20) );
  layer_add_child(statusbar, bitmap_layer_get_layer(bmp_charging_layer));
  image_charging_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING_ICON);
  image_hourvibe_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOURVIBE_ICON);
  image_dnd_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DONOTDISTURB_ICON);

  dnd_period_check();
  hourvibe_period_check();
  set_status_charging_icon();

  battery_layer = layer_create(stat_bounds);
  layer_set_update_proc(battery_layer, battery_layer_update_callback);
  layer_add_child(statusbar, battery_layer);

  datetime_layer = layer_create(slot_top_bounds);
  layer_set_update_proc(datetime_layer, datetime_layer_update_callback);
  layer_add_child(slot_top, datetime_layer);

  calendar_layer = layer_create(slot_bot_bounds);
  layer_set_update_proc(calendar_layer, calendar_layer_update_callback);
  layer_add_child(slot_bot, calendar_layer);
  layer_set_hidden(calendar_layer, true);

  splash_layer = layer_create(slot_bot_bounds);
  layer_set_update_proc(splash_layer, splash_layer_update_callback);
  layer_add_child(slot_bot, splash_layer);

  toggle_slot_bottom((void*)splash_layer);  
  bottom_toggle = app_timer_register(2000, &toggle_slot_bottom, (void*)calendar_layer); 

  date_layer = text_layer_create( GRect(REL_CLOCK_DATE_LEFT, REL_CLOCK_DATE_TOP, device_width - 4, SY(REL_CLOCK_DATE_HEIGHT)) ); 
  set_layer_attr_sfont(date_layer, device_height > 168 ? FONT_KEY_GOTHIC_28 : FONT_KEY_GOTHIC_24, GTextAlignmentCenter);
  position_date_layer(); 
  update_date_text();
  layer_add_child(datetime_layer, text_layer_get_layer(date_layer));

  weather_layer = layer_create(slot_top_bounds);
  layer_set_update_proc(weather_layer, weather_layer_update_callback);
  layer_add_child(datetime_layer, weather_layer);

  time_layer = text_layer_create( GRect(REL_CLOCK_TIME_LEFT, SY(REL_CLOCK_TIME_TOP), device_width - 2, SY(REL_CLOCK_TIME_HEIGHT)) );

#if defined(PBL_PLATFORM_EMERY)
  set_layer_attr_cfont(time_layer, RESOURCE_ID_FONT_FUTURA_CONDENSED_65, GTextAlignmentCenter);
#else
  set_layer_attr_sfont(time_layer, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter);
#endif

  toggle_weather();
  position_time_layer(); 
  update_time_text();
  layer_add_child(datetime_layer, text_layer_get_layer(time_layer));

  week_layer = text_layer_create( GRect(4, SY(REL_CLOCK_SUBTEXT_TOP), device_width - 4, SY(18)) );
  set_layer_attr_sfont(week_layer, device_height > 168 ? FONT_KEY_GOTHIC_18 : FONT_KEY_GOTHIC_14, GTextAlignmentLeft);
  layer_add_child(datetime_layer, text_layer_get_layer(week_layer));
  if ( settings.show_week == 0 ) {
    layer_set_hidden(text_layer_get_layer(week_layer), true);
  }

  day_layer = text_layer_create( GRect(4, SY(REL_CLOCK_SUBTEXT_TOP), device_width - 4, SY(18)) ); 
  set_layer_attr_sfont(day_layer, device_height > 168 ? FONT_KEY_GOTHIC_18 : FONT_KEY_GOTHIC_14, GTextAlignmentCenter);
  position_day_layer(); 
  layer_add_child(datetime_layer, text_layer_get_layer(day_layer));
  if ( settings.show_day == 0 ) {
    layer_set_hidden(text_layer_get_layer(day_layer), true);
  }

  ampm_layer = text_layer_create( GRect(0, SY(REL_CLOCK_SUBTEXT_TOP), device_width, SY(18)) );
  set_layer_attr_sfont(ampm_layer, device_height > 168 ? FONT_KEY_GOTHIC_18 : FONT_KEY_GOTHIC_14, GTextAlignmentRight);
  layer_add_child(datetime_layer, text_layer_get_layer(ampm_layer));
  if ( settings.show_am_pm == 0 ) {
    layer_set_hidden(text_layer_get_layer(ampm_layer), true);
  }

  update_datetime_subtext();

  text_connection_layer = text_layer_create( GRect(20+STAT_BT_ICON_LEFT, 0, stat_batt_left - (20 + STAT_BT_ICON_LEFT), SY(22)) ); 
  set_layer_attr_sfont(text_connection_layer, device_height > 168 ? FONT_KEY_GOTHIC_24 : FONT_KEY_GOTHIC_18, GTextAlignmentLeft);
  update_connection();
  position_connection_layer();
  
  layer_add_child(statusbar, text_layer_get_layer(text_connection_layer));
  
  // Initialize standard for ALL watches. (It will be dynamically resized in toggle_statusbar)
  text_phone_battery_layer = text_layer_create( GRect(4, s_batt_top - 3, 64, s_batt_height + 4) );
  set_layer_attr_sfont(text_phone_battery_layer, FONT_KEY_GOTHIC_18_BOLD, GTextAlignmentLeft);

  
  if (persist_exists(PK_PHONE_BATT)) {
    static char pb_text[10];
    snprintf(pb_text, sizeof(pb_text), "P: %d%%", (int)persist_read_int(PK_PHONE_BATT));
    text_layer_set_text(text_phone_battery_layer, pb_text);
  } else {
    text_layer_set_text(text_phone_battery_layer, "P: --%");
  }
  
  layer_add_child(statusbar, text_layer_get_layer(text_phone_battery_layer));
  
  toggle_phone_battery_display(); 

  set_unifont();

  statusbar_visible();
  toggle_statusbar();

  window_set_background_color(window, settings.inverted ? GColorWhite : GColorBlack);
}

static void window_unload(Window *window) {
  layer_destroy(text_layer_get_layer(text_phone_battery_layer));
  layer_destroy(text_layer_get_layer(text_connection_layer));
  layer_destroy(text_layer_get_layer(ampm_layer));
  layer_destroy(text_layer_get_layer(day_layer));
  layer_destroy(text_layer_get_layer(week_layer));
  layer_destroy(text_layer_get_layer(time_layer));
  layer_destroy(text_layer_get_layer(date_layer));
  layer_destroy(weather_layer);
  layer_destroy(splash_layer);
  layer_destroy(calendar_layer);
  layer_destroy(datetime_layer);
  layer_destroy(battery_layer);
  layer_remove_from_parent(bitmap_layer_get_layer(bmp_charging_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(bmp_connection_layer));
  bitmap_layer_destroy(bmp_charging_layer);
  bitmap_layer_destroy(bmp_connection_layer);
  gbitmap_destroy(image_connection_icon);
  gbitmap_destroy(image_noconnection_icon);
  gbitmap_destroy(image_charging_icon);
  gbitmap_destroy(image_hourvibe_icon);
  gbitmap_destroy(image_dnd_icon);
  layer_destroy(slot_bot);
  layer_destroy(slot_top);
  layer_destroy(statusbar);
  layer_destroy(slot_status);
}

static void deinit(void) {
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

void handle_vibe_suppression() {
  if (dnd_period_active || battery_plugged) {
    vibe_suppression = true;
  } else if (settings.vibe_hour && vibe_period_active) {
    vibe_suppression = false;
  } else {
    vibe_suppression = false;
  }
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
  *currentTime = *tick_time;
  update_time_text();
  if ( currentTime->tm_min % 10 == 0) {
    dnd_period_check();
    hourvibe_period_check();
    set_status_charging_icon();
    handle_vibe_suppression();
  }
  if (bluetooth_connected && adv_settings.weather_update) {
    if (adv_settings.weather_update && (currentTime->tm_min + 60) % adv_settings.weather_update == 0) {
      weather_request = app_timer_register(1000, &request_weather, NULL);
    } else if (weather.current == 999 && weather.requests < 5) {
      if (weather_request == NULL) { weather_request = app_timer_register(1000, &request_weather, NULL); } 
    } 
  } 

  if (units_changed & HOUR_UNIT) {
    request_timezone(NULL);
    update_datetime_subtext();
    if (settings.vibe_hour && vibe_period_active) {
      generate_vibe(settings.vibe_hour); 
    }
  }

  if (units_changed & DAY_UNIT) {
    layer_mark_dirty(datetime_layer);
    layer_mark_dirty(calendar_layer);
  }
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed)
{
  *currentTime = *tick_time;
  if (settings.show_week == 6) {
    update_seconds_text(week_layer);
  }
  if (settings.show_am_pm == 6) {
    update_seconds_text(ampm_layer);
  }
  if (units_changed & MINUTE_UNIT) {
    handle_minute_tick(tick_time, units_changed);
  }
}

static int need_second_tick_handler(void) {
  if ((settings.show_week == 6) || (settings.show_am_pm == 6)) { return 1; }
  return 0; 
}

static void switch_tick_handler(void) {
  tick_timer_service_unsubscribe(); 
  seconds_shown = need_second_tick_handler();
  if (seconds_shown) {
    tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
  }
}

void my_out_sent_handler(DictionaryIterator *sent, void *context) {
}
void my_out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

void in_js_ready_handler(DictionaryIterator *received, void *context) {
    watch_version_send(NULL);
    if (weather_request == NULL) { weather_request = app_timer_register(3000, &request_weather, NULL); } 
}

void in_weather_handler(DictionaryIterator *received, void *context) {
    Tuple *temp_key = dict_find(received, MESSAGE_KEY_weather_temp);
    Tuple *cond_key = dict_find(received, MESSAGE_KEY_weather_cond);
    
    // CACHING FIX: Only update if the temperature is valid (not 999)
    if (temp_key != NULL && temp_key->value->int16 != 999) { 
      weather.current = temp_key->value->int16; 
      if (cond_key != NULL) { 
        snprintf(weather.condition, sizeof(weather.condition), "%s", cond_key->value->cstring); 
      }
      weather.requests = 0;
      weather.failures = 0;
    } else {
      // It was 999 (an error). We ignore the payload and keep our current data, 
      // but tick up the failure counter.
      weather.failures++;
    }
    
    layer_mark_dirty(weather_layer);
}

void in_timezone_handler(DictionaryIterator *received, void *context) {
    Tuple *tz_offset = dict_find(received, MESSAGE_KEY_timezone_offset);
    if (tz_offset != NULL) {
      timezone_offset = tz_offset->value->int8;
      update_datetime_subtext();
    }
}

void in_configuration_handler(DictionaryIterator *received, void *context) {
    Tuple *debugging = dict_find(received, MESSAGE_KEY_debugging_on);
    if (debugging != NULL) {
      debug.general = (get_int(debugging) != 0);
    }

    Tuple *debuglang = dict_find(received, MESSAGE_KEY_debuglang_on);
    if (debuglang != NULL) {
      debug.language = (get_int(debuglang) != 0);
    }

    Tuple *style_inv = dict_find(received, MESSAGE_KEY_style_inv);
    if (style_inv != NULL) {
      settings.inverted = get_int(style_inv);
      GColor t_color = settings.inverted ? GColorBlack : GColorWhite;
      window_set_background_color(window, settings.inverted ? GColorWhite : GColorBlack);
      text_layer_set_text_color(date_layer, t_color);
      text_layer_set_text_color(time_layer, t_color);
      text_layer_set_text_color(week_layer, t_color);
      text_layer_set_text_color(day_layer, t_color);
      text_layer_set_text_color(ampm_layer, t_color);
      text_layer_set_text_color(text_connection_layer, t_color);
      text_layer_set_text_color(text_phone_battery_layer, t_color);
      layer_mark_dirty(window_get_root_layer(window));
      handle_battery(battery_state_service_peek()); // Force the dynamic color to recalculate
      layer_mark_dirty(window_get_root_layer(window));
    }

    Tuple *style_day_inv = dict_find(received, MESSAGE_KEY_style_day_inv);
    if (style_day_inv != NULL) {
      settings.day_invert = get_int(style_day_inv);
    }

    Tuple *style_grid = dict_find(received, MESSAGE_KEY_style_grid);
    if (style_grid != NULL) {
      settings.grid = get_int(style_grid);
    }

    Tuple *vibe_hour = dict_find(received, MESSAGE_KEY_vibe_hour);
    if (vibe_hour != NULL) {
      settings.vibe_hour = get_int(vibe_hour);
      set_status_charging_icon();
    }

    Tuple *INTL_DOWO = dict_find(received, MESSAGE_KEY_intl_dowo);
    if (INTL_DOWO != NULL) {
      settings.dayOfWeekOffset = get_int(INTL_DOWO);
    }

    Tuple *FMT_DATE = dict_find(received, MESSAGE_KEY_intl_fmt_date);
    if (FMT_DATE != NULL) {
      settings.date_format = get_int(FMT_DATE);
      update_date_text();
    }

    Tuple *style_week = dict_find(received, MESSAGE_KEY_style_week);
    if (style_week != NULL) {
      settings.show_week = get_int(style_week);
      if ( settings.show_week ) {
        layer_set_hidden(text_layer_get_layer(week_layer), false);
      }  else {
        layer_set_hidden(text_layer_get_layer(week_layer), true);
      }
    }

    Tuple *FMT_WEEK = dict_find(received, MESSAGE_KEY_intl_fmt_week);
    if (FMT_WEEK != NULL) {
      settings.week_format = get_int(FMT_WEEK);
    }

    Tuple *style_day = dict_find(received, MESSAGE_KEY_style_day);
    if (style_day != NULL) {
      settings.show_day = get_int(style_day);
      if ( settings.show_day ) {
        layer_set_hidden(text_layer_get_layer(day_layer), false);
      }  else {
        layer_set_hidden(text_layer_get_layer(day_layer), true);
      }
    }

    Tuple *style_am_pm = dict_find(received, MESSAGE_KEY_style_am_pm);
    if (style_am_pm != NULL) {
      settings.show_am_pm = get_int(style_am_pm);
      if ( settings.show_am_pm ) {
        layer_set_hidden(text_layer_get_layer(ampm_layer), false);
      }  else {
        layer_set_hidden(text_layer_get_layer(ampm_layer), true);
      }
    }

    if (need_second_tick_handler() != seconds_shown) {
      switch_tick_handler();
    }

    update_datetime_subtext();

    Tuple *VIBE_PAT_D = dict_find(received, MESSAGE_KEY_vibe_pat_disconnect);
    if (VIBE_PAT_D != NULL) {
      settings.vibe_pat_disconnect = get_int(VIBE_PAT_D);
    }
    Tuple *VIBE_PAT_C = dict_find(received, MESSAGE_KEY_vibe_pat_connect);
    if (VIBE_PAT_C != NULL) {
      settings.vibe_pat_connect = get_int(VIBE_PAT_C);
    }

    Tuple *track_battery = dict_find(received, MESSAGE_KEY_track_battery);
    if (track_battery != NULL) {
      settings.track_battery = get_int(track_battery);
      toggle_phone_battery_display(); 
      if (settings.track_battery) {
        battery_status_send(NULL); 
      }
    }

    Tuple *week_pattern = dict_find(received, MESSAGE_KEY_cal_week_pattern);
    if (week_pattern != NULL) {
      adv_settings.week_pattern = get_int(week_pattern);
    }

    Tuple *appkey;

    appkey = dict_find(received, MESSAGE_KEY_inv_slot_stat);
    if (appkey != NULL) { adv_settings.invertStatBar = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_inv_slot_top);
    if (appkey != NULL) { adv_settings.invertTopSlot = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_inv_slot_bot);
    if (appkey != NULL) { adv_settings.invertBotSlot = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_show_stat_bar);
    if (appkey != NULL) { adv_settings.showStatus = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_show_stat_batt);
    if (appkey != NULL) { adv_settings.showStatusBat = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_show_date);
    if (appkey != NULL) { adv_settings.showDate = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_dnd_start);
    if (appkey != NULL) { adv_settings.DND_start = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_dnd_stop);
    if (appkey != NULL) { adv_settings.DND_stop = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_dnd_noaccel);
    if (appkey != NULL) { adv_settings.DND_accel_off = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_vibe_start);
    if (appkey != NULL) { adv_settings.vibe_hour_start = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_vibe_stop);
    if (appkey != NULL) { adv_settings.vibe_hour_stop = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_vibe_days);
    if (appkey != NULL) { adv_settings.vibe_hour_days = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_idle_reminder);
    if (appkey != NULL) { adv_settings.idle_reminder = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_idle_vibe_patt);
    if (appkey != NULL) { adv_settings.idle_pattern = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_idle_start);
    if (appkey != NULL) { adv_settings.idle_start = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_idle_stop);
    if (appkey != NULL) { adv_settings.idle_stop = get_int(appkey); }

    appkey = dict_find(received, MESSAGE_KEY_weather_fmt);
    if (appkey != NULL) {
      adv_settings.weather_format = get_int(appkey);
      if (weather_request == NULL) { weather_request = app_timer_register(1000, &request_weather, NULL); }
    }

    appkey = dict_find(received, MESSAGE_KEY_weather_update);
    if (appkey != NULL) {
      if (get_int(appkey) < adv_settings.weather_update && weather_request == NULL) {
        weather_request = app_timer_register(1000, &request_weather, NULL);
      }
      adv_settings.weather_update = get_int(appkey);
    }
    statusbar_visible();
    toggle_weather();
    toggle_statusbar();

    Tuple *translation;

    Tuple *chosen_language = dict_find(received, MESSAGE_KEY_language);
    if (chosen_language != NULL) {
      strncpy(lang_gen.language, chosen_language->value->cstring, sizeof(lang_gen.language)-1);
      set_unifont();
    }

    const uint32_t key_abbr_days[7] = { MESSAGE_KEY_trans_abbr_sunday, MESSAGE_KEY_trans_abbr_monday, MESSAGE_KEY_trans_abbr_tuesday, MESSAGE_KEY_trans_abbr_wedsday, MESSAGE_KEY_trans_abbr_thursday, MESSAGE_KEY_trans_abbr_friday, MESSAGE_KEY_trans_abbr_saturday };
    for (int i = 0; i < 7; i++ ) {
      translation = dict_find(received, key_abbr_days[i]);
      if (translation != NULL) {
        strncpy(lang_gen.abbrDaysOfWeek[i], translation->value->cstring, sizeof(lang_gen.abbrDaysOfWeek[i])-1);
      }
    }

    const uint32_t key_days[7] = { MESSAGE_KEY_trans_sunday, MESSAGE_KEY_trans_monday, MESSAGE_KEY_trans_tuesday, MESSAGE_KEY_trans_wedsday, MESSAGE_KEY_trans_thursday, MESSAGE_KEY_trans_friday, MESSAGE_KEY_trans_saturday };
    for (int i = 0; i < 7; i++ ) {
      translation = dict_find(received, key_days[i]);
      if (translation != NULL) {
        strncpy(lang_days.DaysOfWeek[i], translation->value->cstring, sizeof(lang_days.DaysOfWeek[i])-1);
      }
    }

    const uint32_t key_months[12] = { MESSAGE_KEY_trans_january, MESSAGE_KEY_trans_february, MESSAGE_KEY_trans_march, MESSAGE_KEY_trans_april, MESSAGE_KEY_trans_may, MESSAGE_KEY_trans_june, MESSAGE_KEY_trans_july, MESSAGE_KEY_trans_august, MESSAGE_KEY_trans_september, MESSAGE_KEY_trans_october, MESSAGE_KEY_trans_november, MESSAGE_KEY_trans_december };
    for (int i = 0; i < 12; i++ ) {
      translation = dict_find(received, key_months[i]);
      if (translation != NULL) {
        strncpy(lang_months.monthsNames[i], translation->value->cstring, sizeof(lang_months.monthsNames[i])-1);
      }
    }

    const uint32_t key_statuses[2] = { MESSAGE_KEY_trans_connected, MESSAGE_KEY_trans_disconnected };
    for (int i = 0; i < 2; i++ ) {
      translation = dict_find(received, key_statuses[i]);
      if (translation != NULL) {
        strncpy(lang_gen.statuses[i], translation->value->cstring, sizeof(lang_gen.statuses[i])-1);
      }
    }

    vibe_suppression = true;
    update_connection();
    handle_vibe_suppression();

    persist_write_data(PK_SETTINGS, &settings, sizeof(settings) );
    persist_write_data(PK_LANG_GEN, &lang_gen, sizeof(lang_gen) );
    persist_write_data(PK_LANG_MONTHS, &lang_months, sizeof(lang_months) );
    persist_write_data(PK_LANG_DAYS, &lang_days, sizeof(lang_days) );
    persist_write_data(PK_DEBUGGING, &debug, sizeof(debug) );
    persist_write_data(PK_ADV_SETTINGS, &adv_settings, sizeof(adv_settings) );

  if (1) { layer_mark_dirty(calendar_layer); } 
  if (1) { layer_mark_dirty(datetime_layer); } 
}

void my_in_rcv_handler(DictionaryIterator *received, void *context) {
  
  Tuple *phone_batt = dict_find(received, MESSAGE_KEY_PhoneBattery);
  if (phone_batt) {
    int pb_val = (int)get_int(phone_batt);
    persist_write_int(PK_PHONE_BATT, pb_val); 
    static char pb_text[10];
    snprintf(pb_text, sizeof(pb_text), "P: %d%%", pb_val);
    text_layer_set_text(text_phone_battery_layer, pb_text);
  }
  
  Tuple *message_type = dict_find(received, MESSAGE_KEY_message_type);
  if (message_type != NULL) {
    switch ( get_int(message_type) ) {
    case MSG_VAL_SEND_WATCH_VERSION:
      in_js_ready_handler(received, context);
      return;
    case MSG_VAL_TIMEZONE_OFFSET:
      in_timezone_handler(received, context);
      return;
    case MSG_VAL_REQUEST_WEATHER:
      in_weather_handler(received, context);
      return;
    case 99: 
      in_configuration_handler(received, context);
      return;
    }
  } 
  
  in_configuration_handler(received, context);
}

void my_in_drp_handler(AppMessageResult reason, void *context) {
}

static void app_message_init(void) {
  app_message_register_inbox_received(my_in_rcv_handler);
  app_message_register_inbox_dropped(my_in_drp_handler);
  app_message_register_outbox_sent(my_out_sent_handler);
  app_message_register_outbox_failed(my_out_fail_handler);
  app_message_open(1280, 512);
}

static void init(void) {

  if (DEBUGLOG == 1) { debug.general = true; }
  if (TRANSLOG == 1) { debug.language = true; }
  currentTime = get_time();

  app_message_init();

  if (persist_exists(PK_SETTINGS)) {
    persist_read_data(PK_SETTINGS, &settings, sizeof(settings) );
    if (settings.version == 11) { 
      if (settings.date_format > 234) { settings.date_format = settings.date_format + 1; }
      settings.version = 12;
    }
    if (settings.version == 12) { 
      uint8_t rescued_track_battery = (uint8_t)settings.strftime_format[4];
      strncpy(settings.strftime_format, "%Y-%m-%d", sizeof(settings.strftime_format) - 1);
      settings.strftime_format[sizeof(settings.strftime_format) - 1] = '\0';
      settings.track_battery = rescued_track_battery;
      settings.version = 13;
    }
    if (persist_exists(PK_LANG_GEN)) {
      persist_read_data(PK_LANG_GEN, &lang_gen, sizeof(lang_gen) );
    }
    if (persist_exists(PK_LANG_MONTHS)) {
      persist_read_data(PK_LANG_MONTHS, &lang_months, sizeof(lang_months) );
    }
    if (persist_exists(PK_LANG_DAYS)) {
      persist_read_data(PK_LANG_DAYS, &lang_days, sizeof(lang_days) );
    }
    if (persist_exists(PK_DEBUGGING)) {
      persist_read_data(PK_DEBUGGING, &debug, sizeof(debug) );
    }
    if (persist_exists(PK_ADV_SETTINGS)) {
      persist_read_data(PK_ADV_SETTINGS, &adv_settings, sizeof(adv_settings) );
    }
  }

  if (DEBUGLOG == 1) { debug.general = true; }
  if (TRANSLOG == 1) { debug.language = true; }

  if (adv_settings.weather_update) {
    weather_request = app_timer_register(1250, &request_weather, NULL);
  }

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  
  // Handled natively via settings check in window_load now
  window_stack_push(window, false);

  switch_tick_handler();
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  handle_bluetooth(bluetooth_connection_service_peek()); 
  battery_state_service_subscribe(&handle_battery);
  handle_battery(battery_state_service_peek()); 
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}