# -----------------------------------------------------------------
# Cycles Makefile
# -----------------------------------------------------------------
# Programmer: Yuning Shi (yshi@psu.edu)
# -----------------------------------------------------------------

CC = gcc
CFLAGS = -g -O0

SRCDIR = ./src
INCDIR = ./src/include

SRCS_ =  Cycles.c Read_SimCtrl.c Read_Soil.c Read_Crop.c
HEADERS_ = Cycles.h

EXECUTABLE = Cycles
MSG = "...  Compiling Cycles  ..."

LIBS = -lm
INCLUDES = 
LFLAGS = 
SFLAGS = -D_DEBUG_

SRCS = $(patsubst %,$(SRCDIR)/%,$(SRCS_))
HEADERS = $(patsubst %,$(INCDIR)/%,$(HEADERS_))

OBJS = $(SRCS:.c=.o)

all:	$(EXECUTABLE)
	@echo
	@echo $(MSG)
	@echo
$(EXECUTABLE): $(OBJS) $(MODULE_OBJS)
	@$(CC) $(CFLAGS) $(SFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJS) $(MODULE_OBJS) $(LFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(SFLAGS) $(INCLUDES) -c $<  -o $@

.PHONY: clean

clean:
	$(RM) $(SRCDIR)/*.o  *~ Cycles
