#include "tests/threads/tests.h"
#include "devices/timer.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"

static struct semaphore started;
static struct semaphore finished;
static struct thread *sleeper_thread;

static void
blocking_sleeper (void *aux UNUSED)
{
  sleeper_thread = thread_current ();
  sema_up (&started);
  timer_sleep (100);
  sema_up (&finished);
}

void
test_alarm_blocking (void)
{
  enum intr_level old_level;
  enum thread_status status;

  sema_init (&started, 0);
  sema_init (&finished, 0);
  ASSERT (thread_create ("blocking", PRI_DEFAULT, blocking_sleeper,
                         NULL) != TID_ERROR);
  sema_down (&started);
  old_level = intr_disable ();
  status = sleeper_thread->status;
  intr_set_level (old_level);
  msg ("sleeping thread status=%d; expected BLOCKED=%d",
       status, THREAD_BLOCKED);
  if (status != THREAD_BLOCKED)
    fail ("timer_sleep keeps the sleeper READY (busy waiting)");
  sema_down (&finished);
  pass ();
}
