#include "SUhr_display.h"
#include "Arduino.h"

#include "font.h"
//#include <avr/pgmspace.h>
//#include "Adafruit_GFX.h"
//extern const unsigned char font[] PROGMEM;

uint8_t vcom = 0;

volatile bool disp_update=true;

uint8_t dispbuf[128][16];

int SUhr_disp_curx = 0;
int SUhr_disp_cury = 0;
uint8_t textcolor = 0;
uint8_t bgcolor = 1;
uint8_t textsize = 1;
uint8_t leftalign = 0;
uint8_t textsizehalf = 0;
bool interpolation=false;

void SUhr_disp_clrbfr() {
  for (int ii = 0; ii < 128; ii++) {
    for (int jj = 0; jj < 16; jj++) {
      dispbuf[ii][jj] = 255;
    }
  }
}

void SUhr_disp_clr() {
  SUhr_disp_clrbfr();
  SUhr_disp_refresh();
}

void SUhr_disp_init() {


  pinMode(SHARP_SS, OUTPUT);
  digitalWrite(SHARP_SS, LOW);

  pinMode(SHARP_MOSI, OUTPUT);
  pinMode(SHARP_SCK, OUTPUT);
  PRR0 &= ~(1 << PRSPI);

  pinMode(SHARP_SS, OUTPUT);
  digitalWrite(SHARP_SS, LOW);

  pinMode(SHARP_MOSI, OUTPUT);
  pinMode(SHARP_SCK, OUTPUT);

  SPCR = 0;                 // just to be sure
  // SPCR |= (1 << SPIE);   // not using interrupts, so leaving this at 0
  SPCR = (1 << SPE) | (1 << DORD) | (1 << MSTR);   // enable SPI
  //  SPCR |= (1 << DORD);   // LSB first
  //  SPCR |= (1 << MSTR);      // set master mode
  // SPCR |= (1 << CPOL);   // leaving this set to 0
  // SPCR |= (1 << CPHA);   // leaving this set to 0
  // SPCR |= (1 << SPR1);      // using a prescaler setting of 64 (1,0) in this register
  // SPCR |= (1 << SPR0);

  SUhr_disp_clr();
}

void SUhr_disp_refresh() {
  //toggle VCOM;
  if (vcom == 0) {
    vcom = VCOM_bit;
  } else {
    vcom = 0;
  }

  digitalWrite(SHARP_SS, HIGH);
  //_delay_ms(5);
  //


  SPDR = vcom | sharp_write;
  while ( !(SPSR & (1 << SPIF)));



  for (uint8_t ii = 0; ii < 128; ii++) {


    SPDR = ii + 1;
    while ( !(SPSR & (1 << SPIF)));

    for (int jj = 0; jj < 16; jj++) {

      SPDR = dispbuf[ii][jj];
      while ( !(SPSR & (1 << SPIF)));

    }

    SPDR = 0;
    while ( !(SPSR & (1 << SPIF)));

  }



  SPDR = 0;
  while ( !(SPSR & (1 << SPIF)));



  digitalWrite(SHARP_SS, LOW);


}


void SUhr_disp_pixel(int x, int y, uint8_t col) {
  if ((x >= 0) && (y >= 0) && (x < 128) && (y < 128)) {
    if (col == 0) {
      dispbuf[y][x / 8] &= ~(1 << (x % 8));
    } else {
      dispbuf[y][x / 8] |= (1 << (x % 8));
    }
  }
}

