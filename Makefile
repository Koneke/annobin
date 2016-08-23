CC = gcc
CFLAGS = -Wall -c

TARGET = $(notdir $(CURDIR))
SOURCES := source
INCLUDES := include

CFILES = $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
INCLUDE = $(foreach dir,$(INCLUDES),-I$(dir))

.PHONY: all

all:
	$(CC) -std=c99 -o ./$(TARGET).exe $(CFILES) $(INCLUDE) -lcurses

clean:
	rm $(TARGET).exe
