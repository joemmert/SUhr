

#include "SUhr_BLE.h"
#include "Arduino.h"
#include "SUhr_HAL.h"
#include "SUhr_buttons.h"
#include "SUhr_display.h"
#include "menu.h"
#include "SUhr_settings.h"


// ring buffers for serial
volatile char ble_recv_buffer[256];
volatile char PC_recv_buffer[256];
volatile uint8_t ble_recv_writeind = 0;
volatile uint8_t ble_recv_readind = 0;
volatile uint8_t ble_recv_forwardind = 0;
volatile uint8_t PC_recv_writeind = 0;
volatile uint8_t PC_recv_readind = 0;


bool BLE_uart_blocked = false;

bool BLE_sleeping = false;
bool BLE_cmd_mode = false;

void BLE_init() {

  Serial1.begin(115200) ;

  //pinMode(btle_uart_rxd,INPUT_PULLUP);

  pinMode(btle_uart_rx_ind, OUTPUT);
  digitalWrite(btle_uart_rx_ind, LOW);
  BLE_sleeping = false;

  // Reset
  pinMode(btle_rst, OUTPUT);
  digitalWrite(btle_rst, LOW);
  _delay_ms(200);
  digitalWrite(btle_rst, HIGH);
  _delay_ms(5000);
  BLE_cmd_mode = false;


  //We are bac into standard state, now decide what to do based on settings
  if (SUhr_settings.BLE_enabled) { //BLe enabled -> sleep mode on, standard settings
    BLE_enter_cmdmode();
    BLE_enable_lowpower();

    BLE_sendcmdwait("SA,0");  //set authentication method
    BLE_sendcmdwait("S-,SUhr");//set serialized devicename

    BLE_reboot();


    BLE_enter_cmdmode();
    BLE_enable_lowpower();
    BLE_sendcmdwait("SW,0C,04");//pin for UART tx ind -> P1_6
    BLE_sendcmdwait("ST,0190,0190,0001,0300");//set BLE timing constraints
    BLE_sendcmdwait("PZ");//clear all services
    BLE_sendcmdwait("SS,40"); //activate transparent UART on boot
    BLE_sendcmdwait("PS,C444CA28457F45D7A622251D1888CE74");//create a service for the data available interupt
    BLE_sendcmdwait("PC,79FDA908008D48299264978781A39D4F,3E,01");//create a characteristic for the data available interupt

    BLE_reboot();

    BLE_enter_cmdmode();
    BLE_sendcmdwait("WC");
    _delay_ms(200);
    BLE_sendcmd("WW");
    _delay_ms(200);
    BLE_sendcmd("@PW_ON");
    _delay_ms(200);
    BLE_sendcmd("SHW,0072,00");
    _delay_ms(200);
    BLE_sendcmd("|O,08,00");// P1_2 as data interrupt
    _delay_ms(200);
    BLE_sendcmd("@CONN");
    _delay_ms(200);
    BLE_sendcmd("|O,08,%0072");
    _delay_ms(200);
    BLE_sendcmd("T,0038,0038,0000,002A");
    _delay_ms(200);
    BLE_sendcmd("@DISCON");
    _delay_ms(200);
    BLE_sendcmd("|O,08,00");
    _delay_ms(200);
    Serial1.print('\x1b');
    _delay_ms(500);
    BLE_reboot();
    BLE_enter_cmdmode();
    BLE_sendcmdwait("SR,0040");// run scripts after poweron

    BLE_reboot();

    BLE_sleep();
  } else { //BLE disabled, sleep mode off then off into dormant mode
    
    Serial.println(" ");
    Serial.println(" ");
    Serial.println("BLE off");
   BLE_enter_cmdmode();
//    
   BLE_disable_lowpower();
//    BLE_reboot();
//    BLE_enter_cmdmode();
//    BLE_disable_lowpower();
//    BLE_sendcmdwait("O,0");
//    _delay_ms(1000);
    Serial1.print("O,0\r");
  }

  //digitalWrite(btle_uart_rx_ind, HIGH);

}

void BLE_sleep() {
  BLE_sleeping = true;
  digitalWrite(btle_uart_rx_ind, HIGH);
}

void BLE_wake() {

  BLE_sleeping = false;
  digitalWrite(btle_uart_rx_ind, LOW);
}

void BLE_reboot() {
  BLE_sendcmd("R,1");
  BLE_waitforsequence("Rebooting", 1000);
  _delay_ms(5000);
  BLE_cmd_mode = false;

}

