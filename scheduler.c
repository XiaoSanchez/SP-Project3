/* scheduler.c
 * Improved Scheduler Implementation with Enhanced Output
 *
 * This software is distributed under the terms
 * of the GPL, which you should have received
 * along with this source.
 *
 * Scheduler code for time-based functionality with additional output
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "scheduler.h"

struct schedule_entry *events = NULL;
static struct timeval zero = {0, 0};
static sigset_t alarm_sigset;

/* Function to get a formatted timestamp */
static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_now);
}

/* Initialize signal sets and set up the alarm handler */
void init_scheduler(void) {
    struct sigaction act;

    /* Initialize the event list */
    events = NULL;

    /* Set up the alarm handler */
    memset(&act, 0, sizeof(act));
    act.sa_handler = alarm_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGALRM);
    act.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Initialize the signal set for SIGALRM */
    sigemptyset(&alarm_sigset);
    sigaddset(&alarm_sigset, SIGALRM);
}

/* The alarm handler */
void alarm_handler(int signum) {
    (void)signum; // Suppress unused parameter warning

    struct schedule_entry *p;
    struct timeval now;
    struct itimerval itv;
    char timestamp[20];

    /* Block SIGALRM to prevent reentrancy */
    if (sigprocmask(SIG_BLOCK, &alarm_sigset, NULL) < 0) {
        perror("sigprocmask");
        return;
    }

    /* Get the current time */
    if (gettimeofday(&now, NULL) < 0) {
        perror("gettimeofday");
        sigprocmask(SIG_UNBLOCK, &alarm_sigset, NULL);
        return;
    }

    /* Execute due events */
    while (events && TVLESSEQ(events->tv, now)) {
        p = events;
        events = events->next;

        /* Log event execution */
        get_timestamp(timestamp, sizeof(timestamp));
        printf("[%s] Executing scheduled task.\n", timestamp);

        /* Execute the scheduled function */
        if (p->func) {
            p->func(p->data);
        }
        free(p);
    }

    /* Set the timer for the next event */
    if (events) {
        struct timeval interval;
        interval.tv_sec = events->tv.tv_sec - now.tv_sec;
        interval.tv_usec = events->tv.tv_usec - now.tv_usec;
        if (interval.tv_usec < 0) {
            interval.tv_sec -= 1;
            interval.tv_usec += 1000000;
        }
        if (interval.tv_sec < 0) {
            interval.tv_sec = 0;
            interval.tv_usec = 0;
        }

        /* Log next event timing */
        get_timestamp(timestamp, sizeof(timestamp));
        printf("[%s] Setting timer for next event in %ld.%06ld seconds.\n",
               timestamp, (long)interval.tv_sec, (long)interval.tv_usec);

        /* Set up the timer */
        itv.it_interval = zero;
        itv.it_value = interval;
        if (setitimer(ITIMER_REAL, &itv, NULL) < 0) {
            perror("setitimer");
        }
    }

    /* Unblock SIGALRM */
    if (sigprocmask(SIG_UNBLOCK, &alarm_sigset, NULL) < 0) {
        perror("sigprocmask");
    }
}

/* Schedule func to be run at relative time tv with data as arguments */
struct schedule_entry *schedule(struct timeval tv, void (*func)(void *), void *data) {
    struct schedule_entry *new_event, *current, *previous = NULL;
    struct timeval now;
    struct itimerval itv;
    int need_timer = 0;
    char timestamp[20];

    /* Validate input */
    if (!func) {
        fprintf(stderr, "schedule: func is NULL\n");
        return NULL;
    }

    /* Allocate memory for the new event */
    new_event = (struct schedule_entry *)malloc(sizeof(struct schedule_entry));
    if (!new_event) {
        perror("malloc");
        return NULL;
    }

    /* Get the current time */
    if (gettimeofday(&now, NULL) < 0) {
        perror("gettimeofday");
        free(new_event);
        return NULL;
    }

