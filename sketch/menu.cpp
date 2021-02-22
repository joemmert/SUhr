

#include "menu.h"

#include "SUhr_buttons.h"
#include "SUhr_oscillators.h"
bool (*processapp[10])();
bool (*initapp[10])();
bool (*exitapp[10])();

int menu_appind = 0;

String menu_appnames[10];

int menu_numapps = 0;

int menu_sel = 0;

int menu_scrollpos =3;


void init_menu() {
  menu_appind = 0;
  menu_numapps = 0;
  
}

void addmenuitem(String appn, bool (*fun)(), bool (*funinit)(), void (*funexit)()) {
  processapp[menu_numapps] = fun;
  initapp[menu_numapps] = funinit;
  exitapp[menu_numapps] = funexit;
  menu_appnames[menu_numapps] = appn;
  menu_numapps++;
}


void menu_return(int appind) {
  
    disp_update=true;
  bool success = false;

  //exit current app
  if (menu_appind != 0) {
    if (exitapp[menu_appind - 1] != 0) {
      (*exitapp[menu_appind - 1])();
    }
  }

  //select new app
  menu_appind = appind;

  clear_button_events();
  
  // start new app

  if (menu_appind != 0) {
    if (initapp[menu_appind - 1] != 0) {
      success = (*initapp[menu_appind - 1])();
    } else {
      success = true;
    }
  } else {
    success = true;
  }

  // if app failed to start, return to menu
  if (success == false) {
    menu_appind = 0;
    Serial.println("error");
  }

}

// process everything if necessary
// return true if sleep wanted
// return false if no sleep wanted
bool menu_process() {

  if (buttons.S1p) {
    menu_sel--;
    if (menu_sel < 0) {
      menu_sel = menu_numapps - 1;
    }
    
    buttons.S1p=false;
    disp_update=true;
  }

  if (buttons.S2p) {
    menu_sel++;
    if (menu_sel >= menu_numapps) {
      menu_sel = 0;
    }
    buttons.S2p=false;
    disp_update=true;
  }
  if (buttons.S3p) {
    menu_return(menu_sel+1);
    
    buttons.S3p=false;
    disp_update=true;
  }
  
  if (buttons.S4p) {
    menu_return(1);
    
    buttons.S4p=false;
    disp_update=true;
  }

if(menu_scrollpos+menu_sel*18<3){
  menu_scrollpos = 3-menu_sel*18;
}

if(menu_scrollpos+menu_sel*18>118){
  menu_scrollpos = 110-menu_sel*18;
}

if((disp_update)&&(menu_appind==0)){
  disp_update=false;
  SUhr_disp_clrbfr();


    SUhr_disp_buttonlabels('<', '>', 'S', 'W');

  SUhr_disp_setleftalign(12);
  SUhr_disp_setcursor(12, 10);
  SUhr_disp_settextsize(2);
  for (int ii = 0; ii < menu_numapps; ii++) {
    SUhr_disp_setcursor(11, menu_scrollpos+ii*18+1);
    if (menu_sel == ii) {
      SUhr_disp_fillrect(10,SUhr_disp_cury-2,109,17,0);
      SUhr_disp_settextcolor(1, 0);
    } else {
      SUhr_disp_settextcolor(0, 1);
    }

    SUhr_disp_println_string(menu_appnames[ii].c_str());
  }
  SUhr_disp_refresh();


}
  return true;

}
