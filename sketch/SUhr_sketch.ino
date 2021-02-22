
#ifdef F_CPU
#undef F_CPU
#endif
#define F_CPU 7372800UL



#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/delay.h>




#include <Sodaq_LSM303AGR.h>

#include <time.h>

#include "SUhr_HAL.h"
#include "SUhr_oscillators.h"
#include "SUhr_display.h"
#include "SUhr_buttons.h"
#include "menu.h"
#include "watchface.h"
#include "compass.h"
#include "SOOM.h"
#include "alarm.h"
#include "settime.h";
#include "setdate.h";
#include "SUhr_BLE.h"
#include "SUhr_settings.h"
#include "battery.h"
#include "UARTpassthrough.h"



Sodaq_LSM303AGR accel;



void setup() {

  setbacklight();
  SUhr_disp_init();
  SUhr_disp_set_interpolation(true);

  bool loaded = SUhr_load_settings();

  SUhr_disp_clrbfr();
  SUhr_disp_setcursor(10, 50);
  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_settextsize(2);
  SUhr_disp_println_string("   SUhr");
  SUhr_disp_println_string("  Booting");
  SUhr_disp_settextsize(1);
  if (loaded) {
    SUhr_disp_println_string(" Loaded Settings");
  } else {
    SUhr_disp_println_string(" Init. Settings");
  }
  SUhr_disp_refresh();
  _delay_ms(1000);


  SUhr_disp_clrbfr();
  SUhr_disp_setcursor(10, 50);
  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_settextsize(2);
  SUhr_disp_println_string("   SUhr");
  SUhr_disp_println_string("  Booting");
  SUhr_disp_settextsize(1);
  SUhr_disp_println_string(" Clocks");
  SUhr_disp_refresh();





  init_clocks();

  add_timer(3600, -1, &checkbattery) ;


  SUhr_disp_clrbfr();
  SUhr_disp_setcursor(10, 50);
  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_settextsize(2);
  SUhr_disp_println_string("   SUhr");
  SUhr_disp_println_string("  Booting");
  SUhr_disp_settextsize(1);
  SUhr_disp_println_string("  Button Interrupts");
  SUhr_disp_refresh();
  init_buttons();




  SUhr_disp_clrbfr();
  SUhr_disp_setcursor(10, 50);
  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_settextsize(2);
  SUhr_disp_println_string("   SUhr");
  SUhr_disp_println_string("  Booting");
  SUhr_disp_settextsize(1);
  SUhr_disp_println_string("  Accel./Magn.");
  SUhr_disp_refresh();
  init_accel();


  SUhr_disp_clrbfr();
  SUhr_disp_setcursor(10, 50);
  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_settextsize(2);
  SUhr_disp_println_string("   SUhr");
  SUhr_disp_println_string("  Booting");
  SUhr_disp_settextsize(1);
  SUhr_disp_println_string("    Menu");
  SUhr_disp_refresh();
  init_menu();
  addmenuitem("Watch", &watchface_process, &watchface_init, 0);
  addmenuitem("Compass", &compass_process, &compass_init, &compass_exit);
  addmenuitem("Alarm", &alarm_process, &alarm_init, 0);
  addmenuitem("Set Time", &settime_process, &settime_init, 0);
  addmenuitem("Set Date", &setdate_process, &setdate_init, 0);
  addmenuitem("SOOM", &soom_process, &soom_init, &soom_exit);
  addmenuitem("BLE Conf.", &BLE_process, &BLE_init_menu, 0);
  addmenuitem("Passthrg", &UARTpassthrough_process, &UARTpassthrough_init, UARTpassthrough_exit);
  addmenuitem("Turn off", &shutdown_process, &shutdown_init, 0);




  // debugserial

  Serial.begin(9600);



  setbacklight();
  SUhr_disp_clrbfr();
  SUhr_disp_setcursor(10, 50);
  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_settextsize(2);
  SUhr_disp_println_string("   SUhr");
  SUhr_disp_println_string("  Booting");
  SUhr_disp_settextsize(1);
  SUhr_disp_println_string("    BLE");
  SUhr_disp_refresh();
  BLE_init();



  SUhr_disp_clr();

  Serial.println("booted");

  ACSR = 0x80;//deactivate analog comparators



}

