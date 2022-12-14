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

Q_DEFINE_THIS_FILE

/* random seed */
static uint32_t l_rnd;

/* CPU sleeping command */
static volatile bool sleep = false;
static volatile bool nothingToDo;

/* Counter for CPU usage and Idle time measure */
static volatile uint16_t sysTickCount;
static uint16_t previousCycleTime;
static uint16_t busyTime;

#define BUF_SIZE_NB_BIT (7)
#define BUF_SIZE (1<<BUF_SIZE_NB_BIT)
#define NEXT_CHAR(x) ((x)+1 & (BUF_SIZE-1))
static char txBuffer[BUF_SIZE];
static volatile uint8_t txBufHead, txBufTail;

/* Local-scope objects -----------------------------------------------------*/

/* ISRs used in this project ===============================================*/
#define LED_REFRESH_PER_QTIME_TICKS (BSP_LED_ROW_REFRESHES_PER_SEC/BSP_TICKS_PER_SEC);

ISR(TIMER2_COMPA_vect) {
  static uint8_t count = LED_REFRESH_PER_QTIME_TICKS;

  sysTickCount++; /* Increment every 1.25 ms */
  if (--count==0) {
    count = LED_REFRESH_PER_QTIME_TICKS;
    QTIMEEVT_TICK_X(0U, (void *)0);  /* process all time events at rate 0 */
    nothingToDo = false;
  }

  // Refresh LED Display
  Colorduino_refresh_line();
}

///**********************************************************
// * Traitement de l'interruption d??clench??e par la
// * disponibit?? du registre d'envoi d'un caract??re
// **********************************************************
// */
ISR(USART_UDRE_vect) {
  char c;

  Q_ASSERT(txBufHead != txBufTail); /* TxBuffer should not be empty */
  
  c = txBuffer[txBufTail];
  txBufTail = NEXT_CHAR(txBufTail);

  UDR0 = c;

  if (txBufHead == txBufTail) {
    // Buffer empty, so disable interrupts
    UCSR0B &= ~(1<<UDRIE0);
  }
}

///**********************************************************
// * Traitement de l'interruption d??clench??e par la r??ception
// * d'un caract??re
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
        nothingToDo = false;
        break;
      case 's':
      case 'S':
        QACTIVE_POST(AO_Table, &serveEvt, 0U);
        nothingToDo = false;
        break;
      case 'l':
      case 'L':
        sleep = true;
        break;
      case 'h':
      case 'H':
        sleep = false;
        break;
      case 'd':
      case 'D':
        BSP_tx_write_string_P(PSTR("stc="));
        BSP_tx_write_u16dec(sysTickCount);
        BSP_tx_write_string_P(PSTR("\r\n"));
        
        BSP_tx_write_string_P(PSTR("pct="));
        BSP_tx_write_u16dec(previousCycleTime);
        BSP_tx_write_string_P(PSTR("\r\n"));
        
        BSP_tx_write_string_P(PSTR("bt="));
        BSP_tx_write_u16dec(busyTime);
        BSP_tx_write_string_P(PSTR("\r\n"));
        
        BSP_tx_write_string_P(PSTR("---\r\n"));

        BSP_tx_write_string_P(PSTR("sleep="));
        BSP_tx_write_u16dec(sleep);
        BSP_tx_write_string_P(PSTR("\r\n"));

        BSP_tx_write_string_P(PSTR("ntd="));
        BSP_tx_write_u16dec(nothingToDo);
        BSP_tx_write_string_P(PSTR("\r\n"));

        BSP_tx_write_string_P(PSTR("---\r\n"));
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
  UCSR0B &= ~(1<<UDRIE0);
}

size_t BSP_tx_write_char(const char c) {
  uint8_t next = NEXT_CHAR(txBufHead);
  
  if (next == txBufTail) {
    /* txBuffer is full */
    return 0;
  }

  txBuffer[txBufHead] = c;

  txBufHead = next;
  UCSR0B |= 1<<UDRIE0;

  return 1;
}

size_t BSP_tx_write_string(const char *s) {
  size_t len = 0;
  
  while (*s) {
    if (BSP_tx_write_char(*s++)) {
      len++;
    } else {
      break;
    }
  }
  return len;
}

size_t BSP_tx_write_string_P(PGM_P s) {
  size_t len = 0;

  char c;
  while ((c=pgm_read_byte(s++)) != 0) {
    if (BSP_tx_write_char(c)) {
      len++;
    } else {
      break;
    }
  }
  return len;
}

size_t BSP_tx_write_u16dec(uint16_t d) {
  size_t len = 0;
  uint16_t quotient = d/10;
  
  if (quotient!=0) {
    len = BSP_tx_write_u16dec(quotient);
  }
  
  return len+BSP_tx_write_char(d%10+'0');
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
    OCR2A  = (F_CPU / BSP_LED_ROW_REFRESHES_PER_SEC / 1024U) - 1U;
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QV_onIdle(void) {
  uint16_t pTime;
  uint8_t indic;

  nothingToDo = true;

  pTime = previousCycleTime;
  previousCycleTime = sysTickCount;
  
  indic = ((uint16_t) (previousCycleTime-busyTime))*255/(previousCycleTime-pTime);
  Colorduino_SetPixel(7, 0, indic, indic, indic); // Busy indicator

  /* Display the busy and awake indicators, philosopher, fork and table state on LED panel */
  Colorduino_FlipPage();
  memcpy(Colorduino_getCurWriteFrame(), Colorduino_getCurDrawFrame(), Colorduino_getPixelRGBSize());

  while (sleep && nothingToDo) {
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular AVR MCU.
    */
    SMCR = (0 << SM0) | (1 << SE); // idle mode, adjust to your project
    QV_CPU_SLEEP();
  }
  busyTime = sysTickCount;
}

/*..........................................................................*/
Q_NORETURN Q_onAssert(char const * const module, int_t const loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    BSP_tx_write_string_P(PSTR("ASSERTION:"));
    BSP_tx_write_string(module);
    BSP_tx_write_u16dec(loc);
    for (uint32_t volatile i = 100000U; i > 0U; --i) {
    }
    QF_INT_DISABLE(); // disable all interrupts
    QF_RESET();  // reset the CPU
    for (;;) {
    }
}

/* 
* NOTE1:
* One of the LEDs is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invocations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the User LED.
*/
