#debug = yes

GIT_DATE = $(shell git log -1 --date=short --pretty=format:%cd)
GIT_HASH = $(shell git log -1 --date=short --pretty=format:%h)
GIT_ORIGIN = $(shell git remote get-url origin)

SRC_DIR ?= ./src
DEBUG_DIR ?= ./debug
RELEASE_DIR ?= ./release
TEST_DIR ?= ./test

ifeq ($(debug),yes)
	BUILD_DIR = $(DEBUG_DIR)
	CXXFLAGS += -DDEBUG -g
else
	BUILD_DIR = $(RELEASE_DIR)
	CXXFLAGS += -DNDEBUG
endif

TARGET ?= $(BUILD_DIR)/petrel
EXPECT ?= $(TEST_DIR)/expect.sh

LIBS = -pthread
OPTIONS  = -std=c++11 -mssse3 -march=native -mtune=native
OPTIONS += -fno-rtti -fno-common -fno-exceptions

OPTIMIZATIONS = -Ofast -flto -finline-functions -funroll-all-loops
WARNINGS += -pedantic -Wall -Wextra -Wuninitialized -Wpointer-arith -Wcast-qual -Wcast-align
WARNINGS += -Wconversion -Wshadow

CXXFLAGS += $(OPTIONS) $(OPTIMIZATIONS) $(WARNINGS)
CXXFLAGS += -DGIT_DATE=\"$(GIT_DATE)\" -DGIT_HASH=\"$(GIT_HASH)\" -DGIT_ORIGIN=\"$(GIT_ORIGIN)\"
LDFLAGS += $(LIBS) $(OPTIMIZATIONS) -Wl,--no-as-needed

HEADER = StdAfx.hpp
PRECOMP = $(BUILD_DIR)/$(HEADER).gch
HEADER_SRC = $(SRC_DIR)/$(HEADER)

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
DEPS = $(patsubst %.o, %.d, $(OBJECTS))

.PHONY: all test test-hash clean

all: $(TARGET)

test: all
	$(EXPECT) $(TARGET) $(TEST_DIR)/test.rc

test-hash: all
	$(EXPECT) $(TARGET) $(TEST_DIR)/test-hash.rc

clean:
	$(RM) -r $(DEBUG_DIR) $(RELEASE_DIR)

$(TARGET): $(PRECOMP) $(OBJECTS)
	$(CXX) -o $@ $(LDFLAGS) $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(PRECOMP)
	$(CXX) -c -o $@ $< -MMD $(CXXFLAGS) -Winvalid-pch -include $(HEADER_SRC)

$(PRECOMP): $(HEADER_SRC) | $(BUILD_DIR)
	$(CXX) -o $@ $< -MD $(CXXFLAGS)

$(BUILD_DIR):
	mkdir -p $@

-include $(DEPS)
