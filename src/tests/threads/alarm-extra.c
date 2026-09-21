#include "tests/threads/tests.h"
#include <stdint.h>
#include "devices/timer.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"

void
test_alarm_deadline (void)
{
  static const int delays[] = {1, 2, 7, 25, 1, 3};
  unsigned i;

  for (i = 0; i < sizeof delays / sizeof *delays; i++)
    {
      int64_t start = timer_ticks ();
      timer_sleep (delays[i]);
      if (timer_elapsed (start) < delays[i])
        fail ("returned before requested deadline: %d", delays[i]);
    }
  timer_sleep (0);
  timer_sleep (-1);
  timer_sleep (INT64_MIN);
  ASSERT (intr_get_level () == INTR_ON);
  msg ("six deadlines met; zero, negative and INT64_MIN returned");
  pass ();
}

#define STRESS_THREADS 24
#define STRESS_ROUNDS 20
static struct semaphore stress_done;
static int completed[STRESS_THREADS];

static void
stress_sleeper (void *aux)
{
  int id = (int) aux;
  int i;

  for (i = 0; i < STRESS_ROUNDS; i++)
    {
      int delay = 1 + (id * 7 + i * 3) % 11;
      int64_t start = timer_ticks ();
      timer_sleep (delay);
      if (timer_elapsed (start) < delay)
        fail ("stress sleeper returned early");
      completed[id]++;
    }
  sema_up (&stress_done);
}

void
test_alarm_stress (void)
{
  int i;
  sema_init (&stress_done, 0);
  for (i = 0; i < STRESS_THREADS; i++)
    {
      completed[i] = 0;
      ASSERT (thread_create ("stress", PRI_DEFAULT, stress_sleeper,
                             (void *) i) != TID_ERROR);
    }
  for (i = 0; i < STRESS_THREADS; i++)
    sema_down (&stress_done);
  for (i = 0; i < STRESS_THREADS; i++)
    ASSERT (completed[i] == STRESS_ROUNDS);
  msg ("24 threads completed 480 sleeps without early returns");
  pass ();
}

static struct semaphore huge_started;
static struct thread *huge_thread;

static void
huge_sleeper (void *aux UNUSED)
{
  huge_thread = thread_current ();
  sema_up (&huge_started);
  timer_sleep (INT64_MAX);
  fail ("INT64_MAX sleep returned prematurely");
}

void
test_alarm_huge (void)
{
  enum intr_level old_level;
  sema_init (&huge_started, 0);
  ASSERT (thread_create ("huge", PRI_DEFAULT, huge_sleeper,
                         NULL) != TID_ERROR);
  sema_down (&huge_started);
  timer_sleep (3);
  old_level = intr_disable ();
  ASSERT (huge_thread->status == THREAD_BLOCKED);
  intr_set_level (old_level);
  msg ("INT64_MAX remains blocked; observed for three ticks only");
  pass ();
}

void
test_alarm_invalid (void)
{
  msg ("EXPECTED PANIC: calling timer_sleep with interrupts disabled");
  intr_disable ();
  timer_sleep (1);
  fail ("invalid interrupt state was accepted");
}

unsigned long long __udivmoddi4 (unsigned long long n,
                               unsigned long long d,
                               unsigned long long *remainder);

void
test_alarm_arithmetic (void)
{
  unsigned long long q, r;
  q = __udivmoddi4 (0xffffffffffffffffULL, 10, &r);
  ASSERT (q == 1844674407370955161ULL && r == 5);
  q = __udivmoddi4 (0xffffffffffffffffULL, 0x100000000ULL, &r);
  ASSERT (q == 0xffffffffULL && r == 0xffffffffULL);
  q = __udivmoddi4 (7, 11, &r);
  ASSERT (q == 0 && r == 7);
  q = __udivmoddi4 (42, 7, NULL);
  ASSERT (q == 6);
  pass ();
}
