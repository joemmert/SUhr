#include "SUhr_oscillators.h"

#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>




#include <Arduino.h>

volatile uint8_t timer_read = 1;
volatile uint16_t timer3_val = 0;
uint8_t optimum_osccal = 0;
// variables for the 32khz timer
volatile uint64_t epoch = 1611449712UL;//1609104219UL;//3600UL * 9UL + 12UL * 60UL;
volatile uint64_t lastepoch;
//volatile uint64_t epoch = seconds;
volatile int prescale = 0;
int diffsec = 0;


timerentry_strct timers[256];


ISR (TIMER2_OVF_vect)
{
  if (timer_read == 0) {
    timer3_val = TCNT3;
    timer_read = 1;
  }
  prescale++;
  if (prescale == int2_freq) {
    prescale = 0;
    epoch++;
  }

  digitalWrite(SHARP_extcomin, !digitalRead(SHARP_extcomin));
}


uint8_t add_timer(int period, int repeats, void (*callback)()) {
  int ii = 0;
  while (timers[ii].active) {
    ii++;
    if (ii == 256) {
      ii = -1;
      break;
    }
  }
  if (ii > -1) {
    timers[ii].active = true;
    timers[ii].period = period;
    timers[ii].counter = period;
    timers[ii].repeats = repeats;
    timers[ii].onTimer = callback;
  }
  return ii;
}


void delete_timer(int id) {
  if ((id >= 0) && (id < 256)) {
    timers[id].active = false;
    if (id < 255) {
      if (timers[id + 1].active) {
        timers[id] = timers[id + 1];
        delete_timer(id + 1);
      }
    }
  }
}

void process_timers(int delta_t) {
  int ii = 0;
  if (delta_t > 0) {
    while (timers[ii].active) {
      timers[ii].counter -= delta_t;
      if (timers[ii].counter <= 0) {
        if (timers[ii].repeats > 1) {
          timers[ii].repeats--;
          timers[ii].counter = timers[ii].period;
        } else {
          if (timers[ii].repeats == 1) {
            delete_timer(ii);
          } else {
            if (timers[ii].repeats < 0) {
              timers[ii].counter = timers[ii].period;
            }
          }
        }
        (*timers[ii].onTimer)();
      }

      ii++;
      if (ii == 256) {
        break;
      }
    }
  }
}

void init_clocks() {
  for (int ii = 0; ii < 256; ii++) {
    timers[ii].active = false;

  }

  pinMode(ledPin, OUTPUT);
  timer_read = 1;
  timer3_val = 0;
  optimum_osccal = 0;
  epoch = UNIX_TIMESTAMP;//1613918337UL+3600UL;//3600UL * 9UL + 12UL * 60UL;
  //epoch = seconds;
  lastepoch = epoch;
  prescale = 0;


  cli();
  //set up async timer2
  GTCCR |= (1 << TSM) | (1 << PSRASY);
  ASSR |= (1 << AS2);
  TIMSK2 = (1 << TOIE2);
  TCCR2A = 0;
  TCCR2B = ((1 << CS22) );
  GTCCR &= ~(1 << TSM);
  sei();


  // calibrate the RC oscillator, only needed when not using the resonator
  //find_optimum_osccal();

}

uint32_t get_clock_freq() {
  uint32_t temp;
  cli();
  GTCCR |= (1 << TSM)  | (1 << PSRSYNC);
  ASSR |= (1 << AS2);
  TIMSK2 = (1 << TOIE2);
  TCCR2A = 0;
  TCCR2B = ((1 << CS22) );

  TCCR3C = 0;
  TCCR3A = 0;
  TCCR3B = ((1 << CS32) | (1 << CS30) );
  TCNT2 = 0;
  TCNT3 = 0;

  GTCCR &= ~(1 << TSM);

  timer_read = 0;
  sei();
  while (timer_read == 0);

  TCCR3B = 0;
  temp = (((uint32_t) timer3_val) << 10) * (uint32_t) (int2_freq);
  return temp;


}




uint8_t find_optimum_osccal() {

  uint8_t ii;

  uint8_t stepsize = 32;
  uint32_t curr_clock;
  bool done = false;
  uint8_t sign = 0;

  curr_clock = get_clock_freq();

  OSCCAL = 64;
  for (ii = 0; ii < 200; ii++) {

  }

  while (!done) {

    curr_clock = get_clock_freq();
    if (curr_clock == F_CPU) {
      done = true;
      sign = 0;
    }
    if (curr_clock > F_CPU) {
      sign = -1;
    }
    if (curr_clock < F_CPU) {
      sign = 1;
    }
    OSCCAL = OSCCAL + sign * stepsize;
    stepsize = stepsize >> 1;
    if (stepsize == 0) {
      done = true;
    }
  }
  return OSCCAL;
}
