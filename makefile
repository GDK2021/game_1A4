# Makefile for SDL2 game project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -O2 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm

# Source files
SRCS = main.c functions.c mission1.c serie.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = game

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile C files to objects
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Rebuild everything
re: clean all

.PHONY: all clean re
