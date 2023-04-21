CC=gcc
CFLAGS=-Wall -Wextra -Werror -O3

SOURCES=lab11fokN32481.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=lab11fokN32481

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
