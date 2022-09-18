/*============================================================================
* QP/C Real-Time Embedded Framework (RTEF)
* Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
*
* SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
*
* This software is dual-licensed under the terms of the open source GNU
* General Public License version 3 (or any later version), or alternatively,
* under the terms of one of the closed source Quantum Leaps commercial
* licenses.
*
* The terms of the open source GNU General Public License version 3
* can be found at: <www.gnu.org/licenses/gpl-3.0>
*
* The terms of the closed source Quantum Leaps commercial licenses
* can be found at: <www.state-machine.com/licensing>
*
* Redistributions in source code must retain this top-level comment block.
* Plagiarizing this software to sidestep the license obligations is illegal.
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
============================================================================*/
/*!
* @date Last updated on: 2022-07-30
* @version Last updated for: @ref qpc_7_0_1
*
* @file
* @brief QF/C port to AVR, cooperative QV kernel
*/
#ifndef QF_PORT_H
#define QF_PORT_H
#include <avr/io.h>
/* The maximum number of active objects in the application, see NOTE01 */
#define QF_MAX_ACTIVE        8U

#define QF_EVENT_SIZ_SIZE    1U
#define QF_EQUEUE_CTR_SIZE   1U
#define QF_MPOOL_SIZ_SIZE    1U
#define QF_MPOOL_CTR_SIZE    1U
#define QF_TIMEEVT_CTR_SIZE  2U

/* ...do NOT use dynamic events */
#define QF_MAX_EPOOL         0U


#ifndef SREG
#define SREG _SFR_IO8(0x3F)
#define SREG_C                          0
#define SREG_Z                          1
#define SREG_N                          2
#define SREG_V                          3
#define SREG_S                          4
#define SREG_H                          5
#define SREG_T                          6
#define SREG_I                          7
#endif

/* QF CPU reset for AVR */
#define QF_RESET()       __asm__ __volatile__ ("jmp 0x0000" ::)

/* QF interrupt disable/enable... */
#define QF_INT_DISABLE() __asm__ __volatile__ ("cli" ::)
#define QF_INT_ENABLE()  __asm__ __volatile__ ("sei" ::)

/* QF critical section entry/exit... */
#define QF_CRIT_STAT_TYPE    unsigned char
#define QF_CRIT_ENTRY(stat_) do { \
    (stat_) =  SREG & (1<<SREG_I); \
    QF_INT_DISABLE(); \
} while (false)
#define QF_CRIT_EXIT(stat_)  do { \
    if (stat_) QF_INT_ENABLE(); \
} while (false)


#include <stdint.h>      /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>     /* Boolean type.      WG14/N843 C99 Standard */
//#include <Arduino.h>   /* Don't include, C++ not compatible with C */

#include "qep_port.h"   /* QEP port */
#include "qv_port.h"    /* QV cooperative kernel port */
#include "qf.h"         /* QF platform-independent public interface */

/*****************************************************************************
* NOTE01:
* The maximum number of active objects QF_MAX_ACTIVE can be increased
* up to 64, if necessary. Here it is set to a lower level to save some RAM.
*/

#endif /* QF_PORT_H */
