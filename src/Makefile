#debug = yes

TARGET = petrel

srcdir ?= .
debugdir = ../debug
releasedir = ../release
configdir = ..

ifeq ($(debug),yes)
	builddir ?= $(debugdir)
	CXXFLAGS += -DDEBUG -g
	checkrc := $(configdir)/$(notdir $(TARGET))2.rc
else
	builddir ?= $(releasedir)
	CXXFLAGS += -DNDEBUG
	checkrc := $(configdir)/$(notdir $(TARGET)).rc
endif

LIBS = -pthread
OPTIONS  = -std=c++0x -mssse3 -march=native -mtune=native
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

-include $(DEPS)

.PHONY: all check clean

all: $(builddir)/$(TARGET)

check: all
	$(builddir)/$(TARGET) $(checkrc)

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
