#debug = yes

TARGET = petrel

srcdir ?= ./src
debugdir = ./debug
releasedir = ./release
testdir = ./test

ifeq ($(debug),yes)
	builddir ?= $(debugdir)
	CXXFLAGS += -DDEBUG -g
else
	builddir ?= $(releasedir)
	CXXFLAGS += -DNDEBUG
endif

LIBS = -pthread
OPTIONS  = -std=c++11 -mssse3 -march=native -mtune=native
OPTIONS += -finline-functions -funroll-all-loops

OPTIMIZATIONS = -Ofast -flto -fno-rtti -fno-common
OPTIMIZATIONS += -fno-exceptions
WARNINGS += -pedantic -Wall -Wextra -Wuninitialized -Wpointer-arith -Wcast-qual -Wcast-align
#WARNINGS += -Wsign-conversion
WARNINGS += -Wconversion -Wshadow

CXXFLAGS += $(OPTIONS) $(OPTIMIZATIONS) $(WARNINGS)
LDFLAGS += $(LIBS) $(OPTIMIZATIONS) -Wl,--no-as-needed

HEADER = StdAfx.hpp
PRECOMP := $(builddir)/$(HEADER).gch
HEADER := $(srcdir)/$(HEADER)

SOURCES := $(wildcard $(srcdir)/*.cpp)
OBJECTS := $(patsubst $(srcdir)/%.cpp, $(builddir)/%.o, $(SOURCES))
DEPS := $(patsubst %.o, %.d, $(OBJECTS))

.PHONY: all check clean

all: $(builddir)/$(TARGET)

test: all
	$(testdir)/test.sh $(builddir)/$(TARGET) $(testdir)/$(TARGET).rc

clean:
	$(RM) -r *.o *.gch *.d $(debugdir) $(releasedir)

$(builddir)/$(TARGET): $(PRECOMP) $(OBJECTS)
	$(CXX) -o $@ $(LDFLAGS) $(OBJECTS)

$(builddir)/%.o: $(srcdir)/%.cpp $(PRECOMP)
	$(CXX) $< -c -o $@ -MMD $(CXXFLAGS) -Winvalid-pch -include $(HEADER)

$(PRECOMP): $(HEADER) | $(builddir)
	$(CXX) $< -o $@ -MD $(CXXFLAGS)

$(builddir):
	mkdir -p $@

-include $(DEPS)
