/*****************************************************************************
* Product: DPP on Colorduino (Funduino V1.a Hoya) board, cooperative QV kernel
* Last updated for version 7.1.1
* Last updated on  2022-09-17 by ef15c
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
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
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpc.h"
#include "dpp.h"
#include "bsp.h"

#include "Colorduino_c_api.h"

/* AVR MCU */
#include <avr/interrupt.h> 
/* add other drivers if necessary... */

//Q_DEFINE_THIS_FILE

/* random seed */
static uint32_t l_rnd;

/* CPU sleeping command */
static bool sleep = false;

/* Local-scope objects -----------------------------------------------------*/

/* ISRs used in this project ===============================================*/
ISR(TIMER2_COMPA_vect) {
  QTIMEEVT_TICK_X(0U, (void *)0);  /* process all time events at rate 0 */

  // Refresh LED Display
  Colorduino_refresh_line();
}

///**********************************************************
// * Traitement de l'interruption déclenchée par la réception
// * d'un caractère
// **********************************************************
// */
ISR(USART_RX_vect) {
  if (bit_is_clear(UCSR0A, UPE0)) {
    // No Parity error
    unsigned char c = UDR0;
    switch (c) { // read the incoming byte
      case 'p':
      case 'P':
        QACTIVE_POST(AO_Table, &pauseEvt, 0U);
        break;
      case 's':
      case 'S':
        QACTIVE_POST(AO_Table, &serveEvt, 0U);
        break;
      case 'l':
      case 'L':
        sleep = true;
        break;
      case 'h':
      case 'H':
        sleep = false;
        break;
    }
  } else {
    // Parity error, read byte but discard it
    UDR0;
  };
}
/**********************************************************
 * Initialisation de l'USART0 de l'ATmega328P
 **********************************************************
 */
void initUSART0(unsigned long baud)
{
  // Try u2x mode first
  uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
  UCSR0A = 1 << U2X0;

  // assign the baud_setting, a.k.a. ubrr (USART Baud Rate Register)
  UBRR0H = baud_setting >> 8;
  UBRR0L = baud_setting;

  UCSR0C = 0x06; /* 8 bits No parity 1 stop bit */

  UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
//  UCSR0B &= ~(1<<UDRIE0);
}


/* BSP functions ===========================================================*/
void BSP_init(void) {
    initUSART0(115200);
    Colorduino_IO_Init();           //Init IO
    Colorduino_LED_Init();          //Init LED Hardware
 
  // compensate for relative intensity differences in R/G/B brightness
  // array of 6-bit base values for RGB (0~63)
  // whiteBalVal[0]=red
  // whiteBalVal[1]=green
  // whiteBalVal[2]=blue
unsigned char whiteBalVal[3] = {25,63,63}; // for CILE 2088RGB-5 6x6cm round matrix
  Colorduino_SetWhiteBal(whiteBalVal);

}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char const *stat) {
  uint8_t r=0, g=0, b=0;
  int8_t x=-1, y=-1;

  /* Update color to new state */
  switch (*stat) {
    case 't':
      b = 127;
      break;
    case 'e':
      g = 127;
      break;
    case 'h':
      r = 127;
      break;
  }

  /* Locate philsosopher in the LED panel */
  switch (n) {
    case 0:
        x = 4;
        y = 2;
      break;
    case 1:
        x = 2;
        y = 2;
      break;
    case 2:
        x = 1;
        y = 4;
      break;
    case 3:
        x = 3;
        y = 6;
      break;
    case 4:
        x = 5;
        y = 4;
      break;
  }

  /* Update philosopher status in LED panel write buffer */
  Colorduino_SetPixel(x, y, r, g, b);
}
/*..........................................................................*/
void BSP_displayForkStat(uint8_t n, char const *stat) {
  uint8_t r=0, g=0, b=0;
  int8_t x=-1, y=-1;

  /* Update color to new state */
  switch (*stat) {
    case 'f':
      r = 10;
      g = 10;
      b = 10;
      break;
    case 'u':
      r = 127;
      g = 127;
      b = 127;
      break;
  }

  /* Locate fork in the LED panel */
  switch (n) {
    case 0:
        x = 3;
        y = 2;
      break;
    case 1:
        x = 1;
        y = 2;
      break;
    case 2:
        x = 1;
        y = 6;
      break;
    case 3:
        x = 5;
        y = 6;
      break;
    case 4:
        x = 5;
        y = 2;
      break;
  }

  /* Update philosopher status in LED panel write buffer */
  Colorduino_SetPixel(x, y, r, g, b);
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
  uint8_t r=0, g=0, b=0;

  if (paused == 0U) {
    /* Table in serving mode */
    r = 127;
    b = 127;
  }

  Colorduino_SetPixel(3, 4, r, g, b);
}
/*..........................................................................*/
uint32_t BSP_random(void) { /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * ((uint32_t)3U*7U*11U*13U*23U);

    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}


/* QF callbacks ============================================================*/
void QF_onStartup(void) {
    // set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking...
    TCCR2A = (1U << WGM21) | (0U << WGM20);
    TCCR2B = (1U << CS22 ) | (1U << CS21) | (1U << CS20); // 1/2^10
    ASSR  &= ~(1U << AS2);
    TIMSK2 = (1U << OCIE2A); // enable TIMER2 compare Interrupt
    TCNT2  = 0U;

    // set the output-compare register based on the desired tick frequency
    OCR2A  = (F_CPU / BSP_TICKS_PER_SEC / 1024U) - 1U;

    /* Initialize Idle LED */
    Colorduino_SetPixel(7, 0, 255, 255, 255); // Idle on
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QV_onIdle(void) {
  /* Display the philosopher, fork and table state on LED panel */
  Colorduino_FlipPage();
  /* toggle Top left LED on and then off, see NOTE1 */
  memcpy(Colorduino_getCurWriteFrame(), Colorduino_getCurDrawFrame(), Colorduino_getPixelRGBSize());
  Colorduino_SetPixel(7, 0,   0,   0,   0); // Idle off
  Colorduino_FlipPage();

  if (sleep) {
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular AVR MCU.
    */
    SMCR = (0 << SM0) | (1 << SE); // idle mode, adjust to your project
    QV_CPU_SLEEP();
  }
}

/*..........................................................................*/
Q_NORETURN Q_onAssert(char const * const module, int_t const loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    /* RESET */
    QF_RESET();
}

/* 
* NOTE1:
* One of the LEDs is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invocations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the User LED.
*/
