CC = gcc
CFLAGS = -Wall -Wextra

TARGET = myfind

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) main.o -o $(TARGET)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o
clean: 
	rm -f $(TARGET) main.o
