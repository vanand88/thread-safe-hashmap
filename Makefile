# Compiler flags
CC           = g++
CFLAGS  = -pthread
INCLUDE = 
LIB = 

# Project files
SRCS = main.cc
OBJS = $(SRCS:.cc=.o)
EXE  = thread-safe-hashmap

# Debug build settings
DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

# Release build settings
RELDIR = release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release

# Default build
all: prep release

# Debug rules
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) $(DBGOBJS) $(LIB) -o $(DBGEXE)

$(DBGDIR)/%.o: %.cc hashmap.h
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

# Release rules
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) $(RELOBJS) $(LIB) -o $(RELEXE)

$(RELDIR)/%.o: %.cc hashmap.h
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

# Other rules
prep:
	@mkdir -p $(DBGDIR) $(RELDIR)

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)
