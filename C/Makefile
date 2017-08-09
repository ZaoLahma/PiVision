CCOMMAND = gcc
CFLAGS = -Wall -c -Wextra -g
LINKARGS = -lpthread
SOURCES = $(wildcard src/*.c)
SOURCES += $(wildcard src/external/*.c)
OBJECTS = $(SOURCES:.c=.o)
INC_DIRS = -I./inc -I./inc/external
EXE_NAME = PiVision

$(EXE_NAME): $(OBJECTS)
	$(CCOMMAND) $(OBJECTS) $(LINKARGS) -o $(EXE_NAME)

%.o: %.c
	$(CCOMMAND) $(INC_DIRS) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE_NAME) $(OBJECTS)
