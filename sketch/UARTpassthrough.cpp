#include "UARTpassthrough.h"
#include "menu.h"
#include "SUhr_buttons.h"
#include "SUhr_BLE.h"
#include "Arduino.h"

bool UARTpassthrough_init() {




  SUhr_disp_clrbfr();

  SUhr_disp_setcursor(10, 10);
  SUhr_disp_buttonlabels(' ', ' ', ' ', 'X');

  SUhr_disp_setcursor(10, 10);
  SUhr_disp_settextsize(2);

  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_println_string("UART");
  SUhr_disp_println_string("Passthrgh");
  SUhr_disp_settextsize(1);

  SUhr_disp_println_string("Everything sent to");
  SUhr_disp_println_string("watch UART is"); 
  SUhr_disp_println_string("sent to RN4871 and");
  SUhr_disp_println_string("vice versa.");

  
  SUhr_disp_refresh();
  return true;
}

bool UARTpassthrough_process() {

  if (buttons.S4p) {
    menu_return(0);
    buttons.S4p = false;
  }
  

  //uart forwarding from ble to debug
  while (ble_recv_writeind != ble_recv_forwardind) {
    Serial.print(ble_recv_buffer[ble_recv_forwardind]);
    ble_recv_forwardind++;
  }
  while (PC_recv_writeind != PC_recv_readind) {
    Serial1.print(PC_recv_buffer[PC_recv_readind]);
    PC_recv_readind++;
  }
  Serial.flush();
  Serial1.flush();
 
  return false;
}



void UARTpassthrough_exit() {

}
