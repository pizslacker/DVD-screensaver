# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

# Project files
TARGET = dvd-screensaver
SRC = dvd-screensaver.c

# --- macOS Homebrew Users ---
# If you are on an Apple Silicon Mac (M1/M2/M3) using Homebrew, 
# uncomment the next two lines so the compiler can find SDL2:
# CFLAGS += -I/opt/homebrew/include
# LDFLAGS += -L/opt/homebrew/lib

# Default target
all: $(TARGET)

# Compile the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)

# Compile and run immediately
run: $(TARGET)
	./$(TARGET)