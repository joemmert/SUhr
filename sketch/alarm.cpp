#include "alarm.h"
#include "menu.h"
#include "SUhr_buttons.h"
#include "SUhr_oscillators.h"
#include "time.h"
#include "SUhr_HAL.h"

uint8_t alarm_hour = 6;
uint8_t alarm_minute = 0;
bool alarm_on = false;


int8_t alarm_sethour = 0;
//uint8_t alarm_settenminute = 0;
int8_t alarm_setminute = 0;

int8_t alarm_sel = 0;
int8_t alarm_setsel = 0;

bool alarm_init() {
  alarm_sel=0;
  alarm_setsel=0;
  return true;
}

bool alarm_process() {

  if (alarm_sel < 2) {
    if (buttons.S1p) {
      alarm_sel--;
      if (alarm_sel < 0) {
        alarm_sel = 1;
      }
      buttons.S1p = false;
    }

    if (buttons.S2p) {
      alarm_sel++;
      if (alarm_sel > 1) {
        alarm_sel = 0;
      }
      buttons.S2p = false;
    }

    if (buttons.S3p) {
      if (alarm_sel == 0) {
        alarm_on = !alarm_on;
      }
      if (alarm_sel == 1) {
        alarm_sel = 2;
        alarm_setsel = 0;
        alarm_setminute=alarm_minute;
        alarm_sethour=alarm_hour;
      }
      buttons.S3p = false;
    }
  } else {
    if (buttons.S1p) {
      if (alarm_setsel == 0) {
        alarm_sethour++;
        if (alarm_sethour >= 24) {
          alarm_sethour = 0;
        }
      }
      if (alarm_setsel == 1) {
        alarm_setminute += 10;
        if (alarm_setminute > 59) {
          alarm_setminute = alarm_setminute - 60;
        }
      }

      if (alarm_setsel == 2) {
        alarm_setminute ++;
        if (alarm_setminute > 59) {
          alarm_setminute = alarm_setminute - 60;
        }
      }
      buttons.S1p = false;
    }

    if (buttons.S2p) {

      if (alarm_setsel == 0) {
        alarm_sethour--;
        if (alarm_sethour < 0) {
          alarm_sethour = 23;
        }
      }
      if (alarm_setsel == 1) {
        alarm_setminute -= 10;
        if (alarm_setminute < 0) {
          alarm_setminute = alarm_setminute + 60;
        }
      }

      if (alarm_setsel == 2) {
        alarm_setminute--;
        if (alarm_setminute < 0) {
          alarm_setminute = alarm_setminute + 60;
        }
      }
      buttons.S2p = false;
    }

    if (buttons.S3p) {
      alarm_setsel++;
      if (alarm_setsel == 3) {
        alarm_setsel = 0;
        alarm_sel = 1;
        alarm_hour = alarm_sethour;
        alarm_minute = alarm_setminute;
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
  if (alarm_sel == 0) {
    SUhr_disp_settextcolor(1, 0);
  } else {
    SUhr_disp_settextcolor(0, 1);
  }

  SUhr_disp_print_string("Alarm ");
  if (alarm_on) {
    SUhr_disp_println_string("on");
  } else {
    SUhr_disp_println_string("off");
  }


  if (alarm_sel == 1) {
    SUhr_disp_settextcolor(1, 0);
  } else {
    SUhr_disp_settextcolor(0, 1);
  }

  SUhr_disp_println_string("Set alarm");

  
  SUhr_disp_settextsize(3);
  SUhr_disp_settextcolor(0, 1);

  if (alarm_sel < 2) {
    SUhr_disp_print_string(" ");
    SUhr_disp_print_int(alarm_hour);
    SUhr_disp_print_string(":");

    if (alarm_minute < 10) {
      SUhr_disp_print_string("0");
    } 
    SUhr_disp_print_int(alarm_minute);
    
  } else {
    SUhr_disp_print_string(" ");
    if (alarm_setsel == 0) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    SUhr_disp_print_int(alarm_sethour);

    SUhr_disp_settextcolor(0, 1);
    SUhr_disp_print_string(":");


    if (alarm_setsel == 1) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    if (alarm_setminute < 10) {
      SUhr_disp_print_string("0");
    } else {
      SUhr_disp_print_int(alarm_setminute / 10);
    }



    if (alarm_setsel == 2) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    SUhr_disp_print_int(alarm_setminute % 10);
  }



  SUhr_disp_refresh();

  return false;
}



void alarm_exit() {

}


void checkalarm(){
    if (alarm_on){
    struct tm ts;
    time_t temp_time = epoch;
    ts = *localtime(&temp_time);

    int8_t hour =  ts.tm_hour;
    int8_t minute =  ts.tm_min;
    int16_t sec =  ts.tm_sec;

    if((alarm_hour==hour) &&(alarm_minute==minute)){
      notify(10);
      alarm_on=false;
    }

    }
}
