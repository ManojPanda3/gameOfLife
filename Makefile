CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -Iincludes/ -Llib/ -lm
SOURCES = src/*.c 
TARGET = build/run

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $(TARGET)  $(LDFLAGS)

# Clean the build
clean:
	rm -f $(TARGET)
