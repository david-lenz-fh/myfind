CC = gcc
CFLAGS = -Wall -Wextra

TARGET = myfind

SRCS = main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean: 
	rm -f $(TARGET)
