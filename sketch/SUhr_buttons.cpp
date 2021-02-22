#include "SUhr_buttons.h"
#include "SUhr_HAL.h"

#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <Arduino.h>


volatile buttons_strct buttons;

void init_buttons() {

  //analog reference for battery voltage sensing
  analogReference(INTERNAL1V1);
  // charge sensing
  pinMode(chg_sense,INPUT_PULLUP);
  // vibrator
  pinMode(VIB,OUTPUT);

  
  cli();
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  buttons.S1_state = digitalRead(SW1);
  buttons.S2_state = digitalRead(SW2);
  buttons.S3_state = digitalRead(SW3);
  buttons.S4_state = digitalRead(SW4);
  // set up switch interrupts
  PCMSK0 =  (1 << PCINT6); // SW3
  PCMSK1 = (1 << PCINT11) | (1 << PCINT9) ; // SW2 & data_int
  PCMSK2 = (1 << PCINT20) ; //SW4
  PCMSK3 =  (1 << PCINT28);//SW1
  PCICR = (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2) | (1 << PCIE3);
  sei();
}


void clear_button_events() {
  buttons.S1p = 0;
  buttons.S2p = 0;
  buttons.S3p = 0;
  buttons.S4p = 0;
  buttons.S1r = 0;
  buttons.S2r = 0;
  buttons.S3r = 0;
  buttons.S4r = 0;
}


ISR(PCINT0_vect) {
  uint8_t SW3_curr = digitalRead(SW3);
  if ((SW3_curr == LOW) && (buttons.S3_state == HIGH)) {
    buttons.S3p = true;
    BL_counter = BL_timer_set;
  }
  if ((SW3_curr == HIGH) && (buttons.S3_state == LOW)) {
    buttons.S3r = true;
    BL_counter = BL_timer_set;
  }
  buttons.S3_state = SW3_curr;
}




ISR(PCINT1_vect) {
  uint8_t SW2_curr = digitalRead(SW2);
  if ((SW2_curr == LOW) && (buttons.S2_state == HIGH)) {
    buttons.S2p = true;
    BL_counter = BL_timer_set;
  }
  if ((SW2_curr == HIGH) && (buttons.S2_state == LOW)) {
    buttons.S2r = true;
    BL_counter = BL_timer_set;
  }
  buttons.S2_state = SW2_curr;

  if (digitalRead(btle_data_int) == HIGH) {
    data_available = true;
  } else {
    data_available = false;

  }
}


ISR(PCINT2_vect) {
  uint8_t SW4_curr = digitalRead(SW4);
  if ((SW4_curr == LOW) && (buttons.S4_state == HIGH)) {
    buttons.S4p = true;
    BL_counter = BL_timer_set;
  }
  if ((SW4_curr == HIGH) && (buttons.S4_state == LOW)) {
    buttons.S4r = true;
    BL_counter = BL_timer_set;
  }
  buttons.S4_state = SW4_curr;
}




ISR(PCINT3_vect) {
  uint8_t SW1_curr = digitalRead(SW1);
  if ((SW1_curr == LOW) && (buttons.S1_state == HIGH)) {
    buttons.S1p = true;
    BL_counter = BL_timer_set;
  }
  if ((SW1_curr == HIGH) && (buttons.S1_state == LOW)) {
    buttons.S1r = true;
    BL_counter = BL_timer_set;
  }
  buttons.S1_state = SW1_curr;
}
