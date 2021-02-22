
#include <Sodaq_LSM303AGR.h>

// Sharpmemory LCD
#define SHARP_SCK  7
#define SHARP_MOSI 5
#define SHARP_SS   4
#define SHARP_extmode 26
#define SHARP_extcomin 25



//Btle
#define btle_uart_rx_ind 31
#define btle_app 0
#define btle_rst 24
#define btle_data_int 1 //PCINT9
#define btle_P2_7 19
#define btle_uart_rxd 10
extern volatile bool data_available;


#define VIB 27
#define shtdn 2

// backlight
#define ledPin   13
#define BL_time 8
#define watchret_time 30
#define BL_timer_set (BL_time+watchret_time)
extern volatile int BL_counter;

void update_BL(int diffsec);
void setbacklight();
void init_accel();

void notify(int repeats);
bool shutdown_init();
bool shutdown_process();
double getbatteryvoltage();

bool getcharging();

void vibrate();



// battery
#define bat_sense 28
#define chg_sense 29


// Switsches
#define SW1 12 //right, top, PCINT28
#define SW2 3 //right, bottom, PCINT11
#define SW3 30 //left, bottom, PCINT6
#define SW4 20 //left, top, PCINT20





extern Sodaq_LSM303AGR accel;




// for debugging serial
#define BAUD 9600UL      // Baudrate
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
// for RN4871 communication
#define BAUD1 115200UL      // Baudrate
#define UBRR_VAL1 ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
