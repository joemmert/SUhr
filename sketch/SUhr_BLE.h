
#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

// ring buffers for serial
extern volatile char ble_recv_buffer[256];
extern volatile char PC_recv_buffer[256];
extern volatile uint8_t ble_recv_writeind;
extern volatile uint8_t ble_recv_readind;
extern volatile uint8_t ble_recv_forwardind;
extern volatile uint8_t PC_recv_writeind;
extern volatile uint8_t PC_recv_readind;

extern void serialEvent1();
void BLE_init();

bool BLE_enter_cmdmode();

bool BLE_exit_cmdmode();

void BLE_enable_lowpower();

void BLE_disable_lowpower();

void BLE_reboot();


void BLE_sendcmd(char * cmd);

void BLE_sendcmdwait(char * cmd);

bool BLE_waitforsequence(char * seq, int duration);

void BLE_sleep();
void BLE_wake();


bool BLE_init_menu();

bool BLE_process();