void SUhr_disp_line(int xs, int ys, int x1, int y1, uint8_t col) {
  int x0 = xs;
  int y0 = ys;
  int dx =  abs(x1 - x0);
  int sx = (x0 < x1) ? 1 : -1;
  int dy = -abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy; /* error value e_xy */
  int e2 = 2 * err;




  if (col == 0) {
    while (1) {  /* loop */

      if ((x0 >= 0) && (y0 >= 0) && (x0 < 128) && (y0 < 128)) {
        dispbuf[y0][x0 / 8] &= ~(1 << (x0 % 8));
      }

      // SUhr_disp_pixel(x0, y0, col);
      if ((x0 == x1) && (y0 == y1)) break;
      e2 = 2 * err;
      if (e2 >= dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 <= dx) {
        err += dx;
        y0 += sy;
      }
    }
  }

  if (col > 0) {
    while (1) {  /* loop */
      if ((x0 >= 0) && (y0 >= 0) && (x0 < 128) && (y0 < 128)) {
        dispbuf[y0][x0 / 8] |= (1 << (x0 % 8));
      }
      //SUhr_disp_pixel(x0, y0, col);
      if ((x0 == x1) && (y0 == y1)) break;
      e2 = 2 * err;
      if (e2 >= dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 <= dx) {
        err += dx;
        y0 += sy;
      }
    }
  }
}

void SUhr_disp_setcursor(int x, int y) {

  SUhr_disp_curx = x;
  SUhr_disp_cury = y;

}

void  SUhr_disp_setleftalign(int x) {

  leftalign = x;

}

void SUhr_disp_settextcolor(int te, int bg) {
  textcolor = te;
  bgcolor = bg;
}

void SUhr_disp_settextsize(int siz) {
  if (siz > 0) {
    textsize = siz;
    textsizehalf = (textsize+1) / 2;
  }
}

void SUhr_disp_print_string(char text[]) {
  char * pt;
  pt = text;
  while (*pt != 0) {
    SUhr_disp_print_char(*pt);
    pt++;
  }
}

void SUhr_disp_print_int(uint32_t i) {
  int siz = snprintf(NULL, 0, "%i", i);
  char * str = malloc(siz + 1);
  snprintf(str, siz + 1, "%i", i);
  str[siz] = 0;
  SUhr_disp_print_string(str);
  free(str);
}

void SUhr_disp_print_double(double i, int afterpoint) {
  int ful = (int) i;
  SUhr_disp_print_int(ful);
  SUhr_disp_print_char('.');
  double res = abs(i - ((double) ful)) * pow(10, afterpoint) + 0.5;
  uint8_t zeros =afterpoint-((int) log10((int) res))-1;
  while(zeros>0){
    zeros--;
    SUhr_disp_print_char('0');
  }
  
  SUhr_disp_print_int((int) res);

}



void SUhr_disp_println_string(char * text) {

  SUhr_disp_print_string(text);
  SUhr_disp_curx = leftalign;
  SUhr_disp_cury += 8 * textsize;

}


void SUhr_disp_fillrect(int x, int y, int width, int height, int col) {
  for (int xx = x; xx < (x + width + 1); xx++) {
    SUhr_disp_line(xx, y, xx, y + height, col);
  }
}

void SUhr_disp_println_int(int i) {

  SUhr_disp_print_int(i);
  SUhr_disp_curx = leftalign;
  SUhr_disp_cury += 8 * textsize;

}
void SUhr_disp_println_double(double i, int afterpoint) {

  SUhr_disp_print_double(i, afterpoint);
  SUhr_disp_curx = leftalign;
  SUhr_disp_cury += 8 * textsize;

}



