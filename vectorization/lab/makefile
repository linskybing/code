CC = g++
CFLAGS = -O3 -march=native
CXXFLAGS = $(CFLAGS)

BIN = gen main float-diff
all: $(BIN)

gen: gen.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

float-diff: float-diff.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

main: main.cpp
	$(CC) $(CFLAGS) -mavx2 $< -o $@

.PHONY: clean

clean:
	rm -f $(BIN)
	rm -f *.bin