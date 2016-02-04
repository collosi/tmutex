#include "mutex.h"
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <linux/futex.h>

#include <errno.h>

#define cpu_relax() asm volatile("pause\n": : :"memory")


int mutex_init(mutex_t *m) {
  m->u = 0;
  return 0;
}

int mutex_destroy(mutex_t *m) {
  /* Do nothing */
  (void) m;
  return 0;
}

int mutex_lock(mutex_t *m) {
  int i, j;

  /* Try to grab lock */
  for (i = 0; i < 10; i++) {
    __transaction_atomic {
      if (m->b.locked == 0) {
        m->b.locked = 1;
        return 0;
      }
    }
    for (j = 0; j < 100; j++) {
      cpu_relax();
    }
  }

  /* Have to sleep */
  while (1) {

    __transaction_atomic {
      unsigned u = m->u;
      m->u = 257;
      if (!(u & 1)) {
        break;
      }
    }

    syscall(SYS_futex, m, FUTEX_WAIT_PRIVATE, 257, (void*)0, (void*)0, 0);
  }
  return 0;
}

int mutex_unlock(mutex_t*m) {
  int i;

  /* Locked and not contended */
  __transaction_atomic {
    if (m->u == 1) {
      m->u = 0;
      return 0;
    }
    /* Unlock */
    m->b.locked = 0;
  }

  __sync_synchronize();

  /* Spin and hope someone takes the lock */
  for (i = 0; i < 200; i++) {
    if (m->b.locked) return 0;

    cpu_relax();
  }

  /* We need to wake someone up */
  m->b.contended = 0;

  syscall(SYS_futex, m, FUTEX_WAKE_PRIVATE, 1, (void*)0, (void*)0, 0);

  return 0;
}

int mutex_trylock(mutex_t*m) {
  __transaction_atomic {
    if (!m->b.locked) {
      m->b.locked = 1;
      return 0;
    }
  }
  return EBUSY;
}

