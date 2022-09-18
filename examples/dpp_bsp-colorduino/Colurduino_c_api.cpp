/*****************************************************************************
* Product: DPP on Colorduino (Funduino V1.a Hoya) board, cooperative QV kernel
* Last updated for version 7.1.1
* Last updated on  2022-09-17 by ef15c
*
* Copyright (C) 2022 Christian Schoffit.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* sphpn@free.fr
*****************************************************************************/

#include <Colorduino.h>
#include "Colorduino_c_api.h"

void Colorduino_refresh_line() {
  close_all_lines;
  Colorduino.run();
  Colorduino.open_line(Colorduino.line);
  if (++Colorduino.line > 7) Colorduino.line = 0;
}

void Colorduino_Init() {
  Colorduino.Init();
}

void Colorduino_IO_Init() {
  Colorduino._IO_Init();
}

void Colorduino_LED_Init() {
  Colorduino._LED_Init();
}

void Colorduino_TC2_Init() {
  Colorduino._TC2_Init();
}

void Colorduino_SetWhiteBal(unsigned char wbval[3]) {
  Colorduino.SetWhiteBal(wbval);
}

void Colorduino_FlipPage() {
  Colorduino.FlipPage();
}

void Colorduino_SetPixel(char x, char y, unsigned char r, unsigned char g, unsigned char b) {
  Colorduino.SetPixel(x, y, r, g, b);
}
  
void *Colorduino_getCurWriteFrame() {
  return Colorduino.curWriteFrame;
}

void *Colorduino_getCurDrawFrame() {
  return Colorduino.curDrawFrame;
}

size_t Colorduino_getPixelRGBSize() {
  return sizeof Colorduino.frameBuffer0;
}
