# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Executable name
TARGET = cell_detection

# Source files
# Blur.c
SRCS = main.c cbmp.c Blur.c\
       Step1GrayScale.c Step2BinaryThreshold.c Step3Erode.c \
       Step4Capture.c Step5MarkingCells.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

# Compile each .c into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program (optional)
run: $(TARGET)
	./$(TARGET) samples/easy/input.bmp output_images/output.bmp

.PHONY: all clean run