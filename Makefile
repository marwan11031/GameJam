CC = gcc
CFLAGS = -Wextra -Wall -std=c99
LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11
TARGET = game

SRC = $(wildcard *.c)
OBJS = $(SRC: .c=.o)


$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)


%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean

clean:
	rm -f $(OBJS) $(TARGET)
