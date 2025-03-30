CC = g++
CFLAGS = -O3 -Wall -fopenmp

SRC = matrix_v1.cpp matrix_v2.cpp matrix_v3.cpp
TARGET = $(SRC:.cpp=)

all: $(TARGET)

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET)