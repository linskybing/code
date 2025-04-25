CC = g++
CFLAGS = -O0 -Wall

SRC = $(wildcard *.cpp)
TARGET = $(SRC:.cpp=)

all: $(TARGET)

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@.o

.PHONY: clean

clean:
	rm -f $(TARGET)