void SUhr_disp_print_char_noninterp(char text) {
  uint8_t shape[5];
  for (int ii = 0; ii < 5; ii++) {
    shape[ii] = pgm_read_byte(&(font[(text * 5) + ii]));
  }
  int tx;
  int ty;
  uint8_t indx = 0;
  uint8_t restx = 0;
  uint8_t posshapx = 0;
  for (uint8_t xx = 0; xx < (5 * textsize); xx++) {
    tx = SUhr_disp_curx + xx;
    indx = tx / 8;
    restx = tx % 8;
    posshapx = xx / textsize;
    for (uint8_t yy = 0; yy < (8 * textsize); yy++) {
      ty = SUhr_disp_cury + yy;
      if ((ty < 128) && (tx < 128) && (tx >= 0) && (ty >= 0)) {
        if ((shape[posshapx] & (1 << (yy / textsize))) > 0) {
          if (textcolor == 0) {
            dispbuf[ty][indx] &= ~(1 << (restx));
          }
          if (textcolor == 1) {

            dispbuf[ty][indx] |= (1 << (restx));
          }
        } else {
          if (bgcolor == 0) {
            dispbuf[ty][indx] &= ~(1 << (restx));
          }
          if (bgcolor == 1) {

            dispbuf[ty][indx] |= (1 << (restx));
          }

        }
      }
    }
  }
  //    for (uint8_t xx = 0; xx < (6 * textsize); xx++) {
  //
  //      tx = SUhr_disp_curx+xx;
  //      indx =tx/8;
  //  for (uint8_t yy = 7 * textsize; yy < (8 * textsize); yy++) {
  //
  //      ty = SUhr_disp_cury+yy;
  //      if ((ty < 128) && (tx < 128) && (tx>=0) && (ty>=0)) {
  //        if (bgcolor == 0) {
  //          dispbuf[ty][indx] &= ~(1 << (tx % 8));
  //        }
  //        if (bgcolor == 1) {
  //
  //          dispbuf[ty][indx] |= (1 << (tx % 8));
  //        }
  //      }
  //    }
  //  }
  for (uint8_t xx = (5 * textsize); xx < (6 * textsize); xx++) {

    tx = SUhr_disp_curx + xx;
    indx = tx / 8;
    restx = tx % 8;
    for (uint8_t yy = 0; yy < (7 * textsize); yy++) {
      ty = SUhr_disp_cury + yy;
      if ((ty < 128) && (tx < 128) && (tx >= 0) && (ty >= 0)) {
        if (bgcolor == 0) {
          dispbuf[ty][indx] &= ~(1 << (tx % 8));
        }
        if (bgcolor == 1) {

          dispbuf[ty][indx] |= (1 << (tx % 8));
        }
      }
    }
  }
  SUhr_disp_curx += 6 * textsize;
  if (SUhr_disp_curx > (122)) {
    SUhr_disp_curx = leftalign;
    SUhr_disp_cury += 8 * textsize;
  }



}



static inline bool getcharpixvar(int xx, int yy,  bool shap[7][5]) {
  if ((xx < 0) || (yy < 0) || (xx >= 5) || (yy >= 8)) {
    return false;
  } else {
    return shap[yy][xx];
  }
}


uint8_t getinterpchar(uint8_t xx, uint8_t yy, uint8_t shapposx, uint8_t shapposy, bool shap[7][5]) {
  int xxx =     (xx + (textsizehalf)) / textsize; //   round((float) xx / (float) textsize);
  if (xxx == (shapposx)) {
    xxx--;
  }

  int yyy =   (yy + textsizehalf) / textsize; //round(((float) yy / (float) textsize));
  if (yyy == (shapposy)) {
    yyy--;
  }

  int xxx2 = 2 * shapposx - xxx;

  int yyy2 = 2 * shapposy - yyy;

  //int sum = getcharpixvar(xx/textsize,yyy,shap)+getcharpixvar(xx/textsize,yyy2,shap)+getcharpixvar(xxx,yy/textsize,shap)+getcharpixvar(xxx2,yy/textsize,shap);


  //if (sum<3){
  return (getcharpixvar(shapposx, yyy, shap) && getcharpixvar(xxx, shapposy, shap));
  //}else{
  //  return 0;
  //}
}

void SUhr_disp_set_interpolation(bool intp){
  interpolation = intp;
}

