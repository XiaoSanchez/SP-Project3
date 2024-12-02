# Scheduler Program README

## Overview

This document provides an in-depth explanation of the Scheduler Program, a simple time-based task scheduler written in C. The program allows users to schedule functions to execute at specified times or intervals, using POSIX signals and timers. The Scheduler Program has been enhanced with additional output to improve visualization and understanding of its operation.

## Features

- **Task Scheduling**: Schedule functions to run after a certain time interval or at a specific absolute time.
- **Periodic Execution**: Easily reschedule tasks to run periodically.
- **Signal Handling**: Uses `SIGALRM` to handle timing events.
- **Event Queue Management**: Maintains an event queue sorted by execution time.
- **Enhanced Logging**: Provides detailed log messages with timestamps for better visualization of the scheduler's behavior.

## Getting Started

### Prerequisites

- **C Compiler**: A C99-compatible compiler (e.g., GCC).
- **Make Utility**: For building the program using the provided Makefile.
- **POSIX-Compliant System**: The program relies on POSIX APIs and may not work on non-POSIX systems.

### Compilation

To compile the Scheduler Program, use the provided Makefile:

1. **Navigate to the Program Directory**:

   Open a terminal and navigate to the directory containing the program files.

2. **Compile the Program**:

   Run the following command to compile the program:

   ```sh
   make
   ```

   This command will compile the source files and create an executable named `scheduler`.

### Running the Program

To run the Scheduler Program, execute the following command:

```sh
./scheduler
```

Upon execution, the program initializes the scheduler, schedules the initial task, and enters the main loop. You will see log messages printed to the console, indicating the scheduler's activity.

## Program Output

The program provides detailed log messages to visualize its operation. Here's what you can expect:

- **Initialization**:

  ```
  ------ Improved Scheduler -------
  [YYYY-MM-DD HH:MM:SS] Scheduling initial task to run in X seconds.
  [YYYY-MM-DD HH:MM:SS] Scheduled new task to run at T (in X.XXXXXX seconds).
  [YYYY-MM-DD HH:MM:SS] Setting timer for next event in X.XXXXXX seconds.
  ```

  - Displays the initial scheduling of the task.

- **Task Execution**:

  ```
  [YYYY-MM-DD HH:MM:SS] Executing scheduled task.
  [YYYY-MM-DD HH:MM:SS] Task is running.
  [YYYY-MM-DD HH:MM:SS] Scheduled new task to run at T (in X.XXXXXX seconds).
  [YYYY-MM-DD HH:MM:SS] Setting timer for next event in X.XXXXXX seconds.
  [YYYY-MM-DD HH:MM:SS] Task rescheduled to run in X seconds.
  ```

  - Indicates when a task is executed and rescheduled.

- **Event Queue Status**:

  - The program can be extended to display the current state of the event queue for additional clarity.

## Program Structure

### Main Components

1. **Scheduler Initialization**:

   - Sets up the signal handler for `SIGALRM`.
   - Initializes the event list and signal masks.

2. **Scheduling Functions**:

   - **`schedule`**: Schedules a function to run after a relative time interval.
   - **`aschedule`**: Schedules a function to run at an absolute time.
   - **`deschedule`**: Cancels a scheduled function.

3. **Signal Handling**:

   - **`alarm_handler`**: Handles `SIGALRM` signals, executes scheduled tasks whose time has come, and resets the timer for future tasks.

4. **Main Function**:

   - Initializes the scheduler.
   - Schedules the initial task.
   - Enters an infinite loop to keep the program running and waits for signals.

### Flow of Execution

1. **Initialization**:

   - The program starts by initializing the scheduler, setting up signal handlers, and preparing the event queue.

2. **Scheduling the Initial Task**:

   - The initial task (`func_test`) is scheduled to run after a specified interval (e.g., 5 seconds).
   - A log message indicates the scheduling.

3. **Main Loop**:

   - The program enters an infinite loop using `pause()` to wait for signals.
   - This approach is efficient and minimizes CPU usage.

4. **Signal Handling**:

   - When the scheduled time arrives, `SIGALRM` is sent to the process.
   - The `alarm_handler` function is invoked to handle the signal.
   - The handler executes due tasks and resets the timer for the next event.

