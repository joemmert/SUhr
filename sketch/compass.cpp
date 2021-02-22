#include "compass.h"
#include "menu.h"
#include "SUhr_buttons.h"

double mag_x_max, mag_y_max, mag_z_max, mag_x_min, mag_y_min, mag_z_min;
bool compass_init() {
  accel.enableMagnetometer(Sodaq_LSM303AGR::MagHighResMode, Sodaq_LSM303AGR::Hz100, Sodaq_LSM303AGR::Continuous);

  _delay_ms(500);
  mag_x_max = accel.getMagX();
  mag_x_min = mag_x_max;
  mag_y_max = accel.getMagY();
  mag_y_min = mag_y_max;
  mag_z_max = accel.getMagZ();
  mag_z_min = mag_z_max;
  return true;
}

bool compass_process() {
  if (buttons.S4p) {
    menu_return(0);
    buttons.S4p = false;
  }




  SUhr_disp_clrbfr();

  SUhr_disp_setcursor(10, 10);
  SUhr_disp_buttonlabels(' ', ' ', ' ', 'X');

  SUhr_disp_setcursor(10, 10);
  SUhr_disp_settextsize(2);

  SUhr_disp_settextcolor(0, 1);
  SUhr_disp_println_string("Compass");


  double curx = accel.getMagX();
  if (curx < mag_x_min) {
    mag_x_min = curx;
  }
  if (curx > mag_x_max) {
    mag_x_max = curx;
  }


  double cury = accel.getMagY();
  if (cury < mag_y_min) {
    mag_y_min = cury;
  }
  if (cury > mag_y_max) {
    mag_y_max = cury;
  }


  double curz = accel.getMagZ();
  if (curz < mag_z_min) {
    mag_z_min = curz;
  }
  if (curz > mag_z_max) {
    mag_z_max = curz;
  }

  double px = -30 * 2 * (curx - (mag_x_max + mag_x_min) / 2) / (mag_x_max - mag_x_min);
  double py = -30 * 2 * (cury - (mag_y_max + mag_y_min) / 2) / (mag_y_max - mag_y_min);
  double lngth = sqrt(px * px + py * py);
  double tx = px / lngth * 30;
  double ty = py / lngth * 30;
  int ipx = tx;
  int ipy = ty;
  int hpx = -ty / 5;
  int hpy = tx / 5;

  SUhr_disp_line(64, 64, ipx + 64, ipy + 64, 0);
  SUhr_disp_line(64 + hpx, 64 + hpy, 64 - hpx, 64 - hpy, 0);
  SUhr_disp_line(64 - hpx, 64 - hpy, 64 + ipx, 64 + ipy, 0);
  SUhr_disp_line(64 + ipx, 64 + ipy, 64 + hpx, 64 + hpy, 0);
  SUhr_disp_refresh();

  return false;
}



void compass_exit() {

  accel.disableMagnetometer();
}
