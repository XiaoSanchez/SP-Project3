/* main.c
 * Improved Scheduler Main Function with Enhanced Output
 *
 * This software is distributed under the terms
 * of the GPL, which you should have received
 * along with this source.
 *
 * Example usage of the improved scheduler with additional output
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "scheduler.h"

/* Flag to indicate that an event has occurred */
volatile sig_atomic_t event_occurred = 0;

/* Test function to be scheduled */
void func_test(void *data) {
    (void)data; // Suppress unused parameter warning

    /* Set the flag to indicate that the event occurred */
    event_occurred = 1;
}

/* Function to get a formatted timestamp */
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_now);
}

/* Main function */
int main(void) {
    struct timeval tv;
    char timestamp[20];

    printf("------ Improved Scheduler -------\n");

    /* Initialize the scheduler */
    init_scheduler();

    /* Schedule func_test to run every 5 seconds */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    /* Get the current timestamp */
    get_timestamp(timestamp, sizeof(timestamp));
    printf("[%s] Scheduling initial task to run in %ld seconds.\n", timestamp, tv.tv_sec);

    if (!schedule(tv, func_test, NULL)) {
        fprintf(stderr, "Failed to schedule func_test\n");
        exit(EXIT_FAILURE);
    }

    /* Main loop */
    while (1) {
        /* Use pause() to wait for signals */
        pause();

        /* Check if an event occurred */
        if (event_occurred) {
            /* Get the current timestamp */
            get_timestamp(timestamp, sizeof(timestamp));
            printf("[%s] Task is running.\n", timestamp);

            /* Reschedule the task */
            if (!schedule(tv, func_test, NULL)) {
                fprintf(stderr, "Failed to reschedule func_test\n");
                exit(EXIT_FAILURE);
            }

            /* Log rescheduling */
            get_timestamp(timestamp, sizeof(timestamp));
            printf("[%s] Task rescheduled to run in %ld seconds.\n", timestamp, tv.tv_sec);

            /* Reset the flag */
            event_occurred = 0;
        }
    }

    return 0;
}