void BLE_enable_lowpower() {
  pinMode(btle_uart_rx_ind, OUTPUT);
  digitalWrite(btle_uart_rx_ind, LOW);

  BLE_sendcmd("SO,1");
  BLE_waitforsequence("AOK", 10000);

}
void BLE_disable_lowpower() {
  pinMode(btle_uart_rx_ind, OUTPUT);
  digitalWrite(btle_uart_rx_ind, LOW);

  BLE_sendcmd("SO,0");
  BLE_waitforsequence("AOK", 10000);

}


bool BLE_waitforsequence(char * seq, int duration) {
  char recvd[256];
  uint8_t recvpos = 0;
  bool ret = false;
  //unsigned long tim=millis();
  //while(((unsigned long) millis()-tim)<duration){
  unsigned long tim = 0;
  while (tim < duration) {
    _delay_ms(1);
    tim++;
    serialEvent1();
    while (ble_recv_writeind != ble_recv_readind) {
      recvd[recvpos] = ble_recv_buffer[ble_recv_readind];
      ble_recv_readind++;
      recvpos++;
      recvd[(uint8_t) recvpos + 1] = 0;
    }
    if (strstr(recvd, seq) != 0) {
      ret = true;
      break;
    }
  }
  return ret;
}

bool BLE_enter_cmdmode() {
  _delay_ms(100);
  BLE_uart_blocked = true;
  Serial1.print("$$$");
  BLE_cmd_mode = true;
  return BLE_waitforsequence("CMD>", 1000);
}



bool BLE_exit_cmdmode() {
  _delay_ms(100);
  Serial1.print("---\r");
  bool ret = BLE_waitforsequence("END", 1000);
  BLE_cmd_mode = false;
  BLE_uart_blocked = false;
  return ret;
}

void BLE_process_input(){
  
    serialEvent1();
    while (ble_recv_writeind != ble_recv_readind) {
      
      ble_recv_readind++;
      
    }
}

void BLE_sendcmd(char * cmd) {
  BLE_process_input();
  Serial1.print(cmd);
  Serial1.print("\r");
}
void BLE_sendcmdwait(char * cmd) {
  BLE_process_input();
  Serial1.print(cmd);
  Serial1.print("\r");
  BLE_waitforsequence("AOK", 10000);
}




int8_t BLE_sel = 0;

bool BLE_init_menu() {
  BLE_sel = 0;
  return true;
}

