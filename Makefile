# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g

# Target executable name
TARGET = scheduler

# Source files
SRCS = main.c scheduler.c

# Header files
HEADERS = scheduler.h

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Rule to link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .c files into .o files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets (not files)
.PHONY: all clean
