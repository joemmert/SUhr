#include "battery.h"
#include "menu.h"
#include "SUhr_buttons.h"


uint8_t bat_hist[256];
uint8_t bat_hist_ind=0;

bool battery_init() {
  
  return true;
}

bool battery_process() {
 
  return true;
}



void battery_exit() {

}


double checkbattery(){
  double batv=getbatteryvoltage();
  double temp=(batv-3.00)/(4.21-3.00)*255;
  
  bat_hist_ind++;
  if(temp>255){
    bat_hist[bat_hist_ind] = 255;
  }else{
    if(temp<0){
      bat_hist[bat_hist_ind] = 0;
    }else{
      bat_hist[bat_hist_ind] = round(temp);
    }
  }
  
  return batv;
}