    /* Calculate the absolute execution time */
    new_event->tv.tv_sec = now.tv_sec + tv.tv_sec;
    new_event->tv.tv_usec = now.tv_usec + tv.tv_usec;
    if (new_event->tv.tv_usec >= 1000000) {
        new_event->tv.tv_sec += 1;
        new_event->tv.tv_usec -= 1000000;
    }

    /* Set the function and data */
    new_event->func = func;
    new_event->data = data;
    new_event->next = NULL;

    /* Block SIGALRM */
    if (sigprocmask(SIG_BLOCK, &alarm_sigset, NULL) < 0) {
        perror("sigprocmask");
        free(new_event);
        return NULL;
    }

    /* Insert the new event into the event list in order */
    current = events;
    while (current && TVLESS(current->tv, new_event->tv)) {
        previous = current;
        current = current->next;
    }

    if (previous) {
        new_event->next = previous->next;
        previous->next = new_event;
    } else {
        /* Insert at the beginning */
        new_event->next = events;
        events = new_event;
        need_timer = 1;
    }

    /* Log scheduling */
    get_timestamp(timestamp, sizeof(timestamp));
    printf("[%s] Scheduled new task to run at %ld.%06ld (in %ld.%06ld seconds).\n",
           timestamp,
           (long)new_event->tv.tv_sec, (long)new_event->tv.tv_usec,
           (long)(new_event->tv.tv_sec - now.tv_sec),
           (long)(new_event->tv.tv_usec - now.tv_usec));

    /* Set the timer if the new event is the next to occur */
    if (need_timer) {
        struct timeval interval;
        interval.tv_sec = new_event->tv.tv_sec - now.tv_sec;
        interval.tv_usec = new_event->tv.tv_usec - now.tv_usec;
        if (interval.tv_usec < 0) {
            interval.tv_sec -= 1;
            interval.tv_usec += 1000000;
        }
        if (interval.tv_sec < 0) {
            interval.tv_sec = 0;
            interval.tv_usec = 0;
        }

        /* Set up the timer */
        itv.it_interval = zero;
        itv.it_value = interval;
        if (setitimer(ITIMER_REAL, &itv, NULL) < 0) {
            perror("setitimer");
        }
    }

    /* Unblock SIGALRM */
    if (sigprocmask(SIG_UNBLOCK, &alarm_sigset, NULL) < 0) {
        perror("sigprocmask");
    }

    return new_event;
}

/* Schedule func to be run at an absolute time tv with data as arguments */
struct schedule_entry *aschedule(struct timeval tv, void (*func)(void *), void *data) {
    struct timeval now, interval;

    /* Get the current time */
    if (gettimeofday(&now, NULL) < 0) {
        perror("gettimeofday");
        return NULL;
    }

    /* Calculate the interval */
    interval.tv_sec = tv.tv_sec - now.tv_sec;
    interval.tv_usec = tv.tv_usec - now.tv_usec;
    if (interval.tv_usec < 0) {
        interval.tv_sec -= 1;
        interval.tv_usec += 1000000;
    }
    if (interval.tv_sec < 0) {
        interval.tv_sec = 0;
        interval.tv_usec = 0;
    }

    return schedule(interval, func, data);
}

/* Remove a scheduled event from the queue */
void deschedule(struct schedule_entry *s) {
    struct schedule_entry *current, *previous = NULL;

    if (!s) {
        return;
    }

    /* Block SIGALRM */
    if (sigprocmask(SIG_BLOCK, &alarm_sigset, NULL) < 0) {
        perror("sigprocmask");
        return;
    }

    /* Find and remove the event */
    current = events;
    while (current) {
        if (current == s) {
            if (previous) {
                previous->next = current->next;
            } else {
                events = current->next;
            }
            free(current);

            /* Log descheduling */
            char timestamp[20];
            get_timestamp(timestamp, sizeof(timestamp));
            printf("[%s] Task descheduled.\n", timestamp);

            break;
        }
        previous = current;
        current = current->next;
    }

    /* Unblock SIGALRM */
    if (sigprocmask(SIG_UNBLOCK, &alarm_sigset, NULL) < 0) {
        perror("sigprocmask");
    }
}