bool BLE_process() {

  if (buttons.S1p) {
    BLE_sel--;
    if (BLE_sel < 0) {
      BLE_sel = 2;
    }
    buttons.S1p = false;
  }

  if (buttons.S2p) {
    BLE_sel++;
    if (BLE_sel > 2) {
      BLE_sel = 0;
    }
    buttons.S2p = false;
  }

  if (buttons.S3p) {
    if (BLE_sel == 0) {
      BLE_wake();
      Serial.println("BLE off");
      BLE_enter_cmdmode();
      BLE_disable_lowpower();
      BLE_sendcmdwait("O,0");
    }
    if (BLE_sel == 1) {
      BLE_wake();

      // Reset
      pinMode(btle_rst, OUTPUT);
      digitalWrite(btle_rst, LOW);
      _delay_ms(200);
      digitalWrite(btle_rst, HIGH);
      _delay_ms(200);
      BLE_cmd_mode = false;



      BLE_enter_cmdmode();
      BLE_enable_lowpower();

      BLE_sendcmdwait("SA,0");  //set authentication method
      BLE_sendcmdwait("S-,SUhr");//set serialized devicename

      BLE_reboot();


      BLE_enter_cmdmode();
      BLE_enable_lowpower();
      BLE_sendcmdwait("SW,0C,04");//pin for UART tx ind -> P1_6
      BLE_sendcmdwait("ST,0190,0190,0001,0300");//set BLE timing constraints
      BLE_sendcmdwait("PZ");//clear all services
      BLE_sendcmdwait("SS,40"); //activate transparent UART on boot
      BLE_sendcmdwait("PS,C444CA28457F45D7A622251D1888CE74");//create a service for the data available interupt
      BLE_sendcmdwait("PC,79FDA908008D48299264978781A39D4F,3E,01");//create a characteristic for the data available interupt

      BLE_reboot();


      BLE_enter_cmdmode();
      BLE_sendcmdwait("WC");
      _delay_ms(200);
      BLE_sendcmd("WW");
      _delay_ms(200);
      BLE_sendcmd("@PW_ON");
      _delay_ms(200);
      BLE_sendcmd("SHW,0072,00");
      _delay_ms(200);
      BLE_sendcmd("|O,08,00");// P1_2 as data interrupt
      _delay_ms(200);
      BLE_sendcmd("@CONN");
      _delay_ms(200);
      BLE_sendcmd("|O,08,%0072");
      _delay_ms(200);
      BLE_sendcmd("T,0038,0038,0000,002A");
      _delay_ms(200);
      BLE_sendcmd("@DISCON");
      _delay_ms(200);
      BLE_sendcmd("|O,08,00");
      _delay_ms(200);
      Serial1.print('\x1b');
      _delay_ms(500);
      BLE_reboot();
      BLE_enter_cmdmode();
      BLE_sendcmdwait("SR,0040");// run scripts after poweron



      BLE_reboot();
    }

    
    if (BLE_sel == 2) {
      BLE_wake();

      // Reset
      pinMode(btle_rst, OUTPUT);
      digitalWrite(btle_rst, LOW);
      _delay_ms(200);
      digitalWrite(btle_rst, HIGH);
      _delay_ms(200);
      BLE_cmd_mode = false;



      BLE_enter_cmdmode();
      BLE_enable_lowpower();

      BLE_sendcmdwait("SA,0");  //set authentication method
      BLE_sendcmdwait("S-,SUhr");//set serialized devicename

      BLE_reboot();


      BLE_enter_cmdmode();
//      BLE_sendcmdwait("SW,0C,04");//pin for UART tx ind -> P1_6
//      BLE_sendcmdwait("ST,0190,0190,0001,0300");//set BLE timing constraints
//      BLE_sendcmdwait("PZ");//clear all services
//      BLE_sendcmdwait("SS,40"); //activate transparent UART on boot
//      BLE_sendcmdwait("PS,C444CA28457F45D7A622251D1888CE74");//create a service for the data available interupt
//      BLE_sendcmdwait("PC,79FDA908008D48299264978781A39D4F,3E,01");//create a characteristic for the data available interupt

      BLE_reboot();


      BLE_enter_cmdmode();
      BLE_sendcmdwait("WC");
      _delay_ms(200);
      BLE_sendcmd("WW");
      _delay_ms(200);
      BLE_sendcmd("@PW_ON");
      _delay_ms(200);
      BLE_sendcmd("SHW,0072,00");
      _delay_ms(200);
      BLE_sendcmd("|O,08,00");// P1_2 as data interrupt
      _delay_ms(200);
      BLE_sendcmd("@CONN");
      _delay_ms(200);
      BLE_sendcmd("|O,08,%0072");
      _delay_ms(200);
      BLE_sendcmd("T,0038,0038,0000,002A");
      _delay_ms(200);
      BLE_sendcmd("@DISCON");
      _delay_ms(200);
      BLE_sendcmd("|O,08,00");
      _delay_ms(200);
      Serial1.print('\x1b');
      _delay_ms(500);
      BLE_reboot();
      BLE_enter_cmdmode();
      BLE_sendcmdwait("SR,0040");// run scripts after poweron


      BLE_sendcmdwait("JC");

      BLE_sendcmdwait("JA,0,000000000000");


      BLE_reboot();
    }
    buttons.S3p = false;
  }

  if (buttons.S4p) {
    menu_return(0);

    buttons.S4p = false;
  }

  SUhr_disp_clrbfr();

  SUhr_disp_setcursor(10, 10);
  SUhr_disp_buttonlabels('<', '>', 'A', 'X');

  SUhr_disp_setcursor(10, 10);
  SUhr_disp_settextsize(2);


  SUhr_disp_settextcolor(0, 1);
  if (SUhr_settings.BLE_enabled) {
    SUhr_disp_println_string("BLE on");
  } else {
    SUhr_disp_println_string("BLE off");
  }


  if (BLE_sel == 0) {
    SUhr_disp_settextcolor(1, 0);
  } else {
    SUhr_disp_settextcolor(0, 1);
  }

  SUhr_disp_println_string("Off&Lock");


  if (BLE_sel == 1) {
    SUhr_disp_settextcolor(1, 0);
  } else {
    SUhr_disp_settextcolor(0, 1);
  }

  SUhr_disp_println_string("Conn.Dev.");


  if (BLE_sel == 2) {
    SUhr_disp_settextcolor(1, 0);
  } else {
    SUhr_disp_settextcolor(0, 1);
  }

  SUhr_disp_println_string("On&Lock");


  SUhr_disp_refresh();

  return true;
}
