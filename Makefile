#-----------------------------------------------------
#----------------------[ Target ]---------------------
#-----------------------------------------------------
TARGET = isp15xx
OBJECT = ./Object
DEBUG := 1

#-----------------------------------------------------
#---------------------[ Options ]---------------------
#-----------------------------------------------------
ifeq ($(DEBUG),)
OPT = -O2
STRIP = strip
else
OPT = -O0 -g -DDEBUG=1
STRIP = true
endif

SILENT = @
PATCH = patch
ECHO = @echo
CC = gcc
AR = ar
RM = rm -f
CXX = g++
LDFLAGS = -L/usr/local/lib -lpthread

CFLAGS = $(OPT) -std=c++0x -Wall -Wextra -Wno-unused-parameter \
 -Wshadow -Wpointer-arith -Wcast-qual -Wformat-security \
 -Werror=format-security -Werror=format -DLINUX

RM = rm -f
CXXFLAGS = $(CFLAGS)

SOURCES = Alarm.cc \
		  Binary.cc \
		  Client.cc \
		  CmdLine.cc \
		  Elf32.cc \
		  iHex.cc \
		  ISP.cc \
		  LED.cc \
		  Log.cc \
		  Main.cc \
		  Mutex.cc \
		  Serial.cc \
		  Signal.cc \
		  Utility.cc
OBJECTS = $(patsubst %.cc,$(OBJECT)/%.o,$(SOURCES))

all: $(TARGET)

$(OBJECT)/%.o: %.cc
	$(ECHO) "Compiling $<" 
	$(SILENT)$(CXX) $(CXXFLAGS) -c $< -o $@


#- - - - - - - - - - - - - - - - - - - - -
# Link it all together into an executable
#- - - - - - - - - - - - - - - - - - - - -
$(TARGET): $(OBJECTS)
	$(ECHO) "Linking $@"
	$(SILENT)$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
ifeq ($(strip $(DEBUG)),)
	$(SILENT)$(STRIP) $(TARGET)
endif
	$(SILENT)chgrp dialout $@

#-----------------------------------------------------
#---------------------[ Depend ]----------------------
#-----------------------------------------------------
depend: .depend

.depend: $(SOURCES)
	$(ECHO) "Generating dependencies"
	$(SILENT)mkdir -p Object
	$(SILENT)$(RM) .depend
	$(SILENT)$(CXX) $(CXXFLAGS) -MM $^ >> .tmp;
	$(SILENT)$(SHELL) -ec "sed 's/\(^[ ]*.o\)/Object\//' .tmp > .depend"
	$(SILENT)$(RM) .tmp > /dev/null

#-----------------------------------------------------
#---------------------[ Install ]---------------------
#-----------------------------------------------------
install:
	$(ECHO) "Installing"
	$(SILENT)cp ./isp15xx /usr/local/bin
	$(SILENT)$(STRIP) /usr/local/bin/isp15xx
	$(SILENT)chmod +x /usr/local/bin/isp15xx

#-----------------------------------------------------
#----------------------[ Clean ]----------------------
#-----------------------------------------------------
clean:
	$(ECHO) "Cleaning"
	$(SILENT)$(RM) $(OBJECTS) $(TARGET)

distclean: clean
	$(SILENT)$(RM) *~ .depend

include .depend