void SUhr_disp_print_char(char text) {
  if ((textsize == 1)||(interpolation==false)) {
    SUhr_disp_print_char_noninterp(text);
  } else {

    uint8_t shape[5];
    bool shap[7][5];
    uint8_t temp, temp2;
    for (uint8_t ii = 0; ii < 5; ii++) {
      temp = pgm_read_byte(&(font[(text * 5) + ii]));
      shape[ii] = temp;
      temp2 = 1;
      for (uint8_t jj = 0; jj < 7; jj++) {
        shap[jj][ii] = ((temp & temp2) > 0);
        temp2 *= 2;
      }
    }
    int tx;
    int ty;

    uint8_t indx, restx, shapposx, shapposy;
    for (uint8_t xx = 0; xx < (5 * textsize); xx++) {
      tx = SUhr_disp_curx + xx;
      indx = tx / 8;
      restx = tx % 8;
      shapposx = xx / textsize;
      for (uint8_t yy = 0; yy < (8 * textsize); yy++) {
        ty = SUhr_disp_cury + yy;
        shapposy = yy / textsize;
        if ((ty < 128) && (tx < 128) && (tx >= 0) && (ty >= 0)) {
          if (shap[shapposy][shapposx] && (shapposy < 7)) {
            if (textcolor == 0) {
              dispbuf[ty][indx] &= ~(1 << (restx));
            }
            if (textcolor == 1) {

              dispbuf[ty][indx] |= (1 << (restx));
            }
          } else {// background pixel-> try to interpolate
            if (shapposy < 7) {
              if (getinterpchar(xx, yy, shapposx, yy / textsize, shap)) {
                //if(false){
                if (textcolor == 0) {
                  dispbuf[ty][indx] &= ~(1 << (restx));
                }
                if (textcolor == 1) {

                  dispbuf[ty][indx] |= (1 << (restx));
                }
              } else {
                if (bgcolor == 0) {
                  dispbuf[ty][indx] &= ~(1 << (restx));
                }
                if (bgcolor == 1) {

                  dispbuf[ty][indx] |= (1 << (restx));
                }
              }
            } else {


              if (bgcolor == 0) {
                dispbuf[ty][indx] &= ~(1 << (restx));
              }
              if (bgcolor == 1) {

                dispbuf[ty][indx] |= (1 << (restx));
              }
            }

          }
        }
      }
    }
    //
    //  // spacer frame around char in background color
    //  for (uint8_t yy = 7 * textsize; yy < (8 * textsize); yy++) {
    //    for (uint8_t xx = 0; xx < (6 * textsize); xx++) {
    //      tx = SUhr_disp_curx+xx;
    //      ty = SUhr_disp_cury+yy;
    //      if ((ty < 128) && (tx < 128) && (tx>=0) && (ty>=0)) {
    //        if (bgcolor == 0) {
    //          dispbuf[ty][tx / 8] &= ~(1 << (tx % 8));
    //        }
    //        if (bgcolor == 1) {
    //
    //          dispbuf[ty][tx / 8] |= (1 << (tx % 8));
    //        }
    //      }
    //    }
    //  }
    for (uint8_t xx = (5 * textsize); xx < (6 * textsize); xx++) {
        tx = SUhr_disp_curx + xx;
      indx = tx / 8;
      restx = tx % 8;
      shapposx = xx / textsize;
      for (uint8_t yy = 0; yy < (8 * textsize); yy++) {
        ty = SUhr_disp_cury + yy;
        if ((ty < 128) && (tx < 128) && (tx >= 0) && (ty >= 0)) {
          if (bgcolor == 0) {
            dispbuf[ty][indx] &= ~(1 << (restx));
          }
          if (bgcolor == 1) {

            dispbuf[ty][indx] |= (1 << (restx));
          }
        }
      }
    }

    // advance
    SUhr_disp_curx += 6 * textsize;

    //line break
    if (SUhr_disp_curx > (128 - 6)) {
      SUhr_disp_curx = leftalign;
      SUhr_disp_cury += 8 * textsize;
    }


  }
}






