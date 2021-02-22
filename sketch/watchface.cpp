


#include "watchface.h"
#include "SUhr_buttons.h"
#include "SUhr_HAL.h"
#include "SUhr_oscillators.h"
#include "time.h"
#include "menu.h"
#include "alarm.h"

bool watch_update = false;
int8_t watch_lastmin = -1;

_Bool watchface_init() {
  watch_update = true;
  return true;
}


// process everything if necessary
// return true if sleep wanted
// return false if no sleep wanted
_Bool watchface_process() {
  bool sleep = true;
  if (buttons.S3p) {
    menu_return(0);
    sleep = false;
    buttons.S3p = false;
  }
//  if (buttons.S4p) {
//    buttons.S4p = false;
//notify(5);
//watch_update=true;
//  }


  struct tm ts;
  time_t temp_time = epoch;
  if (diffsec > 0) {

    ts = *localtime(&temp_time);

    if (watch_lastmin != ((int8_t) ts.tm_min)) {
      watch_lastmin =  ts.tm_min;
      watch_update = true;
    }
  }
  if (!sleep) {
    watch_update = false;
  }
  if (watch_update) {
    watch_update =false;
    ts = *localtime(&temp_time);

    int8_t day =  ts.tm_mday;
    int8_t month =  ts.tm_mon + 1;
    int16_t year =  ts.tm_year + 1870;
    int8_t hour =  ts.tm_hour;
    int8_t minute =  ts.tm_min;
    int16_t sec =  ts.tm_sec;




    SUhr_disp_clrbfr();
    SUhr_disp_fillrect(0, 0, 128, 20, 0);
    SUhr_disp_fillrect(0, 68, 128, 60, 0);

    SUhr_disp_buttonlabels(' ', ' ', 'M', ' ');

    if (alarm_on) {
      SUhr_disp_setcursor(20, 12);
      SUhr_disp_settextcolor(1, 0);
      SUhr_disp_settextsize(1);
      SUhr_disp_print_string("Alarm: ");
      SUhr_disp_print_int(alarm_hour);
      SUhr_disp_print_string(":");
      if (alarm_minute < 10) {
        SUhr_disp_print_string("0");
      }
      SUhr_disp_print_int(alarm_minute);

    }



    SUhr_disp_setcursor(20, 35);
    SUhr_disp_settextcolor(0, 1);
    SUhr_disp_settextsize(3);
    if (hour < 10) {
      SUhr_disp_print_string(" ");
    }
    SUhr_disp_print_int(hour);
    SUhr_disp_print_string(":");
    if (minute < 10) {
      SUhr_disp_print_string("0");
    }
    SUhr_disp_print_int(minute);

    SUhr_disp_setcursor(35, 24);
    SUhr_disp_settextcolor(0, 1);
    SUhr_disp_settextsize(1);
    SUhr_disp_print_int(day);
    SUhr_disp_print_string(".");
    SUhr_disp_print_int(month);
    SUhr_disp_print_string(".");
    SUhr_disp_print_int(year);




    SUhr_disp_setcursor(12,2);
    SUhr_disp_settextcolor(1,0);
    SUhr_disp_print_double(getbatteryvoltage(),2);
    SUhr_disp_print_string(" V");

    if (getcharging()){
      SUhr_disp_print_string(" CHRGNG");
    }
    
    SUhr_disp_refresh();
  }
  return sleep;
}