void loop() {
  bool sleep = false;

  diffsec = epoch - lastepoch;
  lastepoch = epoch;

  //Backlight timer
  update_BL(diffsec);

  process_timers(diffsec);
  checkalarm();


  if (menu_appind == 0) {
    sleep = menu_process();

  } else {
    sleep = (*processapp[menu_appind - 1])();
  }
  //serialEvent1();

  //go to sleep if possible
  if (sleep) {

    OCR2A = 0;                       // Dummyzugriff
    while ((ASSR & (1 << OCR2AUB))); // Warte auf das Ende des Zugriffs
    //sleep
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    sleep_mode();

  }


}

//
//
//////////////////////////////////////////////
//// Init BLE
//////////////////////////////////////////////
//void initBLE() {
//  rn487xBle.hwInit(-1, btle_uart_rx_ind) ;
//
//  Serial1.begin(rn487xBle.getDefaultBaudRate()) ;
//
//  rn487xBle.initBleStream(&Serial1) ;
//  if (rn487xBle.swInit())
//  {
//    Serial.println("Init. procedure done!") ;
//  }
//  else
//  {
//    Serial.println("Init. procedure failed!") ;
//
//  }
//
//  // Fist, enter into command mode
//  rn487xBle.enterCommandMode();
//  //rn487xBle.factoryReset();
//  if (rn487xBle.swInit())
//  {
//    Serial.println("Init. procedure done!") ;
//  }
//  else
//  {
//    Serial.println("Init. procedure failed!") ;
//
//  }
//  rn487xBle.enterCommandMode();
//  //rn487xBle.enableLowPower();
//  //rn487xBle.sendCommand("ST,0020,0C80,0000,0C80");
//  //rn487xBle.sendCommand("ST,0006,0080,0000,0300");
//  rn487xBle.sendCommand("SA,5");
//  delay(200);
//  rn487xBle.setSerializedName("SUhr");
//  rn487xBle.reboot();
//  delay(2000);
//  rn487xBle.enterCommandMode();
//  delay(200);
//  //rn487xBle.sendCommand("JA,0,000000000000");
//  rn487xBle.enableLowPower();
//  delay(200);
//  delay(200);
//  rn487xBle.sendCommand("SW,0C,04");
//  delay(200);
//  rn487xBle.sendCommand("ST,0190,0190,0001,0300");
//  rn487xBle.enableLowPower();
//
//  rn487xBle.clearAllServices();
//  rn487xBle.setDefaultServices(0x40);
//  rn487xBle.setServiceUUID("c444ca28457f45d7a622251d1888ce74");
//  rn487xBle.setCharactUUID("79fda908008d48299264978781a39d4f", 0x3E, 1);
//  rn487xBle.reboot();
//  rn487xBle.enterCommandMode();
//  rn487xBle.sendCommand("WC");
//  _delay_ms(200);
//  rn487xBle.sendCommand("WW");
//  _delay_ms(200);
//  rn487xBle.sendCommand("@PW_ON");
//  _delay_ms(200);
//  rn487xBle.sendCommand("SHW,0072,00");
//  _delay_ms(200);
//  rn487xBle.sendCommand("|O,08,00");
//  _delay_ms(200);
//  rn487xBle.sendCommand("@CONN");
//  _delay_ms(200);
//  rn487xBle.sendCommand("|O,08,%0072");
//  _delay_ms(200);
//  rn487xBle.sendCommand("T,0038,0038,0000,002A");
//  _delay_ms(200);
//  rn487xBle.sendCommand("@DISCON");
//  _delay_ms(200);
//  rn487xBle.sendCommand("|O,08,00");
//  _delay_ms(200);
//  int temp32 = 27;
//  Serial1.print('\x1b');
//  delay(500);
//  rn487xBle.reboot();
//  rn487xBle.enterCommandMode();
//  rn487xBle.setSupportedFeatures(0x0040);
//
//  rn487xBle.reboot();
//
//  //rn487xBle.enterDataMode();
//
//
//  //rn487xBle.stopAdvertising() ;
//  rn487xBle.hwSleep();
//}
//
//
//
//






/////////////////////////////////////////////
// Serial events
/////////////////////////////////////////////
void serialEvent1() {
  // get the new byte:

  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    //Serial.print(inChar);
    ble_recv_buffer[ble_recv_writeind] = inChar;
    ble_recv_writeind++;
  }
}


void serialEvent() {
  // get the new byte:
  char inChar = (char)Serial.read();
  // add it to the inputString:
  PC_recv_buffer[PC_recv_writeind] = inChar;
  PC_recv_writeind++;


  while (Serial.available()) {
    inChar = (char)Serial.read();
    PC_recv_buffer[PC_recv_writeind] = inChar;
    PC_recv_writeind++;
  }
}