//
//void SUhr_disp_print_char(char text) {
//
//  uint8_t shape[5];
//  bool shap[7][5];
//  uint8_t temp,temp2;
//  for (int ii = 0; ii < 5; ii++) {
//    temp = pgm_read_byte(&(font[(text * 5) + ii]));
//    shape[ii] = temp;
//    temp2=1;
//    for(inz jj=0;jj<7;jj++){
//      shap[jj][ii]= ((temp & temp2)>0);
//      temp*=2;
//    }
//  }
//  int tx;
//  int ty;
//
//  for (int xx = 0; xx < (5 * textsize); xx++) {
//    for (int yy = 0; yy < (7 * textsize); yy++) {
//      tx = SUhr_disp_curx+xx;
//      ty = SUhr_disp_cury+yy;
//      if ((ty < 128) && (tx < 128) && (tx>=0) && (ty>=0)) {
//        if ((shape[xx / textsize] & (1 << (yy / textsize))) > 0) {
//        //if (getcharpixvar((int)xx/textsize,(int)yy/textsize, shape) ){
//          if (textcolor == 0) {
//            dispbuf[ty][tx / 8] &= ~(1 << (tx % 8));
//          }
//          if (textcolor == 1) {
//
//            dispbuf[ty][tx / 8] |= (1 << (tx % 8));
//          }
//        } else {// background pixel-> try to interpolate
//          if(getinterpchar(xx, yy, shape)){
//          //if(false){
//            if (textcolor == 0) {
//              dispbuf[ty][tx / 8] &= ~(1 << (tx % 8));
//            }
//            if (textcolor == 1) {
//
//              dispbuf[ty][tx / 8] |= (1 << (tx % 8));
//            }
//          }else{
//
//
//            if (bgcolor == 0) {
//              dispbuf[ty][tx / 8] &= ~(1 << (tx % 8));
//            }
//            if (bgcolor == 1) {
//
//              dispbuf[ty][tx / 8] |= (1 << (tx % 8));
//            }
//          }
//
//        }
//      }
//    }
//  }
//
//  // spacer frame around char in background color
//  for (int yy = 7 * textsize; yy < (8 * textsize); yy++) {
//    for (int xx = 0; xx < (6 * textsize); xx++) {
//      tx = SUhr_disp_curx+xx;
//      ty = SUhr_disp_cury+yy;
//      if ((ty < 128) && (tx < 128) && (tx>=0) && (ty>=0)) {
//        if (bgcolor == 0) {
//          dispbuf[ty][tx / 8] &= ~(1 << (tx % 8));
//        }
//        if (bgcolor == 1) {
//
//          dispbuf[ty][tx / 8] |= (1 << (tx % 8));
//        }
//      }
//    }
//  }
//  for (int xx = (5 * textsize); xx < (6 * textsize); xx++) {
//    for (int yy = 0; yy < (7 * textsize); yy++) {
//      tx = SUhr_disp_curx+xx;
//      ty = SUhr_disp_cury+yy;
//      if ((ty < 128) && (tx < 128) && (tx>=0) && (ty>=0)) {
//        if (bgcolor == 0) {
//          dispbuf[ty][tx / 8] &= ~(1 << (tx % 8));
//        }
//        if (bgcolor == 1) {
//
//          dispbuf[ty][tx / 8] |= (1 << (tx % 8));
//        }
//      }
//    }
//  }
//
//  // advance
//  SUhr_disp_curx += 6 * textsize;
//
//  //line break
//  if (SUhr_disp_curx > (128 - 6)) {
//    SUhr_disp_curx = leftalign;
//    SUhr_disp_cury += 8 * textsize;
//  }
//
//
//
//}





void SUhr_disp_buttonlabels(char SW1s, char SW2s, char SW3s, char SW4s) {
  SUhr_disp_fillrect(0, 0, 9, 128, 0);
  SUhr_disp_fillrect(119, 0, 9, 128, 0);

  SUhr_disp_settextcolor(1, 0);
  SUhr_disp_settextsize(1);

  SUhr_disp_setcursor(1, 3);
  SUhr_disp_print_char(SW4s);

  SUhr_disp_setcursor(120, 3);
  SUhr_disp_print_char(SW1s);

  SUhr_disp_setcursor(1, 120);
  SUhr_disp_print_char(SW3s);

  SUhr_disp_setcursor(120, 120);
  SUhr_disp_print_char(SW2s);

  SUhr_disp_setleftalign(11);
}
