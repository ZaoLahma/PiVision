print-%: ; @echo $*=$($*)

CCOMMAND = g++
CFLAGS = -Wall -c -Wextra --std=c++11 
LINKARGS = -L/usr/X11/lib -lX11 -lpthread
SRC_FILES = ./src/*.cpp
INC_DIRS = -I./inc -I/usr/local/include
EXE_NAME = PiVision

UNAME_M := $(shell uname -m)
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_M),x86_64)
CCFLAGS += -D AMD64
endif
ifneq ($(filter arm%,$(UNAME_M)),)
CCFLAGS += -D ARM
LINKARGS += ./lib/arm/jobdispatcher.a
else ifeq ($(UNAME_S),Linux)
LINKARGS += ./lib/linux_amd64/jobdispatcher.a
else ifeq ($(UNAME_S),Darwin)
LINKARGS += ./lib/osx/jobdispatcher.a
endif

all: compile link

compile:
	$(CCOMMAND) $(CFLAGS) $(INC_DIRS) $(SRC_FILES)
	
link:
	$(CCOMMAND) -o $(EXE_NAME) ./*.o $(LINKARGS)
	
clean:
	rm -rf ./*.o
	rm ./$(EXE_NAME)