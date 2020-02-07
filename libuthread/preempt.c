#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define HUNDTHOUSAND 100000

static sigset_t sig_set;
struct itimerval auto_alarm;

void sig_handler() {
    uthread_yield();
}

void preempt_disable(void)
{
    sigprocmask(SIG_BLOCK, &sig_set, NULL);
}

void preempt_enable(void)
{
    sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
}

void preempt_setup(void) {
    auto_alarm.it_interval.tv_sec = 0;
    auto_alarm.it_interval.tv_usec = HUNDTHOUSAND/HZ;
    auto_alarm.it_value.tv_sec = 0;
    auto_alarm.it_value.tv_usec = HUNDTHOUSAND/HZ;
}
void preempt_start(void)
{
    preempt_setup();

    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGVTALRM);
}


