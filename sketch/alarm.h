


#include "SUhr_HAL.h"


extern uint8_t alarm_hour;
extern uint8_t alarm_minute;
extern bool alarm_on;

bool alarm_init();
bool alarm_process();
void alarm_exit();

void checkalarm();
