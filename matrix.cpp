/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator HAL
 *
 * MATRIX Creator HAL is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
hal::EverloopImage image1d;

  while (true) {
        for (auto& led : image1d.leds) {
          led.red = 255;
          led.green = 0;
          led.blue = 0;
          led.white = 0;
      }
    everloop.Write(&image1d);

    usleep(200000);
  }
*/

#include "matrix.h"

Matrix::Matrix() {
  bus.SpiInit();
  everloop.Setup(&bus);
}
void Matrix::setEverloop(hal::EverloopImage &img){
	m_img = img;
	everloop.Write(&m_img);
}
void Matrix::setEverloopAll(unsigned char r, unsigned char g, unsigned char b, unsigned char w){
      hal::EverloopImage image1d;
      for (auto& led : image1d.leds) {
          led.red = r;
          led.green = g;
          led.blue = b;
          led.white = w;
      }
      setEverloop(image1d);
}
void Matrix::setEverloopCircular(unsigned int ctr, unsigned char r, unsigned char g, unsigned char b, unsigned char w){
      hal::EverloopImage image1d;
      unsigned int refctr=0;
      for (auto& led : image1d.leds) {
	  if(ctr > refctr){
          	led.red = r;
          	led.green = g;
          	led.blue = b;
          	led.white = w;
	  	refctr++;
	  } else {
		led.red = 0;
          	led.green = 0;
          	led.blue = 0;
          	led.white = 1;
	  }
      }
      setEverloop(image1d);
}