5. **Task Execution**:

   - The scheduled function (`func_test`) performs its designated action.
   - In this program, it sets a flag to indicate that an event occurred.
   - The main loop detects this flag, logs the task execution, and reschedules the task.

6. **Rescheduling**:

   - The task is rescheduled to run again after the specified interval.
   - Log messages indicate the rescheduling and next execution time.

## Customization

### Adjusting the Task Interval

To change how often the scheduled task runs:

1. **Locate the Interval Setting**:

   - In the main function, find where the interval is set for the task.

2. **Modify the Interval**:

   - Change the `tv.tv_sec` and `tv.tv_usec` values to adjust the seconds and microseconds, respectively.

   - For example, to run the task every 2 seconds:

     - Set `tv.tv_sec = 2;`
     - Set `tv.tv_usec = 0;`

3. **Recompile the Program**:

   - After making changes, recompile the program using `make`.

### Customizing the Scheduled Task

To modify the action performed by the scheduled task:

1. **Locate the Task Function**:

   - The task function is typically named `func_test` or similar.

2. **Modify the Task Function**:

   - Replace the existing code within the function with the desired operations.
   - Ensure that any required variables or resources are properly managed.

3. **Rescheduling**:

   - If the task should run periodically, ensure it reschedules itself at the end of its execution.

4. **Recompile the Program**:

   - Recompile the program to apply the changes.

## Enhanced Logging and Visualization

The program includes enhanced logging to improve visualization:

- **Timestamps**:

  - All log messages include a timestamp in the format `[YYYY-MM-DD HH:MM:SS]`.

- **Detailed Messages**:

  - Messages provide information about scheduling, execution, rescheduling, and timer settings.

- **Consistent Format**:

  - Logs are formatted consistently for easy reading and parsing.

- **Event Queue (Optional Extension)**:

  - The program can be extended to display the current state of the event queue.
  - This feature helps visualize the tasks waiting to be executed.

## Error Handling and Safety

- **Signal Safety**:

  - The program carefully manages signal blocking and unblocking to prevent reentrancy issues.
  - Critical sections where the event list is modified are protected using `sigprocmask`.

- **Error Checking**:

  - System calls and library functions are checked for errors.
  - Appropriate error messages are printed using `perror` or `fprintf`.

- **Memory Management**:

  - Dynamically allocated memory for event entries is managed carefully.
  - Memory is freed after tasks are executed or descheduled.

- **Async-Signal-Safe Practices**:

  - The signal handler avoids calling non-async-signal-safe functions.
  - Output within the signal handler is minimized to prevent issues.

## Compilation and Execution Tips

- **Ensure Proper Permissions**:

  - The executable may need execute permissions. Use `chmod +x scheduler` if necessary.

- **Clean Build**:

  - If encountering issues, run `make clean` before recompiling.

- **Running in the Background**:

  - Use `./scheduler &` to run the program in the background.
  - Use `nohup ./scheduler &` to prevent the program from terminating when the terminal is closed.

- **Redirecting Output**:

  - Output can be redirected to a file using `./scheduler > output.log 2>&1`.

## Debugging

- **Using a Debugger**:

  - The program includes debugging symbols if compiled with the `-g` flag.
  - Use `gdb ./scheduler` to start debugging.

- **Setting Breakpoints**:

  - Breakpoints can be set in functions like `func_test` or `alarm_handler`.

- **Monitoring Variables**:

  - Inspect variables and program state to diagnose issues.

## Common Issues and Solutions

- **No Output**:

  - Ensure that the task function is being called and that logging statements are present.

- **Compilation Errors**:

  - Verify that all files are up to date and that the required headers and libraries are available.

- **Signals Not Handled**:

  - Check that `init_scheduler` is correctly setting up signal handlers.
  - Ensure that no other parts of the program interfere with `SIGALRM`.

## Conclusion

The Scheduler Program provides a foundational example of how to implement a simple, time-based task scheduler in C using POSIX signals and timers. With enhanced logging and careful management of signals and memory, it serves as a solid starting point for more complex scheduling needs.

Feel free to modify and extend the program to suit your specific requirements. Always remember to recompile after making changes and test thoroughly to ensure reliability.