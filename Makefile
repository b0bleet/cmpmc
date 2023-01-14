TARGET = cmpmc
LIBS = -lm -lpthread
CC = gcc
CFLAGS = -g -Wall -std=c11

.PHONY: default all clean

default: $(TARGET)
all: default

SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -I . -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

format:
	@for src in $(SOURCES) $(HEADERS) ; do \
		echo "formatting $$src" ; \
		clang-format -style LLVM -i "$$src" ; \
	done

clean:
	-rm -f *.o
	-rm -f $(TARGET)
