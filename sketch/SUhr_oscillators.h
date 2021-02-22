#include "compile_time.h"
#include "SUhr_HAL.h"

#include <inttypes.h>


//Timing

#define prescaler 64UL
#define int2_freq (32768UL/prescaler/256UL)



extern volatile uint8_t timer_read;
extern volatile uint16_t timer3_val;
extern uint8_t optimum_osccal;
// variables for the 32khz timer
extern volatile uint64_t lastepoch;
extern volatile uint64_t epoch;
extern volatile int prescale;
extern int diffsec;





typedef struct {
bool active;
int period, counter,repeats;
void (*onTimer)();
} timerentry_strct;


extern timerentry_strct timers[256];

void init_clocks();

uint32_t get_clock_freq();



uint8_t find_optimum_osccal();



uint8_t add_timer(int period, int repeats, void (*callback)()) ;


void delete_timer(int id) ;

void process_timers(int delta_t);
