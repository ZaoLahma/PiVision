CCOMMAND = gcc
CFLAGS = -Wall -c -Wextra -g
LINKARGS = -lpthread
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)
INC_DIRS = -I./inc
EXE_NAME = PiVision

$(EXE_NAME): $(OBJECTS)
	$(CCOMMAND) $(OBJECTS) $(LINKARGS) -o $(EXE_NAME)

%.o: %.cpp
	$(CCOMMAND) $(INC_DIRS) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE_NAME) $(OBJECTS)
