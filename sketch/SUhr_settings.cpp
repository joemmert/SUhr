#include "SUhr_HAL.h"

#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include <avr/eeprom.h>

#include "SUhr_settings.h"


#define settings_version 100000UL  //better change this when significant changes to the structure are made -> Then you will have to redo all settings, but no garbage is read into the settings


//typedef struct  {
//  uint32_t vers_check;
//  bool BLE_enabled;
//  uint8_t BLE_partner_uuid[16];
//} SUhr_settings_t;

SUhr_settings_t SUhr_settings_eeprom EEMEM;

SUhr_settings_t SUhr_settings;

uint32_t cur_settings_vers = settings_version + sizeof(SUhr_settings)+ &SUhr_settings_eeprom;



void SUhr_standard_settings(SUhr_settings_t *standard){
  
  standard->vers_check = cur_settings_vers;
  standard->BLE_enabled = false;
  for(int ii=0; ii<16;ii++){
  standard->BLE_partner_uuid[ii]=0;
  }
}

bool SUhr_load_settings(){
  eeprom_read_block(&SUhr_settings, &SUhr_settings_eeprom, sizeof(SUhr_settings));
  if (SUhr_settings.vers_check==cur_settings_vers){
    // perfect we have a corrrect settings block in eeprom-> done
    return true;
  }else{
    // no valid settings block found -> initalize it
    SUhr_standard_settings(&SUhr_settings);
    SUhr_save_settings();
    return false;
  }

  
}

void SUhr_save_settings(){
  eeprom_update_block(&SUhr_settings, &SUhr_settings_eeprom, sizeof(SUhr_settings));
}
