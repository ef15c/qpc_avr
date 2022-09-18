/*$file${.::dpp.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: dpp.qm
* File:  ${.::dpp.h}
*
* This code has been generated by QM 5.2.1 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${.::dpp.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#ifndef DPP_H
#define DPP_H

#ifdef __cplusplus
extern "C"
{
#endif

enum DPPSignals {
    EAT_SIG = Q_USER_SIG, /* published by Table to let a philosopher eat */
    DONE_SIG,       /* published by Philosopher when done eating */
    PAUSE_SIG,      /* published by BSP to pause serving forks */
    SERVE_SIG,      /* published by BSP to serve re-start serving forks */
    TERMINATE_SIG,  /* published by BSP to terminate the application */
    HUNGRY_SIG,     /* posted direclty to Table from hungry Philo */
    TIMEOUT_SIG,    /* used by Philosophers for time events */
    MAX_SIG         /* the last signal */
};

/*$declare${Events::TableEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${Events::TableEvt} ......................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t philoNum;
} TableEvt;
/*$enddecl${Events::TableEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${Events::pauseEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${Events::pauseEvt} ......................................................*/
extern QEvt const pauseEvt;
/*$enddecl${Events::pauseEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${Events::serveEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${Events::serveEvt} ......................................................*/
extern QEvt const serveEvt;
/*$enddecl${Events::serveEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#define N_PHILO (5U)

/*$declare${AOs::Philo_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Philo_ctor} .......................................................*/
void Philo_ctor(void);
/*$enddecl${AOs::Philo_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::Table_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Table_ctor} .......................................................*/
void Table_ctor(void);
/*$enddecl${AOs::Table_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$declare${AOs::AO_Philo[N_PHILO]} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_Philo[N_PHILO]} ................................................*/
extern QActive * const AO_Philo[N_PHILO];
/*$enddecl${AOs::AO_Philo[N_PHILO]} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::AO_Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_Table} .........................................................*/
extern QActive * const AO_Table;
/*$enddecl${AOs::AO_Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifdef __cplusplus
}
#endif

#endif /* DPP_H */

