
typedef struct  {
  uint32_t vers_check;
  bool BLE_enabled;
  uint8_t BLE_partner_uuid[16];
} SUhr_settings_t;


extern SUhr_settings_t SUhr_settings;

extern uint32_t cur_settings_vers;


bool SUhr_load_settings();

void SUhr_save_settings();
