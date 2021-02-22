

#include "SUhr_HAL.h"
#include "SUhr_display.h"

extern bool (*processapp[10])();

extern int menu_appind;

extern String menu_appnames[10];

extern int menu_numapps;


void init_menu();

void addmenuitem(String appn, bool (*fun)(), bool (*funinit)(), void (*funexit)()) ;

void menu_return(int appind);

bool menu_process();
