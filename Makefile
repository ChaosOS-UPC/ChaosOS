# ChaosOS — common Makefile
# Builds only the modules whose source file already exists in src/,
# so it works while the modules are still being written.

CC      = gcc
CFLAGS  = -Wall -Wextra -g
# Add -fopenmp to LDLIBS (and CFLAGS) if a module uses OpenMP
LDLIBS  = -pthread

PROGS   = chaos_launcher mouse_troll win_troll alert_troll update_troll

BINS := $(foreach p,$(PROGS),$(if $(wildcard src/$(p).c),$(p)))
LIBS := $(if $(wildcard src/printf_troll.c),lib_printf.so) \
        $(if $(wildcard src/key_troll.c),lib_key.so)

.PHONY: all clean
all: $(LIBS) $(BINS)

# LD_PRELOAD shared libraries
lib_printf.so: src/printf_troll.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< -ldl

lib_key.so: src/key_troll.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< -ldl

# Worker processes and orchestrator
%: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f *.so $(PROGS)
