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
* @brief QV/C port port to MSP430
*/
#ifndef QV_PORT_H
#define QV_PORT_H

/* QV sleep mode, see NOTE1... */
#define QV_CPU_SLEEP()          do { \
    __asm__ __volatile__ ("sei" ::); \
    __asm__ __volatile__ ("sleep" ::); \
    SMCR = 0U; \
} while (false)

#include "qv.h"  /* QV platform-independent public interface */

#endif /* QV_PORT_H */

/*****************************************************************************
 * * NOTE1:
 * * As described in the "AVR Datasheet" in Section "Reset and Interrupt
 * * Handling", when using the SEI instruction to enable interrupts, the
 * * instruction following SEI will be executed before any pending interrupts.
 * * As the Datasheet shows in the assembly example, the pair of instructions
 * *     SEI       ; enable interrupts
 * *     SLEEP     ; go to the sleep mode
 * * executes ATOMICALLY, and so no interrupt can be serviced between these
 * * instructins. You should NEVER separate these two lines.
 */
