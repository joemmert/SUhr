#include "SOOM.h"
#include "menu.h"
#include "SUhr_buttons.h"


uint8_t gmap[10][10] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 0, 0, 2, 2, 0, 1},
  {1, 0, 0, 1, 0, 0, 2, 2, 0, 1},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

#define mx_max 10
#define my_max 10

double px = 1.5;
double py = 1.5;
double angle = 1.5;

double ang_per_pix = 0.01;
double v = 0.05;
bool soom_init() {
  accel.enableAccelerometer();
  return true;
}


double sign(double x) {
  if (x < 0) {
    return -1.0;
  } else {
    return 1.0;
  }
}

bool soom_process() {
  setbacklight();

  SUhr_disp_clrbfr();
  if (buttons.S3p) {
    menu_return(0);
    buttons.S3p = false;
  }
  if (buttons.S1_state) {
    angle -= 0.1;
  }
  if (buttons.S4_state) {
    angle += 0.1;
  }

  angle += accel.getX();






  double dx = cos(angle - ang_per_pix * 64);
  double dy = sin(angle - ang_per_pix * 64);

  double accel_pressure = -accel.getY() + accel.getZ();
  if (abs(accel_pressure) > 0.5) {
    accel_pressure = 0.5 * (double) sign(accel_pressure);
  }
  double mdx = cos(angle);
  double mdy = sin(angle);
  double tpx = px + mdx * accel_pressure * 0.2;
  double tpy = py + mdy * accel_pressure * 0.2;
  if (gmap[(int) floor(tpx)][(int)floor(tpy)] == 0) {
    px = tpx;
    py = tpy;
  }

  if (buttons.S2_state) {
    mdx = cos(angle);
    mdy = sin(angle);
    tpx = px + mdx * v;
    tpy = py + mdy * v;
    if (gmap[(int) floor(tpx)][(int)floor(tpy)] == 0) {
      px = tpx;
      py = tpy;
    }
  }









  //angle+=0.1;

  double tx = dx;
  double ty = dy;
  double r11 = cos(ang_per_pix);
  double r12 = -sin(ang_per_pix);
  double r21 = sin(ang_per_pix);
  double r22 = cos(ang_per_pix);
  bool hit = false;
  double invdx = 1 / dx;
  double invdy = 1 / dy;
  double bx = px;
  double by = py;
  double sidedistx, sidedisty;
  int stepx, stepy;
  int mapx, mapy, lasthitx, lasthity, lasthitside;
  double dist, dh, olddh;
  int side;
  olddh = 1;
  for (int xi = 0; xi <= 128; xi++) {
    tx = r11 * dx + r12 * dy;
    ty = r21 * dx + r22 * dy;
    dx = tx;
    dy = ty;
    bx = px;
    by = py;
    mapx = floor(bx);
    mapy = floor(by);

    if (dx < 0) {
      stepx = -1;
      sidedistx = (bx - floor(bx)) * invdx;
    } else {
      stepx = 1;
      sidedistx = (ceil(bx) - bx) * invdx;
    }
    if (dy < 0) {
      stepy = -1;
      sidedisty = (by - floor(by)) * invdy;
    } else {
      stepy = 1;
      sidedisty = (ceil(by) - by) * invdy;
    }

    //Thanks, https://lodev.org/cgtutor/raycasting.html
    invdx = (dy == 0) ? 0 : ((dx == 0) ? 1 : abs(1 / dx));
    invdy = (dx == 0) ? 0 : ((dy == 0) ? 1 : abs(1 / dy));


    //Thanks, https://lodev.org/cgtutor/raycasting.html
    hit = false;
    while (hit == false) {
      //jump to next map square, OR in x-direction, OR in y-direction
      if (sidedistx < sidedisty)
      {
        sidedistx += invdx;
        mapx += stepx;
        side = 0;
      }
      else
      {
        sidedisty += invdy;
        mapy += stepy;
        side = 1;
      }
      if ((mapx < 0) || (mapy < 0) || (mapx >= mx_max) || (mapy >= my_max)) {
        break;
      }
      //Check if ray has hit a wall
      if (gmap[mapx][mapy] > 0) hit = 1;
    }


    //Thanks, https://lodev.org/cgtutor/raycasting.html
    if (side == 0) dist = (mapx - px + (1 - stepx) / 2) / dx;
    else           dist = (mapy - py + (1 - stepy) / 2) / dy;

    
      dh = 64 / dist;
    
  if(abs(dh)>255){
    dh=255;
  }
    if ((mapx == lasthitx) && (mapy == lasthity) && (side == lasthitside)) {
      //displ.writePixel(xi, 64 - dh, 0);
      //displ.writePixel(xi, 64 + dh, 0);

      SUhr_disp_line(xi - 1, 64 - olddh, xi, 64 - dh, 0);
      SUhr_disp_line(xi - 1, 64 + olddh, xi, 64 + dh, 0);
    } else {

      SUhr_disp_line(xi - 1, 64 - olddh, xi, 64 - dh, 0);
      SUhr_disp_line(xi - 1, 64 + olddh, xi, 64 + dh, 0);
      if (olddh > dh) {
        SUhr_disp_line(xi, 64 - olddh, xi, 64 + olddh, 0);
      } else {
        SUhr_disp_line(xi, 64 - dh, xi, 64 + dh, 0);
      }
    }
  
    
    olddh = dh;
    lasthitside = side;
    lasthitx = mapx;
    lasthity = mapy;
  }


  Serial.println("L6");
  SUhr_disp_refresh();

  Serial.println("L7");
  return false;
}



void soom_exit() {

  accel.disableAccelerometer();
}
