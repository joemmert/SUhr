#include "settime.h"
#include "menu.h"
#include "SUhr_buttons.h"
#include "SUhr_oscillators.h"
#include "time.h"
#include "SUhr_HAL.h"



int8_t settime_sethour = 0;
//uint8_t alarm_settenminute = 0;
int8_t settime_setminute = 0;
int8_t settime_setday = 0;
int8_t settime_setmonth = 0;
int16_t settime_setyear = 0;

int8_t settime_sel = 0;
int8_t settime_setsel = 0;

bool settime_init() {
  settime_sel=0;
  settime_setsel=0;
  return true;
}

bool settime_process() {

    struct tm ts;
    time_t temp_time = epoch;
    ts = *localtime(&temp_time);

    int8_t day =  ts.tm_mday;
    int8_t month =  ts.tm_mon + 1;
    int16_t year =  ts.tm_year + 1870;
    int8_t hour =  ts.tm_hour;
    int8_t minute =  ts.tm_min;
    int16_t sec =  ts.tm_sec;

  if (settime_sel < 1) {
    if (buttons.S1p) {
      settime_sel--;
      if (settime_sel < 0) {
        settime_sel = 0;
      }
      buttons.S1p = false;
    }

    if (buttons.S2p) {
      settime_sel++;
      if (settime_sel > 0) {
        settime_sel = 0;
      }
      buttons.S2p = false;
    }

    if (buttons.S3p) {
      if (settime_sel == 0) {
        settime_sel = 1;
        settime_setsel = 0;
        settime_setminute=minute;
        settime_sethour=hour;
        settime_setday=day;
        settime_setmonth=month;
        settime_setyear=year;
      }
      buttons.S3p = false;
    }
  } else {
    if (buttons.S1p) {
      if (settime_setsel == 0) {
        settime_sethour++;
        if (settime_sethour >= 24) {
          settime_sethour = 0;
        }
      }
      if (settime_setsel == 1) {
        settime_setminute += 10;
        if (settime_setminute > 59) {
          settime_setminute = settime_setminute - 60;
        }
      }

      if (settime_setsel == 2) {
        settime_setminute ++;
        if (settime_setminute > 59) {
          settime_setminute = settime_setminute - 60;
        }
      }
      buttons.S1p = false;
    }

    if (buttons.S2p) {

      if (settime_setsel == 0) {
        settime_sethour--;
        if (settime_sethour < 0) {
          settime_sethour = 23;
        }
      }
      if (settime_setsel == 1) {
        settime_setminute -= 10;
        if (settime_setminute < 0) {
          settime_setminute = settime_setminute + 60;
        }
      }

      if (settime_setsel == 2) {
        settime_setminute--;
        if (settime_setminute < 0) {
          settime_setminute = settime_setminute + 60;
        }
      }
      buttons.S2p = false;
    }

    if (buttons.S3p) {
      settime_setsel++;
      if (settime_setsel == 3) {
        settime_setsel = 0;
        settime_sel = 0;
        hour = settime_sethour;
        minute = settime_setminute;
        ts.tm_hour = hour;
        ts.tm_min = minute;

        epoch = mktime(&ts);
      }
      buttons.S3p = false;
    }
  }

  if (buttons.S4p) {
    menu_return(0);

    buttons.S4p = false;
  }

  SUhr_disp_clrbfr();

  SUhr_disp_setcursor(10, 10);  
    SUhr_disp_buttonlabels('<', '>', 'A', 'X');

  SUhr_disp_setcursor(10, 10);  
  SUhr_disp_settextsize(2);



  if (settime_sel == 0) {
    SUhr_disp_settextcolor(1, 0);
  } else {
    SUhr_disp_settextcolor(0, 1);
  }

  SUhr_disp_println_string("Set time");

  
  SUhr_disp_settextsize(3);
  SUhr_disp_settextcolor(0, 1);

  if (settime_sel < 1) {
    SUhr_disp_print_string(" ");
    SUhr_disp_print_int(hour);
    SUhr_disp_print_string(":");

    if (minute < 10) {
      SUhr_disp_print_string("0");
    } 
    SUhr_disp_print_int(minute);
    
  } else {
    SUhr_disp_print_string(" ");
    if (settime_setsel == 0) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    SUhr_disp_print_int(settime_sethour);

    SUhr_disp_settextcolor(0, 1);
    SUhr_disp_print_string(":");


    if (settime_setsel == 1) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    if (settime_setminute < 10) {
      SUhr_disp_print_string("0");
    } else {
      SUhr_disp_print_int(settime_setminute / 10);
    }



    if (settime_setsel == 2) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    SUhr_disp_print_int(settime_setminute % 10);
  }



  SUhr_disp_refresh();

  return false;
}



void settime_exit() {

}
