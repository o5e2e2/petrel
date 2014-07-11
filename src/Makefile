#debug = yes

TARGET = petrel

srcdir ?= .
debugdir = ../debug
releasedir = ../release

ifeq ($(debug),yes)
	builddir ?= $(debugdir)
	CXXFLAGS += -DDEBUG -g
	checkrc := $(srcdir)/$(notdir $(TARGET))2.rc
else
	builddir ?= $(releasedir)
	CXXFLAGS += -DNDEBUG
	checkrc := $(srcdir)/$(notdir $(TARGET)).rc
endif

HEADER = StdAfx.hpp
LIBS = -pthread
OPTIONS = -std=c++0x -mssse3 -march=core2 -mtune=native
OPTIMIZATIONS = -Ofast -flto -fno-exceptions -fno-rtti -fno-common
WARNINGS += -pedantic -Wall -Wextra -Wuninitialized -Wconversion -Wpointer-arith -Wcast-qual -Wcast-align -Wshadow -Wwrite-strings
WARNINGS += -Wsign-conversion
#WARNINGS = -Werror

#CXX=g++
#RM=rm -f

CXXFLAGS += $(OPTIONS)
CXXFLAGS += $(OPTIMIZATIONS)
CXXFLAGS += $(WARNINGS)
LDFLAGS += $(LIBS)
LDFLAGS += $(OPTIMIZATIONS)

SOURCES := $(wildcard $(srcdir)/*.cpp)
OBJECTS := $(patsubst $(srcdir)/%.cpp, $(builddir)/%.o, $(SOURCES))
DEPS := $(patsubst %.o, %.d, $(OBJECTS))
PRECOMP := $(builddir)/$(HEADER).gch
HEADER := $(srcdir)/$(HEADER)

.PHONY: all clean check

all: $(builddir)/$(TARGET)

check: all
	$(builddir)/$(TARGET) $(checkrc)

$(builddir)/$(TARGET): $(PRECOMP) $(OBJECTS)
	$(CXX) -o $@ $(LDFLAGS) $(OBJECTS)

$(builddir)/%.o: $(srcdir)/%.cpp $(PRECOMP)
	$(CXX) $< -c -o $@ -MMD $(CXXFLAGS) -Winvalid-pch -include $(HEADER)

$(PRECOMP): $(HEADER) | $(builddir)
	$(CXX) $< -o $@ -MD $(CXXFLAGS)

$(builddir):
	mkdir -p $@

clean:
	$(RM) -r *.o *.gch *.d $(debugdir) $(releasedir)

-include $(DEPS)
