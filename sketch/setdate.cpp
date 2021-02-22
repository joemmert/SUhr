#include "setdate.h"
#include "menu.h"
#include "SUhr_buttons.h"
#include "SUhr_oscillators.h"
#include "time.h"
#include "SUhr_HAL.h"



int8_t setdate_sethour = 0;
//uint8_t alarm_settenminute = 0;
int8_t setdate_setminute = 0;
int8_t setdate_setday = 0;
int8_t setdate_setmonth = 0;
int16_t setdate_setyear = 0;

int8_t setdate_sel = 0;
int8_t setdate_setsel = 0;

bool setdate_init() {
  setdate_sel=0;
  setdate_setsel=0;
  return true;
}

bool setdate_process() {

    struct tm ts;
    time_t temp_time = epoch;
    ts = *localtime(&temp_time);

    int8_t day =  ts.tm_mday;
    int8_t month =  ts.tm_mon + 1;
    int16_t year =  ts.tm_year + 1870;
    int8_t hour =  ts.tm_hour;
    int8_t minute =  ts.tm_min;
    int16_t sec =  ts.tm_sec;

  if (setdate_sel < 1) {
    if (buttons.S1p) {
      setdate_sel--;
      if (setdate_sel < 0) {
        setdate_sel = 0;
      }
      buttons.S1p = false;
    }

    if (buttons.S2p) {
      setdate_sel++;
      if (setdate_sel > 0) {
        setdate_sel = 0;
      }
      buttons.S2p = false;
    }

    if (buttons.S3p) {
      if (setdate_sel == 0) {
        setdate_sel = 1;
        setdate_setsel = 0;
        setdate_setminute=minute;
        setdate_sethour=hour;
        setdate_setday=day;
        setdate_setmonth=month;
        setdate_setyear=year;
      }
      buttons.S3p = false;
    }
  } else {
    if (buttons.S1p) {
      if (setdate_setsel == 0) {
        setdate_setday = setdate_setday+ 10;
        if (setdate_setday > getmaxdays(setdate_setmonth,setdate_setyear)) {
          setdate_setday = setdate_setday % 10;
        }
      }
      if (setdate_setsel == 1) {
        setdate_setday = setdate_setday+1;
        if (setdate_setday > getmaxdays(setdate_setmonth,setdate_setyear)) {
          setdate_setday = 1;
        }
      }
      if (setdate_setsel == 2) {
        setdate_setmonth ++;
        if (setdate_setmonth > 12) {
          setdate_setmonth = 1;
        }
      }

      if (setdate_setsel == 3) {
        setdate_setyear ++;
      }
      buttons.S1p = false;
    }

    if (buttons.S2p){
      if (setdate_setsel == 0) {
        setdate_setday = setdate_setday- 10;
        if (setdate_setday <1) {
          setdate_setday = getmaxdays(setdate_setmonth,setdate_setyear);
        }
      }
      if (setdate_setsel == 1) {
        setdate_setday = setdate_setday-1;
        if (setdate_setday <1) {
          setdate_setday = getmaxdays(setdate_setmonth,setdate_setyear);
        }
      }
      if (setdate_setsel == 2) {
        setdate_setmonth --;
        if (setdate_setmonth <1) {
          setdate_setmonth = 12;
        }
      }

      if (setdate_setsel == 3) {
        setdate_setyear --;
      }
      buttons.S2p = false;
    }

    if (buttons.S3p) {
      setdate_setsel++;
      if (setdate_setsel == 4) {
        setdate_setsel = 0;
        setdate_sel = 0;
        
        ts.tm_year = setdate_setyear-1870;
        ts.tm_mon = setdate_setmonth-1;
        ts.tm_mday = setdate_setday;
        epoch = mktime(&ts);
      }
      buttons.S3p = false;
    }
  }

  if (buttons.S4p) {
    menu_return(0);

    buttons.S4p = false;
  }




// display everything
  SUhr_disp_clrbfr();

  SUhr_disp_setcursor(10, 10);  
    SUhr_disp_buttonlabels('<', '>', 'A', 'X');

  SUhr_disp_setcursor(10, 10);  
  SUhr_disp_settextsize(2);



  if (setdate_sel == 0) {
    SUhr_disp_settextcolor(1, 0);
  } else {
    SUhr_disp_settextcolor(0, 1);
  }

  SUhr_disp_println_string("Set date");

    SUhr_disp_settextcolor(0, 1);
  SUhr_disp_settextsize(1);
  SUhr_disp_println_string("DD.MM.YYYY");
  SUhr_disp_settextsize(2);
  SUhr_disp_settextcolor(0, 1);

  if(setdate_sel < 1) {
    
    if (day < 10) {
      SUhr_disp_print_string("0");
    } 
    SUhr_disp_print_int(day);
    SUhr_disp_print_string(".");

    if (month < 10) {
      SUhr_disp_print_string("0");
    } 
    SUhr_disp_print_int(month);

    SUhr_disp_print_string(".");
    SUhr_disp_print_int(year);
  } else {


    if (setdate_setsel == 0) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    if (setdate_setday < 10) {
      SUhr_disp_print_string("0");
    } else {
      SUhr_disp_print_int(setdate_setday / 10);
    }



    if (setdate_setsel == 1) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    SUhr_disp_print_int(setdate_setday % 10);


    SUhr_disp_settextcolor(0, 1);

    SUhr_disp_print_string(".");

    if (setdate_setsel == 2) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    if (setdate_setmonth<10){
      SUhr_disp_print_string("0");
    }
    SUhr_disp_print_int(setdate_setmonth);

    SUhr_disp_settextcolor(0, 1);
    SUhr_disp_print_string(".");

    
    if (setdate_setsel == 3) {
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }
    SUhr_disp_print_int(setdate_setyear);

    SUhr_disp_settextcolor(0, 1);
    
  }



  SUhr_disp_refresh();

  return false;
}


int getmaxdays(int month, int year){
  if (month==1){
    return 31;
  }
  if (month==2){
    if (year % 4==0){
      return 29;
    }else{
      return 28;
    }
  }
  if (month==3){
    return 31;
  }
  if (month==4){
    return 30;
  }
  if (month==5){
    return 31;
  }
  if (month==6){
    return 30;
  }
  if (month==7){
    return 31;
  }
  if (month==8){
    return 31;
  }
  if (month==9){
    return 30;
  }
  if (month==10){
    return 31;
  }
  if (month==11){
    return 30;
  }
  if (month==12){
    return 31;
  }
}


void setdate_exit() {

}
