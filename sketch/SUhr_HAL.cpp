#include "SUhr_HAL.h"
#include "Arduino.h"
#include "menu.h"
#include "SUhr_oscillators.h"
volatile int BL_counter = 0;

volatile bool data_available = false;


void update_BL(int diffsec) {

  if (BL_counter > 0) {
    BL_counter = BL_counter - diffsec;
  }else{
    if (BL_counter==0){
      BL_counter=-1;
      menu_return(1);
    }
  }
  if (BL_counter >watchret_time){
    digitalWrite(ledPin, HIGH);
  } else {

    digitalWrite(ledPin, LOW);
  }
}


void setbacklight() {

  digitalWrite(ledPin, HIGH);
  BL_counter = BL_timer_set;
}

void init_accel(){
  //IMU
  Wire.begin();
  delay(1000);

  if (accel.checkWhoAmI()) {
    Serial.println("FOUND ACCEL!");
  }
  else {
    Serial.println("NO ACCEL!");
  }

  accel.disableAccelerometer();

  accel.disableMagnetometer();


}

void vibrate(){
  
    digitalWrite(VIB, HIGH);
    _delay_ms(200);
    digitalWrite(VIB, LOW);
    _delay_ms(50);

    digitalWrite(VIB, HIGH);
    _delay_ms(200);
    digitalWrite(VIB, LOW);
}

void notify(int repeats){
  setbacklight();

  add_timer(1, repeats, &vibrate) ;
  
}



bool shutdown_process() {

while(1);
return true;
}

bool shutdown_init() {
pinMode(shtdn,OUTPUT);
digitalWrite(shtdn,HIGH);
_delay_ms(200);
digitalWrite(shtdn,LOW);
_delay_ms(200);

  
return true;

}

double getbatteryvoltage(){
  ADCSRA |= (1 << ADEN);
    uint16_t batv_int = analogRead(bat_sense);
    batv_int = analogRead(bat_sense);
    batv_int = analogRead(bat_sense);
    batv_int = analogRead(bat_sense);
    batv_int += analogRead(bat_sense);
    batv_int += analogRead(bat_sense);
    batv_int += analogRead(bat_sense);
    ADCSRA &= ~(1 << ADEN);
    double temp;
    temp = batv_int>>2;
    double batv  = 10.1 * 1.1 * temp / 1024.0;
    if (batv< 3.05){
      shutdown_init();
    }
    return batv;
}

bool getcharging(){
  return !digitalRead(chg_sense);
}
