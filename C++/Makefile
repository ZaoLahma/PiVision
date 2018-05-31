CCOMMAND = g++
CFLAGS = -Wall -c -Wextra -g --std=c++11
LINKARGS = -lpthread
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
RELEASE_SOURCES = $(wildcard src/main/*/*.cpp)
RELEASE_OBJECTS = $(RELEASE_SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
INC_DIRS = -I./inc -I./submodules/ThreadFramework/inc
COLOR_EXE_NAME = PiVision_COLOR
BW_EXE_NAME = PiVision_BW
FULL_EXE_NAME = PiVision_FULL

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_S),Linux)
ifeq ($(UNAME_M),armv7l)
	LINKARGS += ./submodules/ThreadFramework/lib_linux_arm/jobdispatcher.a
endif
ifeq ($(UNAME_M),x86_64)
	LINKARGS += ./submodules/ThreadFramework/lib_linux_x86/jobdispatcher.a
endif
endif

ifeq ($(UNAME_S),Darwin)
	LINKARGS += ./submodules/ThreadFramework/lib_osx/jobdispatcher.a
endif

$(FULL_EXE_NAME): RELEASE_SOURCES = $(wildcard src/main/full/*.cpp)
$(FULL_EXE_NAME): $(OBJECTS) $(RELEASE_OBJECTS)
	$(CCOMMAND) $(OBJECTS) $(RELEASE_OBJECTS) $(LINKARGS) -o $(FULL_EXE_NAME)

$(BW_EXE_NAME): RELEASE_SOURCES = $(wildcard src/main/bw/*.cpp)
$(BW_EXE_NAME): $(OBJECTS) $(RELEASE_OBJECTS)
	$(CCOMMAND) $(OBJECTS) $(RELEASE_OBJECTS) $(LINKARGS) -o $(BW_EXE_NAME)

$(COLOR_EXE_NAME): RELEASE_SOURCES = $(wildcard src/main/color/*.cpp)
$(COLOR_EXE_NAME): $(OBJECTS) $(RELEASE_OBJECTS)
	$(CCOMMAND) $(OBJECTS) $(RELEASE_OBJECTS) $(LINKARGS) -o $(COLOR_EXE_NAME)

%.o: %.cpp
	$(CCOMMAND) $(INC_DIRS) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(COLOR_EXE_NAME) $(BW_EXE_NAME) $(FULL_EXE_NAME) $(OBJECTS) $(RELEASE_OBJECTS) ./log.txt
