#include <qpc.h>
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

void setup() {
  // put your setup code here, to run once:
  static QEvt const *tableQueueSto[N_PHILO];
  static QEvt const *philoQueueSto[N_PHILO][N_PHILO];
  uint8_t n;

  Philo_ctor(); /* instantiate all Philosopher active objects */
  Table_ctor(); /* instantiate the Table active object */

  QF_init();    /* initialize the framework and the underlying RT kernel */
  BSP_init();   /* initialize the Board Support Package */

  /* start the active objects... */
  for (n = 0U; n < N_PHILO; ++n) {
     QACTIVE_START(AO_Philo[n],           /* AO to start */
                 (uint_fast8_t)(n + 1), /* QP priority of the AO */
                 philoQueueSto[n],      /* event queue storage */
                 Q_DIM(philoQueueSto[n]), /* queue length [events] */
                 (void *)0,             /* stack storage (not used) */
                 0U,                    /* size of the stack [bytes] */
                 (QEvt *)0);             /* initialization event */
  }
  QACTIVE_START(AO_Table,                  /* AO to start */
               (uint_fast8_t)(N_PHILO + 1), /* QP priority of the AO */
               tableQueueSto,             /* event queue storage */
               Q_DIM(tableQueueSto),      /* queue length [events] */
               (void *)0,                 /* stack storage (not used) */
               0U,                        /* size of the stack [bytes] */
               (QEvt *)0);                /* initialization event */
}

void loop() {
  // put your main code here, to run repeatedly:
  QF_run();
}
