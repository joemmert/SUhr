
void init_buttons();

void clear_button_events();

typedef struct {
bool S1p,S2p,S3p,S4p;
bool S1r,S2r,S3r,S4r;
bool S1_state,S2_state,S3_state,S4_state;
} buttons_strct;


extern volatile buttons_strct buttons;
