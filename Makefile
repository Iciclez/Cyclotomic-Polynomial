CC=g++
CFLAGS=-Wall -O3 -std=c++17
TARGET=cyclotomic_polynomial

all: $(TARGET)

$(TARGET).o: $(TARGET).cpp
	$(CC) $(CFLAGS) -c -o $@ $<

test:
	./$(TARGET)

clean:
	-@rm $(TARGET) $(TARGET).o 2> /dev/null || true
